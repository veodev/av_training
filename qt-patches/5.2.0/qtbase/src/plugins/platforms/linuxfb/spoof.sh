#!/bin/sh
export DEV_IMAGE=/opt/ltib/rootfs
export TOOLCHAIN_PREFIX=arm-none-linux-gnueabi-
export TOOLCHAIN_PATH=/usr/local/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_GNU_Linux
export PATH=/usr/local/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_GNU_Linux/bin:/opt/freescale/ltib/usr/spoof:$PATH

/usr/local/qt-5.2.0-host/bin/qmake linuxfb.pro && \
make && \
make install
