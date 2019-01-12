/*
   american fuzzy lop - LLVM-mode instrumentation pass
   ---------------------------------------------------

   Written by Laszlo Szekeres <lszekeres@google.com> and
              Michal Zalewski <lcamtuf@google.com>

   LLVM integration design comes from Laszlo Szekeres. C bits copied-and-pasted
   from afl-as.c are Michal's fault.

   Copyright 2015, 2016 Google Inc. All rights reserved.

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at:

     http://www.apache.org/licenses/LICENSE-2.0

   This library is plugged into LLVM when invoking clang through afl-clang-fast.
   It tells the compiler to add code roughly equivalent to the bits discussed
   in ../afl-as.h.

 */

#define AFL_LLVM_PASS

#include "../config.h"
#include "../debug.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/********Begin aflGo************/
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <list>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>
/********End aflGo*************/

#include "llvm/ADT/Statistic.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"

/********Begin aflGo************/
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/CFGPrinter.h"

#if defined(LLVM34)
#include "llvm/DebugInfo.h"
#else

#include "llvm/IR/DebugInfo.h"

#endif

#if defined(LLVM34) || defined(LLVM35) || defined(LLVM36) 
#define LLVM_OLD_DEBUG_API
#endif
/********End aflGo*************/


using namespace llvm;



static cl::opt <std::string> MemDensityFile(
        "MemDensityFile",
        cl::desc("Memory density file containing the memory density of each basic block."),
        //cl::Optional,
        cl::value_desc("MemDensityFile"),
        cl::init("")
);


static cl::opt <std::string> InstNumFile(
        "InstNumFile",
        cl::desc("Inst number file containing the instrument numbers density of each basic block."),
        //cl::Optional,
        cl::value_desc("InstNumFile"),
        cl::init("")
);

static cl::opt <std::string> EntryDegreeFile(
        "EntryDegreeFile",
        cl::desc("EntryDegreeFile file."),
        //cl::Optional,
        cl::value_desc("EntryDegreeFile"),
        cl::init("")
);


static cl::opt <std::string> DepthFile(
        "DepthFile",
        cl::desc("DepthFile containing the distance of each basic block to the provided targets."),
        cl::Optional,
        cl::value_desc("DepthFile"),
        cl::init("")
);

static cl::opt <std::string> AllFourFile(
        "AllFourFileFile",
        cl::desc("AllFourFile containing the sum of the four passes."),
        cl::Optional,
        cl::value_desc("AllFourFile"),
        cl::init("")
);


namespace {

    class AFLCoverage : public ModulePass {

    public:

        static char ID;

        /****global variable**********/
        std::map<std::string, double> bb_to_depth;
        std::map<std::string, double> bb_to_memDensity;   //MemDensity
        std::map<std::string, int> bb_to_instNum;         //InstNum
        std::map<std::string, double> bb_to_entryDegree;  //EnterDegree
        std::map<std::string, double> bb_to_allFour;      //AllFour

        std::vector <std::string> basic_blocks;

        AFLCoverage() : ModulePass(ID) {}

        std::string getBBName(BasicBlock &BB);

        double getDepth(std::string bb_name);

        int getInstNum(std::string bb_name);

        int getBBNum(std::string bb_name);

        double getEntryDegree(std::string bb_name);

        double getMemDensity(std::string bb_name);

        double getAllFour(std::string bb_name);

        bool runOnModule(Module &M) override;

    };

}

char AFLCoverage::ID = 0;


