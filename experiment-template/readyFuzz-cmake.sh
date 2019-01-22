# out-of-source compile!!!!

rm -rf ../BB*.txt
rm -rf ../installed-*/
export NjorthFuzzPath="$PWD/../../NjorthFuzz"

mkdir build
cd build

# Orginal
export CC="${NjorthFuzzPath}/afl-clang-fast"
export CXX="${NjorthFuzzPath}/afl-clang-fast++"
export OrgCFLAGS="${CFLAGS} -m64"
export OrgCXXFLAGS="${CXXFLAGS} -m64"
cmake -D CMAKE_INSTALL_PREFIX=$PWD/../installed-Org ..
make -j6 
make install
make clean
rm -rf *


# GET_MEM_DENSITY
export GET_MEM_DENSITY=1

export CC="${NjorthFuzzPath}/afl-clang-preprocess"
export CXX="${NjorthFuzzPath}/afl-clang-preprocess++"
export CFLAGS="${OrgCFLAGS} -outDir=$PWD/../" # '-m64' add in OrgCFLAGS before
export CXXFLAGS="${OrgCXXFLAGS} -outDir=$PWD/../"

cmake -D CMAKE_INSTALL_PREFIX=$PWD/../installed-MemDensity ..
make -j6
make install
make clean
rm -rf *

export CC="${NjorthFuzzPath}/afl-clang-fast -m64"
export CXX="${NjorthFuzzPath}/afl-clang-fast++ -m64"
export CFLAGS="${OrgCFLAGS} -MemDensityFile=$PWD/../BBMemDensity.txt"
export CXXFLAGS="${OrgCXXFLAGS} -MemDensityFile=$PWD/../BBMemDensity.txt"

cmake -D CMAKE_INSTALL_PREFIX=$PWD/../installed-MemDensity ..
make -j6
make install
make clean
rm -rf *

unset GET_MEM_DENSITY


# GET_INST_NUM
export GET_INST_NUM=1

export CC="${NjorthFuzzPath}/afl-clang-preprocess"
export CXX="${NjorthFuzzPath}/afl-clang-preprocess++"
export CFLAGS="${OrgCFLAGS} -outDir=$PWD/../"
export CXXFLAGS="${OrgCXXFLAGS} -outDir=$PWD/../"

cmake -D CMAKE_INSTALL_PREFIX=$PWD/../installed-InstNum ..
make -j6
make install
make clean
rm -rf *

export CC="${NjorthFuzzPath}/afl-clang-fast -m64"
export CXX="${NjorthFuzzPath}/afl-clang-fast++ -m64"
export CFLAGS="${OrgCFLAGS} -InstNumFile=$PWD/../BBInstNum.txt"
export CXXFLAGS="${OrgCXXFLAGS} -InstNumFile=$PWD/../BBInstNum.txt"

cmake -D CMAKE_INSTALL_PREFIX=$PWD/../installed-InstNum ..
make -j6
make install
make clean
rm -rf *

unset GET_INST_NUM


# GET_ENTRY_DEGREE
export GET_ENTRY_DEGREE=1

export CC="${NjorthFuzzPath}/afl-clang-preprocess"
export CXX="${NjorthFuzzPath}/afl-clang-preprocess++"
export CFLAGS="${OrgCFLAGS} -outDir=$PWD/../"
export CXXFLAGS="${OrgCXXFLAGS} -outDir=$PWD/../"

cmake -D CMAKE_INSTALL_PREFIX=$PWD/../installed-EntryDegree ..
make -j6
make install
make clean
rm -rf *

export CC="${NjorthFuzzPath}/afl-clang-fast -m64"
export CXX="${NjorthFuzzPath}/afl-clang-fast++ -m64"
export CFLAGS="${OrgCFLAGS} -EntryDegreeFile=$PWD/../BBEntryDegree.txt"
export CXXFLAGS="${OrgCXXFLAGS} -EntryDegreeFile=$PWD/../BBEntryDegree.txt"

cmake -D CMAKE_INSTALL_PREFIX=$PWD/../installed-EntryDegree ..
make -j6
make install
make clean
rm -rf *

unset GET_ENTRY_DEGREE



# GET_DEPTH
export GET_DEPTH=1

export CC="${NjorthFuzzPath}/afl-clang-preprocess"
export CXX="${NjorthFuzzPath}/afl-clang-preprocess++"
export CFLAGS="${OrgCFLAGS} -outDir=$PWD/../"
export CXXFLAGS="${OrgCXXFLAGS} -outDir=$PWD/../"

cmake -D CMAKE_INSTALL_PREFIX=$PWD/../installed-Depth ..
make -j6
make install
make clean
rm -rf *

export CC="${NjorthFuzzPath}/afl-clang-fast -m64"
export CXX="${NjorthFuzzPath}/afl-clang-fast++ -m64"
export CFLAGS="${OrgCFLAGS} -DepthFile=$PWD/../BBDepth.txt"
export CXXFLAGS="${OrgCXXFLAGS} -DepthFile=$PWD/../BBDepth.txt"

cmake -D CMAKE_INSTALL_PREFIX=$PWD/../installed-Depth ..
make -j6
make install
make clean
rm -rf *

unset GET_DEPTH



# GET_ALLFOUR
export GET_ALLFOUR=1

export CC="${NjorthFuzzPath}/afl-clang-preprocess"
export CXX="${NjorthFuzzPath}/afl-clang-preprocess++"
export CFLAGS="${OrgCFLAGS} -outDir=$PWD/../"
export CXXFLAGS="${OrgCXXFLAGS} -outDir=$PWD/../"

cmake -D CMAKE_INSTALL_PREFIX=$PWD/../installed-AllFour ..
make -j6
make install
make clean
rm -rf *

export CC="${NjorthFuzzPath}/afl-clang-fast -m64"
export CXX="${NjorthFuzzPath}/afl-clang-fast++ -m64"
export CFLAGS="${OrgCFLAGS} -AllFourFile=$PWD/../BBAllFour.txt"
export CXXFLAGS="${OrgCXXFLAGS} -AllFourFile=$PWD/../BBAllFour.txt"

cmake -D CMAKE_INSTALL_PREFIX=$PWD/../installed-AllFour ..
make -j6
make install
make clean
rm -rf *

unset GET_ALLFOUR


cd ..
echo "all done....fuzzing it!\n"
rm -rf out-*