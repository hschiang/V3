V3 Source Code Release
          Copyright(c) 2012-2014 DVLab, GIEE, NTU, Taiwan


VERSION: 
   v3-source-20140820 Released at August 28th, 2014.

AUTHOR: (e-mail: author.v3@gmail.com)
   Cheng-Yin Wu  (e-mail: gro070916@yahoo.com.tw; d99943034@ntu.edu.tw)
   Chung-Yang (Ric) Huang  (e-mail: ric@cc.ee.ntu.edu.tw)

   Affiliations: National Taiwan University, Taipei, Taiwan.

INTRODUCTION:
   V3 is a new and extensible framework for hardware verification and 
   debugging researches on both Boolean-level and word-level designs.
   It won two sliver and two bronze medals in the Hardware Model Checking
   Competition (HWMCC) in the past three years (2012-2014). 
   It is a powerful tool for users and an elaborate framework for 
   developers as well. 

   Due to publication issues, we reserve our implementation of several algorithms. 

DOWNLOAD:
   Please download the latest V3 here: 
      http://dvlab.ee.ntu.edu.tw/~publication/V3/download.html
   
   cd <directory-where-you-extracted-V3>
   make clean; make;
   ./v3 or ./bin/v3

   Please also find the tutorial and documentation for V3 programmers here: 
      http://dvlab.ee.ntu.edu.tw/~publication/V3/download.html

COMPILATION:

EXECUTION:

COMMANDS:
   ========== Common Commands : ==========
   DOfile:             Execute the commands in the dofile.
   HELp:               Print this help message.
   HIStory:            Print command history.
   Quit:               Quit the execution.
   SET LOgfile:        Redirect messages to files.
   USAGE:              Report resource usage.
   
   ========== I/O Commands : ==========
   REAd Aig:           Read AIGER Designs.
   REAd Btor:          Read BTOR Designs.
   REAd Rtl:           Read RTL (Verilog) Designs.
   WRIte Aig:          Write AIGER Designs.
   WRIte Btor:         Write BTOR Network.
   WRIte Rtl:          Write RTL (Verilog) Designs.
   
   ========== Print Commands : ==========
   PLOt NTk:           Plot Network Topology.
   PRInt NEt:          Print Net Information.
   PRInt NTk:          Print Network Information.
   
   ========== Synthesis Commands : ==========
   BLAst NTk:          Bit-blast Word-level Networks into Boolean-level Networks.
   DUPlicate NTk:      Duplicate Current Ntk from Verbosity Settings.
   EXPand NTk:         Perform Time-frame Expansion for Networks.
   FLAtten NTk:        Flatten Hierarchical Networks.
   MITer NTk:          Miter Two Networks.
   PRInt NTKVerbosity: Print Verbosities for Network Duplication.
   REDuce NTk:         Perform COI Reduction on Current Network.
   REWrite NTk:        Perform Rule-based Rewriting on Current Network.
   SET NTKVerbosity:   Set Verbosities for Network Duplication.
   STRash NTk:         Perform Structural Hashing on Current Network.
   
   ========== Manipulation Commands : ==========
   @CD:                Change Design for Current Network.
   @LN:                Link a Network with an instance of Current Network.
   @LS:                List Network Instances of Current Network.
   
   ========== Extraction Commands : ==========
   ELAborate FSM:      Elaborate Network and Construct FSM from Input Specification.
   EXTract FSM:        Extract Finite State Machines from Current Network.
   PLOT FSM:           Plot Finite State Machines into *.png files.
   WRIte FSM:          Output Finite State Machines Specifications.
   
   ========== Simulation Commands : ==========
   PLOt TRace:         Plot simulation or counterexample traces.
   SIM NTk:            Simulate on Current Network.
   
   ========== Verification Commands : ==========
   CHEck REsult:       Verify Verification Result.
   ELAborate PRoperty: Elaborate Properties on a Duplicated Network.
   PLOt REsult:        Plot Verification Results.
   PRInt REport:       Print Verbosities for Verification Report.
   PRInt SOlver:       Print Active Solver for Verification.
   SET PRoperty:       Set Properties on Current Network.
   SET REport:         Set Verbosities for Verification Report.
   SET SOlver:         Set Active Solver for Verification.
   VERify BMC:         Perform Bounded Model Checking.
   VERify ITP:         Perform Interpolation-based Model Checking.
   VERify KLIVE:       Perform K-Liveness for Liveness Checking.
   VERify PDR:         Perform Property Directed Reachability.
   VERify SEC:         Perform Sequential Equivalence Checking.
   VERify SIM:         Perform (Constrained) Random Simulation.
   VERify UMC:         Perform Unbounded Model Checking.
   WRIte REsult:       Write Verification Results.
   
   ========== Model Checking Commands : ==========
   READ PROperty:      Read property specification from external file.
   RUN:                Run V3 Model Checker.
   WRITE PROperty:     Write property specification into file.

LICENSE:
   Please refer to the COPYING file and individual files under src/eng. 
