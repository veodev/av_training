TEMPLATE = lib
CONFIG += plugin
TARGET = $$qtLibraryTarget(qtsensors_utsvu)
PLUGIN_TYPE = sensors

QT = sensors core

DEFINES += QT_NO_CAST_FROM_ASCII QT_NO_CAST_TO_ASCII
DEFINES += QT_NO_DEBUG

load(qt_plugin)

HEADERS += utsvusensorbackend.h \
    utsvutemperaturesensor.h \
    ds18s20.h \
    debug.h

SOURCES += utsvusensorbackend.cpp \
    utsvutemperaturesensor.cpp \
    ds18s20.cpp \
    main.cpp

OTHER_FILES = plugin.json