std::string AFLCoverage::getBBName(BasicBlock &BB) {

    /* get the namae of the basic block */
    std::string bb_name("");
    std::string filename("");
    unsigned line=0;

    for (auto &I : BB) {

        if (DILocation * Loc = I.getDebugLoc()) {
            line = Loc->getLine();
            filename = Loc->getFilename().str();

            if (filename.empty()) {
                DILocation *oDILoc = Loc->getInlinedAt();
                if (oDILoc) {
                    line = oDILoc->getLine();
                    filename = oDILoc->getFilename().str();
                }
            }

            /* Don't worry about external libs */
            std::string Xlibs("/usr/");
            if (filename.empty() || line == 0 || !filename.compare(0, Xlibs.size(), Xlibs))
                continue;

            if (bb_name.empty()) {

                std::size_t found = filename.find_last_of("/\\");
                if (found != std::string::npos)
                    filename = filename.substr(found + 1);
                bb_name = filename + ":" + std::to_string(line);
            }

            return bb_name;
        }

    }

}

double AFLCoverage::getDepth(std::string bb_name) {

    double depth = 0;
    if (!bb_name.empty()) {

        /* Find distance for bb_name BB */
        std::map<std::string, double>::iterator it;
        for (it = bb_to_depth.begin(); it != bb_to_depth.end(); ++it) {
            if (it->first.compare(bb_name) == 0) {
                depth = it->second;
                break;
            }
        }

    } else {

        FATAL("bb_name is empty !\n");
    }

    return depth;
}

int AFLCoverage::getInstNum(std::string bb_name) {

    int inst_num = 0;
    if (!bb_name.empty()) {

        /* Find distance for bb_name BB */
        std::map<std::string, int>::iterator it;
        for (it = bb_to_instNum.begin(); it != bb_to_instNum.end(); ++it) {
            if (it->first.compare(bb_name) == 0) {
                inst_num = it->second;
                break;
            }
        }

    } else {

        FATAL("bb_name is empty !\n");
    }

    return inst_num;
}


double AFLCoverage::getMemDensity(std::string bb_name) {

    double memDensity = 0;

    if (!bb_name.empty()) {

        /* Find memDensity for bb_name BB */
        std::map<std::string, double>::iterator it;
        for (it = bb_to_memDensity.begin(); it != bb_to_memDensity.end(); ++it) {
            if (it->first.compare(bb_name) == 0) {
                memDensity = it->second;
                break;
            }
        }

    } else {

        FATAL("bb_name is empty !\n");
    }

    return memDensity;
}


double AFLCoverage::getEntryDegree(std::string bb_name) {

    double EntryDegree = 0;

    if (!bb_name.empty()) {

        /* Find memDensity for bb_name BB */
        std::map<std::string, double>::iterator it;
        for (it = bb_to_entryDegree.begin(); it != bb_to_entryDegree.end(); ++it) {
            if (it->first.compare(bb_name) == 0) {
                EntryDegree = it->second;
                break;
            }
        }

    } else {

        FATAL("bb_name is empty !\n");
    }

    return EntryDegree;
}

double AFLCoverage::getAllFour(std::string bb_name) {

    double allFour = 0;
    if (!bb_name.empty()) {

        /* Find distance for bb_name BB */
        std::map<std::string, double>::iterator it;
        for (it = bb_to_allFour.begin(); it != bb_to_allFour.end(); ++it) {
            if (it->first.compare(bb_name) == 0) {
                allFour = it->second;
                break;
            }
        }

    } else {

        FATAL("bb_name is empty !\n");
    }

    return allFour;
}


