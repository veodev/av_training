#!/bin/bash

export DISPLAY=:0
startx &
sleep 1
LD_LIBRARY_PATH+=/opt/qt-linux/5.7/gcc_64/lib:/usr/local/qwt-6.1.2/lib /build/testing/A31_DESKTOP/avicon31 &
sleep 20
killall avicon31
pkill X