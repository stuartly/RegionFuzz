rm -rf BB*.txt
export NjorthFuzzPath="$PWD/../../NjorthFuzz"


# Orginal
export CC=${NjorthFuzzPath}/afl-clang-fast
export CXX=${NjorthFuzzPath}/afl-clang-fast++
export OrgCFLAGS=${CFLAGS}
export OrgCXXFLAGS=${CXXFLAGS}
cmake -D CMAKE_INSTALL_PREFIX=$PWD/installed-Org CMAKE_C_COMPILER=${CC} CMAKE_CXX_COMPILER=${CXX} CMAKE_C_FLAGS=${CFLAGS} CMAKE_CXX_FLAGS=${CXXFLAGS} ./CMakeLists.txt
make -j6 
make install
make clean


# GET_MEM_DENSITY
export GET_MEM_DENSITY=1

export CC=${NjorthFuzzPath}/afl-clang-preprocess
export CXX=${NjorthFuzzPath}/afl-clang-preprocess++
export CFLAGS="${OrgCFLAGS} -outDir=$PWD"
export CXXFLAGS="${OrgCFLAGS} -outDir=$PWD"

cmake -D CMAKE_INSTALL_PREFIX=$PWD/installed-MemDensity CMAKE_C_COMPILER=${CC} CMAKE_CXX_COMPILER=${CXX} CMAKE_C_FLAGS=${CFLAGS} CMAKE_CXX_FLAGS=${CXXFLAGS} ./CMakeLists.txt
make -j6
make install
make clean

export CC=${NjorthFuzzPath}/afl-clang-fast
export CXX=${NjorthFuzzPath}/afl-clang-fast++
export CFLAGS="${OrgCFLAGS} -MemDensityFile=$PWD/BBMemDensity.txt"
export CXXFLAGS="${OrgCFLAGS} -MemDensityFile=$PWD/BBMemDensity.txt"

cmake -D CMAKE_INSTALL_PREFIX=$PWD/installed-MemDensity CMAKE_C_COMPILER=${CC} CMAKE_CXX_COMPILER=${CXX} CMAKE_C_FLAGS=${CFLAGS} CMAKE_CXX_FLAGS=${CXXFLAGS} ./CMakeLists.txt
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

cmake -D CMAKE_INSTALL_PREFIX=$PWD/installed-InstNum CMAKE_C_COMPILER=${CC} CMAKE_CXX_COMPILER=${CXX} CMAKE_C_FLAGS=${CFLAGS} CMAKE_CXX_FLAGS=${CXXFLAGS} ./CMakeLists.txt
make -j6
make install
make clean

export CC=${NjorthFuzzPath}/afl-clang-fast
export CXX=${NjorthFuzzPath}/afl-clang-fast++
export CFLAGS="${OrgCFLAGS} -InstNumFile=$PWD/BBInstNum.txt"
export CXXFLAGS="${OrgCFLAGS} -InstNumFile=$PWD/BBInstNum.txt"

cmake -D CMAKE_INSTALL_PREFIX=$PWD/installed-InstNum CMAKE_C_COMPILER=${CC} CMAKE_CXX_COMPILER=${CXX} CMAKE_C_FLAGS=${CFLAGS} CMAKE_CXX_FLAGS=${CXXFLAGS} ./CMakeLists.txt
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

cmake -D CMAKE_INSTALL_PREFIX=$PWD/installed-EntryDegree CMAKE_C_COMPILER=${CC} CMAKE_CXX_COMPILER=${CXX} CMAKE_C_FLAGS=${CFLAGS} CMAKE_CXX_FLAGS=${CXXFLAGS} ./CMakeLists.txt
make -j6
make install
make clean

export CC=${NjorthFuzzPath}/afl-clang-fast
export CXX=${NjorthFuzzPath}/afl-clang-fast++
export CFLAGS="${OrgCFLAGS} -EntryDegreeFile=$PWD/BBEntryDegree.txt"
export CXXFLAGS="${OrgCFLAGS} -EntryDegreeFile=$PWD/BBEntryDegree.txt"

cmake -D CMAKE_INSTALL_PREFIX=$PWD/installed-EntryDegree CMAKE_C_COMPILER=${CC} CMAKE_CXX_COMPILER=${CXX} CMAKE_C_FLAGS=${CFLAGS} CMAKE_CXX_FLAGS=${CXXFLAGS} ./CMakeLists.txt
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

cmake -D CMAKE_INSTALL_PREFIX=$PWD/installed-Depth CMAKE_C_COMPILER=${CC} CMAKE_CXX_COMPILER=${CXX} CMAKE_C_FLAGS=${CFLAGS} CMAKE_CXX_FLAGS=${CXXFLAGS} ./CMakeLists.txt
make -j6
make install
make clean

export CC=${NjorthFuzzPath}/afl-clang-fast
export CXX=${NjorthFuzzPath}/afl-clang-fast++
export CFLAGS="${OrgCFLAGS} -DepthFile=$PWD/BBDepth.txt"
export CXXFLAGS="${OrgCFLAGS} -DepthFile=$PWD/BBDepth.txt"

cmake -D CMAKE_INSTALL_PREFIX=$PWD/installed-Depth CMAKE_C_COMPILER=${CC} CMAKE_CXX_COMPILER=${CXX} CMAKE_C_FLAGS=${CFLAGS} CMAKE_CXX_FLAGS=${CXXFLAGS} ./CMakeLists.txt
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

cmake -D CMAKE_INSTALL_PREFIX=$PWD/installed-AllFour CMAKE_C_COMPILER=${CC} CMAKE_CXX_COMPILER=${CXX} CMAKE_C_FLAGS=${CFLAGS} CMAKE_CXX_FLAGS=${CXXFLAGS} ./CMakeLists.txt
make -j6
make install
make clean

export CC=${NjorthFuzzPath}/afl-clang-fast
export CXX=${NjorthFuzzPath}/afl-clang-fast++
export CFLAGS="${OrgCFLAGS} -AllFourFile=$PWD/BBAllFour.txt"
export CXXFLAGS="${OrgCFLAGS} -AllFourFile=$PWD/BBAllFour.txt"

cmake -D CMAKE_INSTALL_PREFIX=$PWD/installed-AllFour CMAKE_C_COMPILER=${CC} CMAKE_CXX_COMPILER=${CXX} CMAKE_C_FLAGS=${CFLAGS} CMAKE_CXX_FLAGS=${CXXFLAGS} ./CMakeLists.txt
make -j6
make install
make clean

unset GET_ALLFOUR



echo "all done....fuzzing it!\n"
rm -rf out-*