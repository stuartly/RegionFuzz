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

    class BlockDepth : public ModulePass {

    public:

        static char ID;

        BlockDepth() : ModulePass(ID) {}

        bool runOnModule(Module &M) override;

        std::string getBBName(BasicBlock &BB);
    };
}

char BlockDepth::ID = 0;


std::string BlockDepth::getBBName(BasicBlock &BB) {

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


bool isVistedEdge(BasicBlock *FromBB, BasicBlock *ToBB, std::multimap<BasicBlock *, BasicBlock *> &VisitedEdgeSet) {

    for (auto &ele:VisitedEdgeSet) {
        if (ele.first == FromBB && ele.second == ToBB) {
            return true;
        }
    }

    return false;

}


bool isInPath(BasicBlock *BB, std::vector<BasicBlock *> Path) {

    if (std::find(Path.begin(), Path.end(), BB) != Path.end()) {
        return true;
    } else {
        return false;
    }
}


void deleteOutEdge(BasicBlock *BB, std::multimap<BasicBlock *, BasicBlock *> &VisitedEdgeSet) {

    VisitedEdgeSet.erase(BB);
}

int getAnalyzedEdge(BasicBlock *BB, std::multimap<BasicBlock *, BasicBlock *> &VisitedEdgeSet) {
    int num=0;
    for (std::multimap<BasicBlock *, BasicBlock *>::iterator it = VisitedEdgeSet.begin();
         it != VisitedEdgeSet.end(); ++it) {
        if (it->first == BB) {
            num++;
        }
    }

    return num;
}



std::set <vector<BasicBlock *>> getAllPath(BasicBlock *EntryPoint, BasicBlock *EndPoint) {

    std::vector <BasicBlock *> path;
    std::set <vector<BasicBlock *>> pathList;
    std::multimap<BasicBlock *, BasicBlock *> VisitedEdgeSet;

    path.push_back(EntryPoint);
    BasicBlock* top = path.back();

    while (!path.empty()) {

        if(top==EndPoint){
            pathList.insert(path);
            path.pop_back();
            //deleteOutEdge(top,VisitedEdgeSet);
            top=path.back();
            continue;
        }

        const TerminatorInst *TInst = top->getTerminator();
        int SuccNum = TInst->getNumSuccessors();

        int i = 0;
        for (i= 0; i < SuccNum; ++i) {

            BasicBlock *succBB = TInst->getSuccessor(i);

            if (!isVistedEdge(top, succBB, VisitedEdgeSet)) {

                if (!isInPath(succBB, path)) {

                    path.push_back(succBB);
                    VisitedEdgeSet.insert(std::pair<BasicBlock*, BasicBlock*>(top,succBB));

                    top = succBB;

                    if (succBB == EndPoint) {
                        pathList.insert(path);
                        path.pop_back();
                        //deleteOutEdge(top,VisitedEdgeSet);
                        top=path.back();
                    }

                    break;

                }else{
                    continue;
                    //path.pop_back();
                    //top = path.back();
                }

            }
        }

        if(i == SuccNum){
            path.pop_back();
            //deleteOutEdge(top,VisitedEdgeSet);
            top = path.back();

        }
    }

    return pathList;

}




double calcuateDepth(std::set <vector<BasicBlock *>> PathList) {

    double depth = 0, length = 0;

    for (auto &ele: PathList) {
        double n = ele.size();
        //errs()<<n<<"\n";
        if (n > 0) {
            length += (1 / n);
        }
    }

    if (length > 0) {
        depth = 1.0 / length;
    }

    return depth;
}


bool BlockDepth::runOnModule(Module &M) {

    std::string outFile = "";
    std::map<std::string, double> BB_TO_Depth;

    for (auto &F: M) {

        for (auto &B:F) {

            BasicBlock *EntryBlock = &F.getEntryBlock();  //start point

            std::string bb_name = getBBName(B);

            BasicBlock *BB = &B; //end point

            set <vector<BasicBlock *>> PathList = getAllPath(EntryBlock, BB);

            double BBDepth = calcuateDepth(PathList);

            BB_TO_Depth.insert(pair<std::string, double>(bb_name, BBDepth));
        }
    }


    //output to file

    std::ofstream bb_mem_density;


    if (outDir.empty()) {
        FATAL("setting -outDir\n");
    } else {
        outFile = outDir + "/BBDepth.txt";
    }

    std::ofstream bb_to_depth;
    bb_to_depth.open(outFile, std::ofstream::out | std::ofstream::app);

    for (auto &ele: BB_TO_Depth) {
        bb_to_depth << ele.first << "," << ele.second << "\n";
    }

    bb_to_depth.close();

    return false;

}


static void registerAFLPass(const PassManagerBuilder &,
                            legacy::PassManagerBase &PM) {
    PM.add(new BlockDepth());
}


static RegisterStandardPasses RegisterAFLPass(
        PassManagerBuilder::EP_OptimizerLast, registerAFLPass);

static RegisterStandardPasses RegisterAFLPass0(
        PassManagerBuilder::EP_EnabledOnOptLevel0, registerAFLPass);
