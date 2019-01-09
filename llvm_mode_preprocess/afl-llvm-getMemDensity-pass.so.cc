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


#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <list>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <math.h>


#include "llvm/ADT/Statistic.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/Debug.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"


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


#include <fstream>
#include "llvm/Pass.h"
#include "llvm/IR/DebugInfo.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstIterator.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/PostDominators.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Analysis/LoopInfo.h"
#include "llvm/Analysis/PostDominators.h"
#include <boost/algorithm/string.hpp>
#include "llvm/IR/MDBuilder.h"
#include "llvm/IR/CallSite.h"
#include "llvm/IR/IntrinsicInst.h"


using namespace llvm;
using namespace std;


static cl::opt <std::string> outDir(
        "outDir",
        cl::desc("The name of File to store results"),
        cl::value_desc("outDir"),
        cl::init("")
);


namespace {

    class BlockMemDensity : public ModulePass {

    public:

        static char ID;

        BlockMemDensity() : ModulePass(ID) {}

        bool runOnModule(Module &M) override;

        std::string getBBName(BasicBlock &BB);

    };

}

char BlockMemDensity::ID = 0;

std::string BlockMemDensity::getBBName(BasicBlock &BB) {

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


bool BlockMemDensity::runOnModule(Module &M) {

    std::string outFile = "";

    std::map<std::string, double> BB_TO_MemDensity;

    /* Black list of function names */
    std::vector <std::string> blacklist = {
            "asan.",
            "llvm.",
            "sancov."
    };


    /*traverse each function and each block*/
    for (auto &F:M) {

        for (auto &BB:F) {

            int SensitiveInstNum    = 0;
            int TotalInstNum  = 0;
            double MemDensity = 0;
            std::string bb_name = getBBName(BB);

            for (auto &Inst:BB) {

                TotalInstNum++;

                if (LoadInst * load = dyn_cast<LoadInst>(&Inst)) {
                    SensitiveInstNum++;
                } else if (AllocaInst * alloc = dyn_cast<AllocaInst>(&Inst)) {
                    SensitiveInstNum++;
                } else if (StoreInst * store = dyn_cast<StoreInst>(&Inst)) {
                    SensitiveInstNum++;
                } else if (FenceInst * fence = dyn_cast<FenceInst>(&Inst)) {
                    SensitiveInstNum++;
                } else if (MemCpyInst * memcpy = dyn_cast<MemCpyInst>(&Inst)) {
                    SensitiveInstNum++;
                } else if (MemMoveInst * memmov = dyn_cast<MemMoveInst>(&Inst)) {
                    SensitiveInstNum++;
                } else if (MemSetInst * memset = dyn_cast<MemSetInst>(&Inst)) {
                    SensitiveInstNum++;
                } else if (MemTransferInst * memtrans = dyn_cast<MemTransferInst>(&Inst)) {
                    SensitiveInstNum++;
                } else if (MemIntrinsic * memIntr = dyn_cast<MemIntrinsic>(&Inst)) {
                    SensitiveInstNum++;
                }else if (CallInst * call = dyn_cast<CallInst>(&Inst)) {
                    if (call->getCalledFunction()) {

                        std::string called = call->getCalledFunction()->getName().str();

                        for (std::vector<std::string>::size_type i = 0; i < blacklist.size(); i++) {
                            if (called.compare(0, blacklist[i].size(), blacklist[i]) == 0)
                                continue;
                        }

                        /*collect sensitive call*/
                        if (called == "strcpy" ||
                            called == "strncpy" ||
                            called == "memcpy" ||
                            called == "memset" ||
                            called == "strcat" ||
                            called == "vsnprintf" ||
                            called == "realpath" ||
                            called == "sprintf" ||
                            called == "strcmp" ||
                            called == "compare" ||
                            called == "strcmpi" ||
                            called == "strncat" ||
                            called == "strchr" ||
                            called == "snprintf" ||
                            called == "vfprintf" ||
                            called == "vsprintf" ||
                            called == "vsnprintf") {
                            SensitiveInstNum++;
                        }
                    }
                }
            }


            if (TotalInstNum != 0) {
                MemDensity = (double) SensitiveInstNum;
            }

            BB_TO_MemDensity.insert(pair<std::string, double>(bb_name, MemDensity));
        }
    }




    /*output bb_mem_density*/
    std::ofstream bb_mem_density;

    if (outDir.empty()) {
        FATAL("setting -outDir\n");
    } else {
        outFile = outDir + "/BBMemDensity.txt";
    }

    bb_mem_density.open(outFile, std::ofstream::out | std::ofstream::app);

    for (auto &ele: BB_TO_MemDensity) {
        bb_mem_density << ele.first << "," << ele.second << "\n";
    }

    bb_mem_density.close();


    return false;

}


static void registerAFLPass(const PassManagerBuilder &,
                            legacy::PassManagerBase &PM) {

    PM.add(new BlockMemDensity());

}


static RegisterStandardPasses RegisterAFLPass(
        PassManagerBuilder::EP_OptimizerLast, registerAFLPass);

static RegisterStandardPasses RegisterAFLPass0(
        PassManagerBuilder::EP_EnabledOnOptLevel0, registerAFLPass);
