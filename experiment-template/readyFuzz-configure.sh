# autogen.sh # not run in *.sh script, please run in terminal firstly.

rm -rf BB*.txt
export NjorthFuzzPath="$PWD/../../NjorthFuzz"


# Orginal
export CC=${NjorthFuzzPath}/afl-clang-fast
export CXX=${NjorthFuzzPath}/afl-clang-fast++
export OrgCFLAGS=${CFLAGS}
export OrgCXXFLAGS=${CXXFLAGS}
./configure --prefix=$PWD/installed-Org
make -j6 
make install
make clean


# GET_MEM_DENSITY
export GET_MEM_DENSITY=1

export CC=${NjorthFuzzPath}/afl-clang-preprocess
export CXX=${NjorthFuzzPath}/afl-clang-preprocess++
export CFLAGS="${OrgCFLAGS} -outDir=$PWD"
export CXXFLAGS="${OrgCFLAGS} -outDir=$PWD"

./configure --prefix=$PWD/installed-MemDensity
make -j6
make install
make clean

export CC=${NjorthFuzzPath}/afl-clang-fast
export CXX=${NjorthFuzzPath}/afl-clang-fast++
export CFLAGS="${OrgCFLAGS} -MemDensityFile=$PWD/BBMemDensity.txt"
export CXXFLAGS="${OrgCFLAGS} -MemDensityFile=$PWD/BBMemDensity.txt"

./configure --prefix=$PWD/installed-MemDensity
make -j6
make install
make clean

unset GET_MEM_DENSITY


# GET_INST_NUM
export GET_INST_NUM=1

export CC=${NjorthFuzzPath}/afl-clang-preprocess
export CXX=${NjorthFuzzPath}/afl-clang-preprocess++
export CFLAGS="${OrgCFLAGS} -outDir=$PWD"
export CXXFLAGS="${OrgCFLAGS} -outDir=$PWD"

./configure --prefix=$PWD/installed-InstNum
make -j6
make install
make clean

export CC=${NjorthFuzzPath}/afl-clang-fast
export CXX=${NjorthFuzzPath}/afl-clang-fast++
export CFLAGS="${OrgCFLAGS} -InstNumFile=$PWD/BBInstNum.txt"
export CXXFLAGS="${OrgCFLAGS} -InstNumFile=$PWD/BBInstNum.txt"

./configure --prefix=$PWD/installed-InstNum
make -j6
make install
make clean

unset GET_INST_NUM


# GET_ENTRY_DEGREE
export GET_ENTRY_DEGREE=1

export CC=${NjorthFuzzPath}/afl-clang-preprocess
export CXX=${NjorthFuzzPath}/afl-clang-preprocess++
export CFLAGS="${OrgCFLAGS} -outDir=$PWD"
export CXXFLAGS="${OrgCFLAGS} -outDir=$PWD"

./configure --prefix=$PWD/installed-EntryDegree
make -j6
make install
make clean

export CC=${NjorthFuzzPath}/afl-clang-fast
export CXX=${NjorthFuzzPath}/afl-clang-fast++
export CFLAGS="${OrgCFLAGS} -EntryDegreeFile=$PWD/BBEntryDegree.txt"
export CXXFLAGS="${OrgCFLAGS} -EntryDegreeFile=$PWD/BBEntryDegree.txt"

./configure --prefix=$PWD/installed-EntryDegree
make -j6
make install
make clean

unset GET_ENTRY_DEGREE



# GET_DEPTH
export GET_DEPTH=1

export CC=${NjorthFuzzPath}/afl-clang-preprocess
export CXX=${NjorthFuzzPath}/afl-clang-preprocess++
export CFLAGS="${OrgCFLAGS} -outDir=$PWD"
export CXXFLAGS="${OrgCXXFLAGS} -outDir=$PWD"

./configure --prefix=$PWD/installed-Depth
make -j6
make install
make clean

export CC=${NjorthFuzzPath}/afl-clang-fast
export CXX=${NjorthFuzzPath}/afl-clang-fast++
export CFLAGS="${OrgCFLAGS} -DepthFile=$PWD/BBDepth.txt"
export CXXFLAGS="${OrgCFLAGS} -DepthFile=$PWD/BBDepth.txt"

./configure --prefix=$PWD/installed-Depth
make -j6
make install
make clean

unset GET_DEPTH



# GET_ALLFOUR
export GET_ALLFOUR=1

export CC=${NjorthFuzzPath}/afl-clang-preprocess
export CXX=${NjorthFuzzPath}/afl-clang-preprocess++
export CFLAGS="${OrgCFLAGS} -outDir=$PWD"
export CXXFLAGS="${OrgCXXFLAGS} -outDir=$PWD"

./configure --prefix=$PWD/installed-AllFour
make -j6
make install
make clean

export CC=${NjorthFuzzPath}/afl-clang-fast
export CXX=${NjorthFuzzPath}/afl-clang-fast++
export CFLAGS="${OrgCFLAGS} -AllFourFile=$PWD/BBAllFour.txt"
export CXXFLAGS="${OrgCFLAGS} -AllFourFile=$PWD/BBAllFour.txt"

./configure --prefix=$PWD/installed-AllFour
make -j6
make install
make clean

unset GET_ALLFOUR



echo "all done....fuzzing it!\n"
rm -rf out-*