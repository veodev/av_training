#!/bin/bash

export PATH="/usr/lib/ccache:$PATH"
export PATH=$PATH:/opt/qt-linux/5.7/gcc_64/bin
export QWT_LOCATION=/usr/local
export QWT_VERSION=6.1.2
mkdir build
cd build
qmake ../avicon.pro -spec linux-g++ CONFIG+=avicon15 CONFIG+=debug CONFIG-=release
lupdate ../avicon.pro -ts ../ui/widgets/internationalization/russian.ts
lrelease ../ui/widgets/internationalization/russian.ts -qm ../ui/widgets/internationalization/russian.qm
lupdate ../avicon.pro -ts ../ui/widgets/internationalization/german.ts
lrelease ../ui/widgets/internationalization/german.ts -qm ../ui/widgets/internationalization/german.qm
make qmake_all
make -j 3
