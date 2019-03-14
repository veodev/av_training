#!/bin/bash

echo "First: $1"


case "$1" in
    "GCC" )
    echo " -DCMAKE_CXX_COMPILER=g++ -DCMAKE_CC_COMPILER=gcc " > ./build/compiler.txt
    ;;
    "Clang" )
    echo " -DCMAKE_CXX_COMPILER=clang++ -DCMAKE_CC_COMPILER=clang " > ./build/compiler.txt
    ;;
    "BR-IMX" )
    echo " -DCMAKE_CXX_COMPILER=g++ -DCMAKE_CC_COMPILER=gcc" > ./build/compiler.txt
    ;;
    "Android" )
    echo " -DCMAKE_CXX_COMPILER=g++ -DCMAKE_CC_COMPILER=gcc"> ./build/compiler.txt
    ;;
esac

echo "End!"

exit 0
