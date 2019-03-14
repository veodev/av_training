QT += core testlib
QT -= gui

TARGET = WaitCondition
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

INCLUDEPATH += \
    ../.. \
    ../../..

SOURCES += \
    main.cpp \
    ../../../EventManager_Lin.cpp \
    ../../../EventManager_Qt.cpp \
    ../../../ThreadClassList_Lin.cpp \
    ../../../LinThread.cpp \
    ../../../ThreadClassList.cpp \
    ../../../platforms.cpp \
    test_EventManager.cpp

HEADERS += \
    ../../../EventManager_Lin.h \
    ../../../EventManager_Qt.h \
    ../../../ThreadClassList_Lin.h \
    ../../../LinThread.h \
    ../../../ThreadClassList.h \
    ../../../EventManager.h \
    ../../../platforms.h \
    test_EventManager.h
