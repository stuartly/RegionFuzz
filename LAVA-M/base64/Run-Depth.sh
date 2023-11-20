rm -rf BB*.txt
rm -rf config.cache
make clean
export TAFLPath="/home/stly/Documents/Demo/RegionFuzz/"



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



${TAFLPath}/afl-fuzz -i inputs -o out-Depth -d -G BBDepth ./installed-Depth/bin/base64 -d @@


