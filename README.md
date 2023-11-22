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
cd LAVA-M/base64/ccoreutils-8.24-lava-safe
bash Run-Depth.sh

```

# Reference
```
@article{situ2021vulnerable,
  title={Vulnerable Region-Aware Greybox Fuzzing},
  author={Situ, Ling-Yun and Zuo, Zhi-Qiang and Guan, Le and Wang, Lin-Zhang and Li, Xuan-Dong and Shi, Jin and Liu, Peng},
  journal={Journal of Computer Science and Technology},
  volume={36},
  number={5},
  pages={1212--1228},
  year={2021},
  publisher={Springer}
}

@inproceedings{situ2019energy,
  title={Energy distribution matters in greybox fuzzing},
  author={Situ, Lingyun and Wang, Linzhang and Li, Xuandong and Guan, Le and Zhang, Wenhui and Liu, Peng},
  booktitle={2019 IEEE/ACM 41st International Conference on Software Engineering: Companion Proceedings (ICSE-Companion)},
  pages={270--271},
  year={2019},
  organization={IEEE}
}


```
