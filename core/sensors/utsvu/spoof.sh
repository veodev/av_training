#!/bin/sh

export DEV_IMAGE=/opt/ltib/rootfs
export TOOLCHAIN_PREFIX=arm-none-linux-gnueabi-
export TOOLCHAIN_PATH=/usr/local/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_GNU_Linux
export PATH=/usr/local/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_GNU_Linux/bin:/opt/freescale/ltib/usr/spoof:$PATH
export QT_VERSION=5.4.0

/opt/ltib/rpm/BUILD/qt-everywhere-opensource-src-${QT_VERSION}/qtbase/bin/qmake /opt/ltib/rpm/BUILD/qt-everywhere-opensource-src-${QT_VERSION}/qtsensors/src/plugins/sensors/utsvu/utsvu.pro -o Makefile && make -f Makefile && \
make && \
make install
