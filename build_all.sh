#!/bin/sh

echo "========================"
echo "Building Avicon project!"
echo "========================"
echo ""
echo "Compiler: $1"
echo "Qt: $2"
echo "Build type: $3"
echo "CPU Count: $4"
echo "Other options: $5"

rm -rf ./build
mkdir -p ./build

# -DCMAKE_CXX_COMPILER_LAUNCHER=ccache

case "$1" in
    "GCC" )
    COMPILER=" -DCMAKE_CXX_COMPILER=g++ -DCMAKE_CC_COMPILER=gcc "
    ;;
    "Clang" )
    COMPILER=" -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CC_COMPILER=clang "
    ;;
    "BR-IMX" )
    COMPILER=" -DCMAKE_TOOLCHAIN_FILE=../toolchains/buildroot.cmake "
    ;;
    "Android" )
    COMPILER=" -DCMAKE_CXX_COMPILER=g++ -DCMAKE_CC_COMPILER=gcc "
    ;;
esac

cd ./build
cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=$3 -DCMAKE_PREFIX_PATH=/qt/$2/gcc_64/lib/cmake $5 $COMPILER --clean-first ../ && make all -j $4




