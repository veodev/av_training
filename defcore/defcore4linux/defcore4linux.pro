QT      += core
QT      -= gui

TARGET  =  defcore4linux
CONFIG  += console
CONFIG  -= app_bundle

TEMPLATE = app

INCLUDEPATH += \
    .. \
    ./

linux-g++ | linux-g++-64 | linux-imx51-g++ | android | mingw {
    SOURCES += \
        ../CriticalSection_Lin.cpp \
        ../EventManager_Lin.cpp \
        ../LinThread.cpp \
        ../ThreadClassList_Lin.cpp

    HEADERS += \
        ../CriticalSection_Lin.h \
        ../EventManager_Lin.h \
        ../LinThread.h \
        ../ThreadClassList_Lin.h

    win32 {
        SOURCES +=
        HEADERS +=
        LIBS += -lws2_32
    } else {
        SOURCES +=
        HEADERS +=
    }
}

SOURCES += main.cpp \
    ../platforms.cpp \
    ../ThreadClassList.cpp \
    ../TickCount.cpp \
    ../ChannelsTable.cpp \
    ../DeviceConfig_Avk31.cpp \
    ../DeviceConfig.cpp \
    ../DeviceCalibration.cpp \
    ../Device.cpp \
    ../sockets/isocket.cpp \
    ../sockets/socket_can.cpp \
    ../sockets/socket_lan.cpp \
    ../datatransfers/datatransfer_can.cpp \
    ../datatransfers/datatransfer_lan.cpp \
    ../datatransfers/idatatransfer.cpp \
    ../prot_umu/iumu.cpp \
    ../prot_umu/prot_umu_can.cpp \
    ../prot_umu/prot_umu_lan.cpp


HEADERS += \
    ../platforms.h \
    ../ThreadClassList.h \
    ../TickCount.h \
    ../CriticalSection.h \
    ../CriticalSection_Lin.h \
    ../EventManager.h \
    ../ChannelsTable.h \
    ../DeviceConfig_test.h \
    ../Definitions.h \
    ../DeviceConfig.h \
    ../DeviceCalibration.h \
    ../Device.h \
    ..prot_umu/prot_umu_lan.h \
    ../systemdetection.h \
    Autocon.inc \
    ../sockets/socket_can.h \
    ../sockets/socket_lan.h \
    ../sockets/isocket.h \
    ../datatransfers/datatransfer_can.h \
    ../datatransfers/datatransfer_lan.h \
    ../datatransfers/idatatransfer.h \
    ../prot_umu_lan_constants.h \
    ../prot_umu/iumu.h \
    ../prot_umu/prot_umu_can.h \
    ../prot_umu/prot_umu_lan.h \
    ../prot_umu/prot_umu_can_constants.h \
    ../prot_umu/prot_umu_lan_constants.h \
    ../prot_umu/utils_can.h
