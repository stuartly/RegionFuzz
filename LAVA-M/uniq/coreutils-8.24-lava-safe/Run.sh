rm -rf BB*.txt
rm -rf config.cache
make clean
export TAFLPath="$PWD/../../../../TAFL"

#Orginal
export CC=${TAFLPath}/afl-clang-fast
export CXX=${TAFLPath}/afl-clang-fast++
export OrgCFLAGS=${CFLAGS}
export OrgCXXFLAGS=${CXXFLAGS}
./configure --prefix=$PWD/installed-Org
make -j5 
make install
make clean
rm -rf config.cache

#GET_MEM_DENSITY
export GET_MEM_DENSITY=1

export CC=${TAFLPath}/afl-clang-preprocess
export CXX=${TAFLPath}/afl-clang-preprocess++
export CFLAGS="${OrgCFLAGS} -outDir=$PWD"
export CXXFLAGS="${OrgCXXFLAGS} -outDir=$PWD"

./configure --prefix=$PWD/installed-MemDensity
make -j5
make install
make clean
rm -rf config.cache

export CC=${TAFLPath}/afl-clang-fast
export CXX=${TAFLPath}/afl-clang-fast++
export CFLAGS="${OrgCFLAGS} -MemDensityFile=$PWD/BBMemDensity.txt"
export CXXFLAGS="${OrgCFLAGS} -MemDensityFile=$PWD/BBMemDensity.txt"

./configure --prefix=$PWD/installed-MemDensity
make -j5
make install
make clean
rm -rf config.cache

unset GET_MEM_DENSITY


#GET_INST_NUM
export GET_INST_NUM=1

export CC=${TAFLPath}/afl-clang-preprocess
export CXX=${TAFLPath}/afl-clang-preprocess++
export CFLAGS="${OrgCFLAGS} -outDir=$PWD"
export CXXFLAGS="${OrgCXXFLAGS} -outDir=$PWD"

./configure --prefix=$PWD/installed-InstNum
make -j5
make install
make clean
rm -rf config.cache

export CC=${TAFLPath}/afl-clang-fast
export CXX=${TAFLPath}/afl-clang-fast++
export CFLAGS="${OrgCFLAGS} -InstNumFile=$PWD/BBInstNum.txt"
export CXXFLAGS="${OrgCFLAGS} -InstNumFile=$PWD/BBInstNum.txt"

./configure --prefix=$PWD/installed-InstNum
make -j5
make install
make clean
rm -rf config.cache
unset GET_INST_NUM


#GET_ENTRY_DEGREE
export GET_ENTRY_DEGREE=1

export CC=${TAFLPath}/afl-clang-preprocess
export CXX=${TAFLPath}/afl-clang-preprocess++
export CFLAGS="${OrgCFLAGS} -outDir=$PWD"
export CXXFLAGS="${OrgCXXFLAGS} -outDir=$PWD"

./configure --prefix=$PWD/installed-EntryDegree
make -j5
make install
make clean
rm -rf config.cache
export CC=${TAFLPath}/afl-clang-fast
export CXX=${TAFLPath}/afl-clang-fast++
export CFLAGS="${OrgCFLAGS} -EntryDegreeFile=$PWD/BBEntryDegree.txt"
export CXXFLAGS="${OrgCFLAGS} -EntryDegreeFile=$PWD/BBEntryDegree.txt"

./configure --prefix=$PWD/installed-EntryDegree
make -j5
make install
make clean
rm -rf config.cache
unset GET_ENTRY_DEGREE



#GET_DEPTH
export GET_DEPTH=1

export CC=${TAFLPath}/afl-clang-preprocess
export CXX=${TAFLPath}/afl-clang-preprocess++
export CFLAGS="${OrgCFLAGS} -outDir=$PWD"
export CXXFLAGS="${OrgCXXFLAGS} -outDir=$PWD"

./configure --prefix=$PWD/installed-Depth
make -j5
make install
make clean
rm -rf config.cache
export CC=${TAFLPath}/afl-clang-fast
export CXX=${TAFLPath}/afl-clang-fast++
export CFLAGS="${OrgCFLAGS} -DepthFile=$PWD/BBDepth.txt"
export CXXFLAGS="${OrgCFLAGS} -DepthFile=$PWD/BBDepth.txt"

./configure --prefix=$PWD/installed-Depth
make -j5
make install
make clean
rm -rf config.cache
unset GET_DEPTH

echo "running fuzzers...\n"

rm -rf out-AFL-Org out-AFL-Fast out-Fair-Fuzz out-TAFL-MemDensity out-TAFL-InstNum out-TAFL-Depth out-TAFL-EntryDegree

gnome-terminal -e "../../../afl-fuzz-org -i ../fuzz_seed -o out-AFL-Org -d ./installed-Org/bin/uniq -u @@"

gnome-terminal -e "../../../afl-fast-fuzz -i ../fuzz_seed -o out-AFL-Fast -d ./installed-Org/bin/uniq -u @@"

gnome-terminal -e "../../../fairfuzz -i ../fuzz_seed -o out-Fair-Fuzz -d ./installed-Org/bin/uniq -u @@"

gnome-terminal -e "../../../tafl-fuzz -i ../fuzz_seed -o out-TAFL-MemDensity -d -G BBMemDensity ./installed-MemDensity/bin/uniq -u @@"

gnome-terminal -e "../../../tafl-fuzz -i ../fuzz_seed -o out-TAFL-InstNum -d -G BBInstNum ./installed-InstNum/bin/uniq -u @@"

gnome-terminal -e "../../../tafl-fuzz -i ../fuzz_seed -o out-TAFL-Depth -d -G BBDepth ./installed-Depth/bin/uniq -u @@"

gnome-terminal -e "../../../tafl-fuzz -i ../fuzz_seed -o out-TAFL-EntryDegree -d -G BBEntryDegree ./installed-EntryDegree/bin/uniq -u @@"

