# Install
```
1. sudo apt-get install bison flex libboost-dev
2. sudo apt-get intall clang llvm 
   or 
   build clang llvm from source code(refer to https://github.com/stuartly/TargetFuzz/blob/master/TAFL/INSTALL_LLVM) (recommend)
3. cd TAFL  && make 
4. cd TAFL/llvm_mode && make 
5. cd TAFL/llvm_mode_preprocess && make
```
 

# Using TAFL-MemDensity
```
/*extract weight value*/
export GET_MEM_DENSITY=1
CC="Abosute/Path/To/afl-clang-preprocess -outDir=Abosute/Path/To/outDir ./configure --prefix=$PWD/installed --disable-shared
make -j4

/*Instrumentation */
CC="Abosute/Path/To/afl-clang-fast -MemDensityFile=Abosute/Path/To/BBMemDensity.txt" ./configure --prefix=$PWD/installed --disable-shared
make && make install

/*Guided fuzzing*/
./TAFL/afl-fuzz -i in -o out -G BBMemDensity -d  ./bin

```

# using TAFL-InstNum
```
/*extract weight value*/
export GET_INST_NUM=1
CC="Abosute/Path/To/afl-clang-preprocess -outDir=Abosute/Path/To/outDir ./configure --prefix=$PWD/installed --disable-shared
make -j4

/*Instrumentation */
CC="Abosute/Path/To/afl-clang-fast -InstNumFile=Abosute/Path/To/BBInstNum.txt" ./configure --prefix=$PWD/installed --disable-shared
make && make install

/*Guided fuzzing*/
./TAFL/afl-fuzz -i in -o out -G BBInstNum -d ./bin

```


# using TAFL-EntryDegree
```
/*extract weight value*/
export GET_ENTRY_DEGREE=1
CC="Abosute/Path/To/afl-clang-preprocess -outDir=Abosute/Path/To/outDir ./configure --prefix=$PWD/installed --disable-shared
make -j4

/*Instrumentation */
CC="Abosute/Path/To/afl-clang-fast -EntryDegreeFile=Abosute/Path/To/BBEntryDegree.txt" ./configure --prefix=$PWD/installed --disable-shared
make && make install

/*Guided fuzzing*/
./TAFL/afl-fuzz -i in -o out -G BBEntryDegree -d  ./bin

```


# using TAFL-Depth
```
/*extract weight value*/
export GET_Depth=1

mkdir TEMP

CC="Abosute/Path/To/afl-clang-preprocess -outDir=Abosute/Path/To/outDir ./configure --prefix=$PWD/installed --disable-shared
make -j4 && make install



/*Instrumentation */
CC="Abosute/Path/To/afl-clang-fast -DepthFile=Abosute/Path/To/BBDepth.txt" ./configure --prefix=$PWD/installed --disable-shared
make && make install

/*Guided fuzzing*/
./TAFL/afl-fuzz -i in -o out -G BBDepth -d ./bin

```


# using TAFL-AllFour
```
/*extract weight value*/
export GET_ALLFOUR=1

mkdir TEMP

CC="Abosute/Path/To/afl-clang-preprocess -outDir=Abosute/Path/To/outDir ./configure --prefix=$PWD/installed --disable-shared
make -j4 && make install



/*Instrumentation */
CC="Abosute/Path/To/afl-clang-fast -DepthFile=Abosute/Path/To/BBAllFour.txt" ./configure --prefix=$PWD/installed --disable-shared
make && make install

/*Guided fuzzing*/
./TAFL/afl-fuzz -i in -o out -G BBAllFour -d ./bin

```


# Using TAFL-NewBranchNum
```
CC=Abosute/Path/To/afl-clang-fast ./configure --prefix=$PWD/installed --disable-shared
make && make install

/*Guided fuzzing*/
./TAFL/afl-fuzz -i in -o out -G NewBranchNum -d  ./bin

```


# Demo on LAVA-M Benchmark

```
cd Benchmark/LAVA-M/base64
bash Run.sh

```