bool AFLCoverage::runOnModule(Module &M) {

    bool is_MemDensity  = false;         //MemDensity
    bool is_InstNumber  = false;         //InstNum
    bool is_EntryDegree = false;         //EnterDegree
    bool is_Depth       = false;         //Depth
    bool is_AllFour     = false;         //AllFour
   //TODO: is_Distance and is_MemDensity can not be true at same time

    if (!DepthFile.empty()) {

        /* put DistanceFile into bb_to_dis map */

        std::ifstream cf(DepthFile.c_str());
        if (cf.is_open()) {

            std::string line;
            while (getline(cf, line)) {

                std::size_t pos = line.find(",");
                std::string bb_name = line.substr(0, pos);
                double bb_depth = (double) (std::stod(line.substr(pos + 1, line.length()).c_str()));

                bb_to_depth.insert(std::pair<std::string, double>(bb_name, bb_depth));
                basic_blocks.push_back(bb_name);

            }
            cf.close();

            is_Depth = true;

        } else {
            FATAL("Unable to find %s.", DepthFile.c_str());
            return false;
        }

    }

        /*put memory operation density to each Basic Block*/
    else if (!MemDensityFile.empty()) {

        std::ifstream cf(MemDensityFile.c_str());
        if (cf.is_open()) {

            std::string line;
            while (getline(cf, line)) {

                std::size_t pos = line.find(",");
                std::string bb_name = line.substr(0, pos);
                double bb_memDensity = (double) (std::stod(line.substr(pos + 1, line.length()).c_str()));

                bb_to_memDensity.insert(std::pair<std::string, double>(bb_name, bb_memDensity));
                basic_blocks.push_back(bb_name);

            }
            cf.close();

            is_MemDensity = true;

        } else {
            FATAL("Unable to find %s.", MemDensityFile.c_str());
            return false;
        }

    }

        /*put inst number to each Basic Block*/
    else if (!InstNumFile.empty()) {

        /* put InstNum of each BB into bb_to_instNum map */

        std::ifstream cf(InstNumFile.c_str());
        if (cf.is_open()) {

            std::string line;
            while (getline(cf, line)) {

                std::size_t pos = line.find(",");
                std::string bb_name = line.substr(0, pos);
                int bb_inst_num = (int) (atof(line.substr(pos + 1, line.length()).c_str()));

                bb_to_instNum.insert(std::pair<std::string, int>(bb_name, bb_inst_num));
                basic_blocks.push_back(bb_name);

            }

            cf.close();

            is_InstNumber = true;

        } else {

            FATAL("Unable to find %s.", InstNumFile.c_str());
            return false;
        }
    }

    else if (!EntryDegreeFile.empty()) {

        /* put InstNum of each BB into bb_to_instNum map */

        std::ifstream cf(EntryDegreeFile.c_str());
        if (cf.is_open()) {

            std::string line;
            while (getline(cf, line)) {

                std::size_t pos = line.find(",");
                std::string bb_name = line.substr(0, pos);
                double bb_entry_degree = (double) (std::stod(line.substr(pos + 1, line.length()).c_str()));

                bb_to_entryDegree.insert(std::pair<std::string, double>(bb_name, bb_entry_degree));
                basic_blocks.push_back(bb_name);

            }

            cf.close();

            is_EntryDegree = true;

        } else {

            FATAL("Unable to find %s.", EntryDegreeFile.c_str());
            return false;
        }

    }

    else if (!AllFourFile.empty()) {

        /* put DistanceFile into bb_to_dis map */

        std::ifstream cf(AllFourFile.c_str());
        if (cf.is_open()) {

            std::string line;
            while (getline(cf, line)) {

                std::size_t pos = line.find(",");
                std::string bb_name = line.substr(0, pos);
                double bb_to_all_four = (double) (std::stod(line.substr(pos + 1, line.length()).c_str()));

                bb_to_allFour.insert(std::pair<std::string, double>(bb_name, bb_to_all_four));
                basic_blocks.push_back(bb_name);

            }
            cf.close();

            is_AllFour = true;

        } else {
            FATAL("Unable to find %s.", AllFourFile.c_str());
            return false;
        }

    }



    LLVMContext &C = M.getContext();
    IntegerType *Int8Ty = IntegerType::getInt8Ty(C);
    IntegerType *Int32Ty = IntegerType::getInt32Ty(C);
    IntegerType *Int64Ty = IntegerType::getInt64Ty(C);
    Type *DoubleTy = Type::getDoubleTy(C);


    /* Show a banner */

    char be_quiet = 0;

    if (isatty(2) && !getenv("AFL_QUIET")) {

        if (is_Depth)
            SAYF(cCYA
                         "afl-llvm-pass (BBDepth) "
                         cBRI
                                 VERSION
                         cRST
                         " by <situlingyun@gmail.com>\n");
        else if (is_MemDensity)
            SAYF(cCYA
                         "afl-llvm-pass (MemDensity) "
                         cBRI
                                 VERSION
                         cRST
                         " by <situlingyun@gmail.com>\n");

        else if (is_InstNumber)
            SAYF(cCYA
                         "afl-llvm-pass (InstNumber) "
                         cBRI
                                 VERSION
                         cRST
                         " by <situlingyun@gmail.com>\n");
        else if (is_EntryDegree)
            SAYF(cCYA
                         "afl-llvm-pass (BBEntryDegree) "
                         cBRI
                                 VERSION
                         cRST
                         " by <situlingyun@gmail.com>\n");
        else if (is_AllFour)
            SAYF(cCYA
                         "afl-llvm-pass (AllFour) "
                         cBRI
                                 VERSION
                         cRST
                         " by <situlingyun@gmail.com>\n");
        else
            SAYF(cCYA
                         "afl-llvm-pass "
                         cBRI
                                 VERSION
                         cRST
                         " by <lszekeres@google.com>\n");


    } else be_quiet = 1;

    /* Decide instrumentation ratio */

    char *inst_ratio_str = getenv("AFL_INST_RATIO");
    unsigned int inst_ratio = 100;

    if (inst_ratio_str) {

        if (sscanf(inst_ratio_str, "%u", &inst_ratio) != 1 || !inst_ratio ||
            inst_ratio > 100)
            FATAL("Bad value of AFL_INST_RATIO (must be between 1 and 100)");

    }



    /* Get globals for the SHM region and the previous location. Note that
       __afl_prev_loc is thread-local. */

    /* AFLMapPtr pointing the start address of the SHM */
    GlobalVariable *AFLMapPtr =
            new GlobalVariable(M, PointerType::get(Int8Ty, 0), false,
                               GlobalValue::ExternalLinkage, 0, "__afl_area_ptr");

    /* AFLPrevLoc is what __afl_prev_loc is thread-local? */
    GlobalVariable *AFLPrevLoc = new GlobalVariable(
            M, Int32Ty, false, GlobalValue::ExternalLinkage, 0, "__afl_prev_loc",
            0, GlobalVariable::GeneralDynamicTLSModel, 0, false);

    /* Instrument all the things! */

    int inst_blocks = 0;

    for (auto &F : M) {

        for (auto &BB : F) {

            /********Begin Instrmentation of bb_to_bb transition********/
            BasicBlock::iterator IP = BB.getFirstInsertionPt();
            IRBuilder<> IRB(&(*IP));

            if (AFL_R(100) >= inst_ratio) continue;

            /* Make up cur_loc */

            unsigned int cur_loc = AFL_R(MAP_SIZE);             /* get a random number as cur_loc (cur_block) */

            ConstantInt *CurLoc = ConstantInt::get(Int32Ty, cur_loc);

            /* Load prev_loc */

            LoadInst *PrevLoc = IRB.CreateLoad(AFLPrevLoc);
            PrevLoc->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));
            Value *PrevLocCasted = IRB.CreateZExt(PrevLoc, IRB.getInt32Ty());

            /* Load SHM pointer */

            LoadInst *MapPtr = IRB.CreateLoad(AFLMapPtr);
            MapPtr->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));
            Value *MapPtrIdx =
                    IRB.CreateGEP(MapPtr, IRB.CreateXor(PrevLocCasted,
                                                        CurLoc)); /* Xor pre_loc and cur_loc as a transition */

            /* Update bitmap */

            /* if the transition_id is in the map, then load it's count, and ++1 */
            LoadInst *Counter = IRB.CreateLoad(MapPtrIdx);
            Counter->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));
            Value *Incr = IRB.CreateAdd(Counter, ConstantInt::get(Int8Ty, 1));
            IRB.CreateStore(Incr, MapPtrIdx)
                    ->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));

            /* Set prev_loc to cur_loc >> 1 */

            StoreInst *Store =
                    IRB.CreateStore(ConstantInt::get(Int32Ty, cur_loc >> 1), AFLPrevLoc);
            Store->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));

            /************************Begin Instrmentation of bb_to_bb transition***********************/


            /************************Begin Instrumentation of additional Int value *********************/

            if (is_InstNumber) {

                std::string bb_name = getBBName(BB);

                if (bb_name.empty()) continue;

                if (find(basic_blocks.begin(), basic_blocks.end(), bb_name) == basic_blocks.end()) {
                    ACTF("Not found BB_name: %s\n", bb_name.c_str());
                    continue;
                }

                int insertIntValue = 0;

                if(is_InstNumber){
                    insertIntValue = getInstNum(bb_name);         /*InstNum*/
                }

                if (insertIntValue >= 0) {
                    /*Instrumentation of distance info*/
                    unsigned int uInsertIntValue = (unsigned) insertIntValue;

#ifdef __x86_64__
                    IntegerType *LargestType = Int64Ty;
                    ConstantInt *MapValueLoc = ConstantInt::get(LargestType, MAP_SIZE);
                    ConstantInt *MapCntLoc = ConstantInt::get(LargestType, MAP_SIZE + 8);
                    ConstantInt *InsertIntVa = ConstantInt::get(LargestType, uInsertIntValue);
#else
                    IntegerType *LargestType = Int32Ty;
                    ConstantInt *MapValueLoc = ConstantInt::get(LargestType, MAP_SIZE);
                    ConstantInt *MapCntLoc = ConstantInt::get(LargestType, MAP_SIZE + 4);
                    ConstantInt *InsertIntVa = ConstantInt::get(LargestType, uInsertIntValue);

#endif

                    /* Add distance to shm[MAPSIZE] */

                    Value *MapValuePtr = IRB.CreateGEP(MapPtr, MapValueLoc);
                    MapValuePtr = IRB.CreatePointerCast(MapValuePtr, LargestType->getPointerTo());

#ifdef LLVM_OLD_DEBUG_API
                    LoadInst *MapValue = IRB.CreateLoad(MapValuePtr);
                    MapValue->mutateType(LargestType);
#else
                    LoadInst *MapValue = IRB.CreateLoad(LargestType, MapValuePtr);
#endif
                    MapValue->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));
                    Value *IncrValue = IRB.CreateAdd(MapValue, InsertIntVa);
                    IRB.CreateStore(IncrValue, MapValuePtr)
                            ->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));

                    /* Increase count at to shm[MAPSIZE + (4 or 8)] */

                    Value *MapCntPtr = IRB.CreateGEP(MapPtr, MapCntLoc);
                    MapCntPtr = IRB.CreatePointerCast(MapCntPtr, LargestType->getPointerTo());

