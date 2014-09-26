/****************************************************************************
  FileName     [ v3MCExtract.cpp ]
  PackageName  [ v3/src/v3mc ]
  Synopsis     [ V3 Constraint and Invariant Extraction Utility. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2014 DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_MC_EXTRACT_C
#define V3_MC_EXTRACT_C

#include "v3MCMain.h"
#include "v3NtkUtil.h"
#include "v3NumUtil.h"
#include "v3SvrBase.h"
#include "v3StgExtract.h"
#include "v3AlgSimulate.h"

// V3 Constraint and Invariant Extraction Functions
V3NtkHandler* const extractFSMInvariants(V3NtkHandler* const handler, const double& maxTime) {
   // This Function Extracts Invariants and Combines them with Outputs
   assert (handler); assert (maxTime > 0);
   V3Ntk* const ntk = handler->getNtk(); assert (ntk);
   if (!ntk->getOutputSize()) return handler;
   // Create FSM Extraction Handler
   V3NetVec targetNets; targetNets.clear(); targetNets.reserve(ntk->getOutputSize());
   for (uint32_t i = 0; i < ntk->getOutputSize(); ++i) targetNets.push_back(ntk->getOutput(i));
   V3FSMExtract* const fsmExtract = new V3FSMExtract(handler, targetNets); assert (fsmExtract);
   // Set FSM Extraction Configurations
   //fsmExtract->setVariableClusterBySCC();
   fsmExtract->setInvariantDirectedCluster();
   //fsmExtract->enableConstructStandardFSM();
   fsmExtract->setMaxTimeUsage(maxTime);
   // Set Currently User-Uncontrollable Configurations
   fsmExtract->filterOutNoMuxVariable();
   fsmExtract->filterOutNonSeqVariable();
   fsmExtract->enableOmitNodesAllCombVars();
   //fsmExtract->enableOmitNodesIndepToVars();
   //fsmExtract->enableStopOnAllInitOrTerms();
   fsmExtract->enableRenderNonEmptyStates();
   // Set Terminal States if Property is Specified
   if (ntk->getOutputSize() == 1) fsmExtract->pushTerminalState(targetNets[0]);
   // Set Maximum State Size for Each FSM
   fsmExtract->setMaxStateSize(50);
   // Start Extracting FSMs
   fsmExtract->startExtractFSMs();
   if (v3MCInteractive) {
      V3VrfShared::printLock();
      cerr << "TOTALLY " << fsmExtract->getFSMListSize() << " FSMs Extracted !!" << endl;
      V3VrfShared::printUnlock();
   }
   // Create Property Elaboration Handler
   V3NtkElaborate* const elabNtk = new V3NtkElaborate(handler, targetNets); assert (elabNtk);
   assert (elabNtk->getNtk()); assert (elabNtk->getNtk()->getOutputSize() == 0);
   for (uint32_t i = 0; i < ntk->getOutputSize(); ++i) {
      elabNtk->getNtk()->createOutput(elabNtk->getCurrentNetId(ntk->getOutput(i)));
      for (uint32_t j = 0; j < fsmExtract->getFSMListSize(); ++j) {
         V3FSM* const fsm = fsmExtract->getFSM(j); assert (fsm); if (!fsm->isCompleteFSM()) continue;
         // Update the Bad Signal for Backward Reachability Analysis
         //fsm->updateExtractedFSM(V3NetVec(1, ntk->getOutput(i))); assert (fsm);
         assert (V3NetUD != elabNtk->getCurrentNetId(ntk->getOutput(i)));
         elabNtk->combineFSMInvariantsToOutputs(i, fsm);
      }
      // Rename Property Output
      elabNtk->resetOutName(i, handler->getOutputName(i));
   }
   assert (ntk->getOutputSize() == elabNtk->getNtk()->getOutputSize()); return elabNtk;
}

void strengthenFairnessConstraints(V3NtkHandler* const handler, const V3NetVec& invConstr, V3NetVec& fairConstr) {
   // This Function Detects Fairness Constraints (i.e. FGp that holds)
   // According to the K-Liveness Paper in FMCAD 2013.
   assert (handler); V3Ntk* const ntk = handler->getNtk(); assert (ntk);
   const bool isBvNtk = dynamic_cast<V3BvNtk*>(ntk);
   const V3GateType conjType = (isBvNtk) ? BV_AND : AIG_NODE;
   // Initialize Solver
   V3SvrBase* solver = allocSolver(V3VrfBase::getDefaultSolver(), ntk); assert (solver);
   V3SvrDataVec formula; formula.clear(); formula.reserve(2); uint32_t d = 0, e = 1;
   // Split Bit-Vector Nets into Bits
   const uint32_t netSize = ntk->getNetSize();
   for (V3NetId id = V3NetId::makeNetId(1); id.id < netSize; ++id.id) {
      uint32_t width = ntk->getNetWidth(id); if (1 == width) continue;
      V3NetId id1; V3InputVec inputs(2, 0); assert (isBvNtk);
      for (uint32_t i = 0; i < width; ++i) {
         inputs[0] = id; inputs[1] = V3BvNtk::hashV3BusId(i, i);
         id1 = ntk->createNet(); ntk->setInput(id1, inputs); ntk->createGate(BV_SLICE, id1);
      }
   }
   // Initialize Candidate Nets to be All Nets and their Negations
   double curTime = clock(); uint32_t fair = 0;
   const bool latchOnly = (ntk->getNetSize() > 100000);
   V3NetVec candidate; candidate.clear(); candidate.reserve(ntk->getNetSize() << 1);
   for (V3NetId id = V3NetId::makeNetId(1); id.id < ntk->getNetSize(); ++id.id) {
      if (ntk->getNetWidth(id) != 1) continue;
      if (latchOnly) { if (V3_FF == ntk->getGateType(id)) candidate.push_back(id); }
      else { if (V3_FF <= ntk->getGateType(id)) candidate.push_back(id); }
   }
   // Add Invariant Constraints to Solver
   if (invConstr.size()) {
      solver->assertBoundedVerifyData(invConstr, d);
      solver->assertBoundedVerifyData(invConstr, e);
   }
   // Start Extracting Fairness Constraints
   uint32_t size = candidate.size(); if (!V3SafeMult(size, (uint32_t)(1 + fairConstr.size()))) size = V3NtkUD;
   V3NetId id, id1, id2, id3; V3InputVec inputs(2, V3NetUD); uint32_t newSize, iter = 0;
   while (true) {
      const uint32_t maxIter = (size > 100000) ? 1 : (size > 10000) ? 3 : (size > 1000) ? 5 : V3NtkUD;
      const bool randON = (size > 3000); if (++iter > maxIter) break;
      for (uint32_t i = 0; i < candidate.size(); ++i) {
         if (randON && (3000 <= (rand() % candidate.size()))) continue;
         // Check Stabilizing Constraint (id --> next(id))
         id = candidate[i]; assert (id.id < ntk->getNetSize());
         if (!solver->existVerifyData(id, d)) solver->addBoundedVerifyData(id, d);
         if (!solver->existVerifyData(id, e)) solver->addBoundedVerifyData(id, e);
         // Check if (id && next(~id)) is UNSAT
         solver->assumeRelease(); solver->assumeProperty(id, false, d); solver->assumeProperty(id, true, e);
         if (solver->assump_solve()) {
            solver->assumeRelease(); solver->assumeProperty(id, true, d); solver->assumeProperty(id, false, e);
            if (solver->assump_solve()) continue;
         }
         ++fair;
         // Add Constraint (id == next(id)) to the Model
         formula.clear(); formula.push_back(solver->getFormula( id, d));
         formula.push_back(solver->getFormula(~id, e)); solver->assertImplyUnion(formula);
         formula.clear(); formula.push_back(solver->getFormula(~id, d));
         formula.push_back(solver->getFormula( id, e)); solver->assertImplyUnion(formula);
         // Update Fairness Constraints f to be (f && (id == next(id)))
         if (V3_FF == ntk->getGateType(id)) {
            if (isBvNtk) {
               id3 = ntk->createNet(); assert (V3NetUD != id3);
               inputs[0] = id; inputs[1] = ntk->getInputNetId(id, 0);
               ntk->setInput(id3, inputs); ntk->createGate(BV_EQUALITY, id3);
            }
            else {
               id1 = ntk->createNet(); assert (V3NetUD != id1);
               inputs[0] =  id; inputs[1] = ~(ntk->getInputNetId(id, 0));
               ntk->setInput(id1, inputs); ntk->createGate(AIG_NODE, id1);
               id2 = ntk->createNet(); assert (V3NetUD != id2);
               inputs[0] = ~id; inputs[1] =  ntk->getInputNetId(id, 0);
               ntk->setInput(id2, inputs); ntk->createGate(AIG_NODE, id2);
               id3 = ntk->createNet(); assert (V3NetUD != id3);
               inputs[0] = ~id1; inputs[1] = ~id2;
               ntk->setInput(id3, inputs); ntk->createGate(AIG_NODE, id3);
            }
            for (uint32_t j = 0; j < fairConstr.size(); ++j) {
               id2 = ntk->createNet(); assert (V3NetUD != id2); inputs[0] = fairConstr[j]; inputs[1] = id3;
               ntk->setInput(id2, inputs); ntk->createGate(conjType, id2); fairConstr[j] = id2;
            }
         }
         // Checking Fairness Constraint FG(id --> f) and FG(~id --> f)
         for (uint32_t j = 0; j < fairConstr.size(); ++j) {
            id1 = fairConstr[j]; assert (id1.id < ntk->getNetSize()); solver->update();
            if (!solver->existVerifyData(id1, d)) solver->addBoundedVerifyData(id1, d);
            // Check if (id && id1) is UNSAT
            solver->assumeRelease(); solver->assumeProperty(id, false, d); solver->assumeProperty(id1, false, d);
            if (!solver->assump_solve()) {
               // Update Fairness Constraint f to be (f && ~id)
               id2 = ntk->createNet(); assert (V3NetUD != id2); inputs[0] = id1; inputs[1] = ~id;
               ntk->setInput(id2, inputs); ntk->createGate(conjType, id2); fairConstr[j] = id2; continue;
            }
            // Check if (~id && id1) is UNSAT
            solver->assumeRelease(); solver->assumeProperty(id, true, d); solver->assumeProperty(id1, false, d);
            if (!solver->assump_solve()) {
               // Update Fairness Constraint f to be (f && id)
               id2 = ntk->createNet(); assert (V3NetUD != id2); inputs[0] = id1; inputs[1] = id;
               ntk->setInput(id2, inputs); ntk->createGate(conjType, id2); fairConstr[j] = id2;
            }
         }
         // Remove id From Candidate List
         if ((1 + i) != candidate.size()) candidate[i] = candidate.back(); candidate.pop_back();
      }
      newSize = candidate.size(); if (!V3SafeMult(newSize, (uint32_t)(1 + fairConstr.size()))) newSize = V3NtkUD;
      if (size <= newSize) break; size = newSize;
   }
   if (v3MCInteractive) {
      V3VrfShared::printLock();
      curTime = (clock() - curTime) / CLOCKS_PER_SEC;
      cerr << "TOTALLY " << fair << " FAIRNESS Extracted  (time = " << curTime << " sec)" << endl;
      V3VrfShared::printUnlock();
   }
}

#endif

