#!/bin/bash

usage() {
    echo "usage: $0 [parameters]"
    echo "	--help"
    echo "	--qt-dir <QTDIR>"
    echo "	--qwt-dir <QWT_ROOT>"
    echo "	--toolchain <toolchain>"
    echo "		available toolchains:"
    for i in `ls cmake/toolchain`
    do
	echo "			" `echo "$i" | sed 's/\.cmake$//'`
    done
    echo "	--build-dir <BUILD DIR>"
    echo "	--sysroot <SYSROOT DIR>"
    exit
}

while [ "$1" != "" ]
do
    case "$1" in
	--help)
	    usage
	;;

	--qt-dir)
	    export QTDIR=$2
	    shift
	;;

	--qwt-dir)
	    export QWT_ROOT=$2
	    shift
	;;

	--toolchain)
	    export TOOLCHAIN=$2
	    shift
	;;

	--build-dir)
	    export BUILD_DIR=$2
	    shift
	;;
	--sysroot)
	    export SYSROOT=$2
	    shift
	;;
	*)
	    echo "Unsupport parameter '$1'"
	    usage
    esac

    shift
done

# script variables
export PROJECT_DIR=$PWD

echo "PROJECT_DIR: " $PROJECT_DIR
echo "QTDIR: " $QTDIR
echo "QWT_ROOT: " $QWT_ROOT
echo "TOOLCHAIN: " $TOOLCHAIN
echo "BUILD_DIR: " $BUILD_DIR
echo "SYSROOT: " $SYSROOT
# clean build directory
if [ -d ${BUILD_DIR} ];
then
    rm -rv ${BUILD_DIR} || exit 1
    echo '-- Existing build directory sucessfully cleaned'
fi

sh ./tools/set-cpu-cores-count.sh

mkdir ${BUILD_DIR}
pushd ${BUILD_DIR}
cmake ${PROJECT_DIR} || exit 1
echo '-- CMake successfully configured'
make -j${CPU_CORES_COUNT} || exit 1
echo '-- build successful'
popd

unset BUILD_DIR
unset CPU_CORES_COUNT