#ifdef LLVM_OLD_DEBUG_API
                    LoadInst *MapCnt = IRB.CreateLoad(MapCntPtr);
                    MapCnt->mutateType(LargestType);
#else
                    LoadInst *MapCnt = IRB.CreateLoad(LargestType, MapCntPtr);
#endif
                    MapCnt->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));
                    Value *IncrCnt = IRB.CreateAdd(MapCnt, ConstantInt::get(LargestType, 1));
                    IRB.CreateStore(IncrCnt, MapCntPtr)
                            ->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));
                }

            }

            /************************End Instrumentation of additional Int value *********************/



            /************************Begin Instrumentation of additional Double value *****************/
            if (is_MemDensity  || is_Depth  || is_EntryDegree || is_AllFour) {

                std::string bb_name = getBBName(BB);

                if (bb_name.empty()) continue;

                if (find(basic_blocks.begin(), basic_blocks.end(), bb_name) == basic_blocks.end()) {
                    ACTF("Not found BB_name: %s\n", bb_name.c_str());
                    continue;
                }

                double memDensity = 0;

                if(is_EntryDegree){
                    memDensity = getEntryDegree(bb_name);     /*EnterDegree*/
                }else if(is_Depth){
                    memDensity = getDepth(bb_name);
                }else if(is_MemDensity) {
                    memDensity = getMemDensity(bb_name);
                }else if(is_AllFour) {
                    memDensity = getAllFour(bb_name);
                }

                if (memDensity >= 0) {

                    double umemDensity = memDensity;

#ifdef __x86_64__
                    Type *DoubleType = DoubleTy;
                    IntegerType *LargestType = Int64Ty;
                    ConstantInt *MapMemDenLoc = ConstantInt::get(LargestType, MAP_SIZE);
                    ConstantInt *MapCntLoc = ConstantInt::get(LargestType, MAP_SIZE + 8);
                    ConstantFP  *MemDensity = ConstantFP::get(M.getContext(), APFloat(umemDensity));
#else
                    Type *DoubleType = DoubleTy;
                    IntegerType *LargestType = Int32Ty;
                    ConstantInt *MapMemDenLoc = ConstantInt::get(LargestType, MAP_SIZE);
                    ConstantInt *MapCntLoc = ConstantInt::get(LargestType, MAP_SIZE + 4);
                    ConstantFP *MemDensity = ConstantFP::get(M.getContext(), APFloat(umemDensity));

#endif


                    /* Add distance to shm[MAPSIZE] */

                    Value *MapMemDenPtr = IRB.CreateGEP(MapPtr, MapMemDenLoc);
                    MapMemDenPtr = IRB.CreatePointerCast(MapMemDenPtr, DoubleType->getPointerTo());

#ifdef LLVM_OLD_DEBUG_API
                    LoadInst *MapMemDen = IRB.CreateLoad(MapMemDenPtr);
                    MapMemDen->mutateType(DoubleType);
#else
                    LoadInst *MapMemDen = IRB.CreateLoad(DoubleType, MapMemDenPtr);

#endif
                    MapMemDen->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));
                    Value *IncrMemDen = IRB.CreateFAdd(MapMemDen, MemDensity);
                    IRB.CreateStore(IncrMemDen, MapMemDenPtr)
                            ->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));

                    /* Increase count at to shm[MAPSIZE + (4 or 8)] */

                    Value *MapCntPtr = IRB.CreateGEP(MapPtr, MapCntLoc);
                    MapCntPtr = IRB.CreatePointerCast(MapCntPtr, LargestType->getPointerTo());

