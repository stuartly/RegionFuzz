# Install
```
1. sudo apt-get install bison flex libboost-dev
2. sudo apt-get intall clang llvm 
3. cd RegionFuzz  && make 
4. cd RegionFuzz/llvm_mode && make 
5. cd RegionFuzz/llvm_mode_preprocess && make
```
 

# Example
```
/*extract weight value*/
export GET_Depth=1
CC="Abosute/Path/To/afl-clang-preprocess -outDir=Abosute/Path/To/outDir ./configure --prefix=$PWD/installed --disable-shared
make -j4 && make install

/*Instrumentation */
CC="Abosute/Path/To/afl-clang-fast -DepthFile=Abosute/Path/To/BBDepth.txt" ./configure --prefix=$PWD/installed --disable-shared
make && make install

/*Guided fuzzing*/
./RegionFuzz/afl-fuzz -i in -o out -G BBDepth -d ./bin

```


# Demo

```
cd Benchmark/LAVA-M/base64
bash Run.sh

```

# Reference
```
Lingyun Situ, Zhiqiang Zuo, Le Guan, Linzhang Wang, Xuandong Li, Jin Shi, Peng Liu. Vulnerable Region-Aware Greybox Fuzzing [J]. Journal of Computer Science and Technology. 2021

```
