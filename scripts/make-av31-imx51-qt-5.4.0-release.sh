#!/bin/bash

export PATH=/bin:/usr/local/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_GNU_Linux/bin:/usr/bin:/usr/local/qt-5.4.0-host/bin:/usr/local/CodeSourcery/Sourcery_CodeBench_Lite_for_ARM_GNU_Linux/
export QWT_LOCATION=/usr/local
export QWT_VERSION=6.1.2-qt-5.4.0
export ROOTFS=/opt/ltib/rootfs
mkdir build
cd build
qmake ../avicon.pro -spec devices/linux-imx51-g++ CONFIG+=release CONFIG+=avicon31 CONFIG-=debug && /usr/bin/make qmake_all
lupdate avicon.pro -ts ../ui/widgets/internationalization/russian.ts
lrelease ../ui/widgets/internationalization/russian.ts -qm ../ui/widgets/internationalization/russian.qm
lupdate ../avicon.pro -ts ../ui/widgets/internationalization/german.ts
lrelease ../ui/widgets/internationalization/german.ts -qm ../ui/widgets/internationalization/german.qm
make clean
make -j 3