#ifdef LLVM_OLD_DEBUG_API
                    LoadInst *MapCnt = IRB.CreateLoad(MapCntPtr);
                    MapCnt->mutateType(LargestType);
#else
                    LoadInst *MapCnt = IRB.CreateLoad(LargestType, MapCntPtr);
#endif
                    MapCnt->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));
                    Value *IncrCnt = IRB.CreateAdd(MapCnt, ConstantInt::get(LargestType, 1));
                    IRB.CreateStore(IncrCnt, MapCntPtr)
                            ->setMetadata(M.getMDKindID("nosanitize"), MDNode::get(C, None));
                }
            }

            /************************End Instrumentation of additional Double value *********************/
            inst_blocks++;
        }
    }


    /* Say something nice. */

    if (!be_quiet) {

        if (!inst_blocks) {
            WARNF("No instrumentation targets found.");
        } else {

            OKF("Instrumented %u locations (%s mode, ratio %u%%).",
                inst_blocks, getenv("AFL_HARDEN") ? "hardened" :
                             ((getenv("AFL_USE_ASAN") || getenv("AFL_USE_MSAN")) ?
                              "ASAN/MSAN" : "non-hardened"), inst_ratio);

        }
    }

    return true;

}


static void registerAFLPass(const PassManagerBuilder &,
                            legacy::PassManagerBase &PM) {

    PM.add(new AFLCoverage());

}


static RegisterStandardPasses RegisterAFLPass(
        PassManagerBuilder::EP_OptimizerLast, registerAFLPass);

static RegisterStandardPasses RegisterAFLPass0(
        PassManagerBuilder::EP_EnabledOnOptLevel0, registerAFLPass);
