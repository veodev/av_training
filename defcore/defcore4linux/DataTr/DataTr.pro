QT       += core
QT       -= gui

TARGET = DataTr
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += ../..

SOURCES += main.cpp \
    ../../DataTr.cpp \
    ../../platforms.cpp \
    ../../TickCount.cpp \
    ../../LANLinSocket.cpp

HEADERS += \
    ../../DataTr.h \
    ../../platforms.h \
    ../../systemdetection.h \
    ../../TickCount.h \
    ../../LANLinSocket.h
