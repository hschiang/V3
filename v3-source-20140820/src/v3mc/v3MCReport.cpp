/****************************************************************************
  FileName     [ v3MCReport.cpp ]
  PackageName  [ v3/src/v3mc ]
  Synopsis     [ V3 Model Checker Verification Result Report. ]
  Author       [ Cheng-Yin Wu ]
  Copyright    [ Copyright(c) 2014 DVLab, GIEE, NTU, Taiwan ]
****************************************************************************/

#ifndef V3_MC_REPORT_C
#define V3_MC_REPORT_C

#include "v3MCMain.h"

// V3 Verification Result Report Functions
void reportAigerCex(const uint32_t& inputSize, const uint32_t& latchSize, const V3CexTrace& cex, const string& prefix) {
   assert (cex.getTraceSize()); assert (prefix.size());
   // Store Output for Each Cycle into String
   V3BitVecX initValue(latchSize ? latchSize : 1);
   V3BitVecX patternValue(inputSize ? inputSize : 1);
   // Report Cex in AIGER 1.9 Format
   V3VrfShared::printLock();
   cout << "1" << endl;
   cout << prefix << endl;
   // Report Initial State
   if (latchSize) {
      if (cex.getInit()) initValue = cex.getInit()->bv_slice(latchSize - 1, 0);
      for (uint32_t k = 0; k < latchSize; ++k) cout << ('1' == initValue[k] ? '1' : '0');
   }
   cout << endl;
   // Report Pattern Values
   for (uint32_t i = 0; i < cex.getTraceSize(); ++i) {
      if (inputSize) {
         patternValue = cex.getData(i).bv_slice(inputSize - 1, 0);
         for (uint32_t k = 0; k < inputSize; ++k) cout << ('1' == patternValue[k] ? '1' : '0');
      }
      cout << endl;
   }
   cout << "." << endl;
   V3VrfShared::printUnlock();
}

void reportAigerInv(const uint32_t& inputSize, const uint32_t& latchSize, const string& prefix) {
   assert (prefix.size());
   // Report Inv in AIGER 1.9 Format
   V3VrfShared::printLock();
   cout << "0" << endl;
   cout << prefix << endl;
   cout << "." << endl;
   V3VrfShared::printUnlock();
}

#endif
