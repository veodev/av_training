#!/bin/bash
export ANDROID_HOME=/opt/android/AndroidSDK/Sdk
export ANDROID_NDK_HOST=linux-x86_64
export ANDROID_NDK_PLATFORM=android-17
export ANDROID_NDK_ROOT=/opt/android/AndroidSDK/Sdk/ndk-bundle
export ANDROID_NDK_TOOLCHAIN_PREFIX=arm-linux-androideabi
export ANDROID_NDK_TOOLCHAIN_VERSION=4.9
export ANDROID_NDK_TOOLS_PREFIX=arm-linux-androideabi
export ANDROID_SDK_ROOT=/opt/android/AndroidSDK/Sdk
export JAVA_HOME=/opt/jdk/current
export PATH=$PATH:/opt/android/qt-5.6-adnroid/5.6/android_armv7/bin
export NAUTIZ_BUM_DRIVER_PATH=/opt/avicon15-bum-driver/ndk
export QWT_LOCATION=/usr/local
export QWT_VERSION=6.1.2
mkdir build
cd build
qmake ../avicon.pro -spec android-g++ CONFIG-=debug CONFIG+=release CONFIG+=avicon15 && /usr/bin/make qmake_all
lupdate ../avicon.pro -ts ../ui/widgets/internationalization/russian.ts
lrelease ../ui/widgets/internationalization/russian.ts -qm ../ui/widgets/internationalization/russian.qm
lupdate ../avicon.pro -ts ../ui/widgets/internationalization/german.ts
lrelease ../ui/widgets/internationalization/german.ts -qm ../ui/widgets/internationalization/german.qm
make clean
make -j 3