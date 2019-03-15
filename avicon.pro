QT += core gui widgets sensors multimedia positioning
QT += svg xml sql bluetooth

#For using ccache install package in your operating system.
#QMAKE_CXX=ccache g++

TEMPLATE = app



#TARGET = avicon31

# gprof profiler flags:
#QMAKE_CFLAGS+=-pg
#QMAKE_CXXFLAGS+=-pg
#QMAKE_LFLAGS+=-pg

QMAKE_LFLAGS+=-static-libstdc++
CONFIG += c++14
# gcov code coverage flags:
#CONFIG+=gcov

#CONFIG += sanitizer sanitize_address

#DEFINES += IMX_DEVICE

avicon31 {
    message("avicon31")
    TARGET = avicon31
    DEFINES += TARGET_AVICON31
    linux-buildroot-g++ | linux-imx51-g++ {
        DEFINES += IMX_DEVICE
        target.path = /usr/local/avicon-31
        INSTALLS += target
    }
} else:avicondbhs {
    message("avicondbhs")
    TARGET = avicondbhs
    DEFINES += TARGET_AVICONDBHS
#    CONFIG += c++11
    linux-g++-32 {
        DEFINES += ATOM_DEVICE
        target.path = /usr/local/avicon-dbhs
        INSTALLS += target
    }
} else:avicondb {
    message("avicondb")
    TARGET = avicondb
    DEFINES += TARGET_AVICONDB
#    CONFIG += c++11
    linux-g++-32 {
        DEFINES += ATOM_DEVICE
        target.path = /usr/local/avicon-db
        INSTALLS += target
    }
} else:avicon15 {
    message("avicon15")
    TARGET = avicon15
    DEFINES += TARGET_AVICON15
#    CONFIG += c++11
} else:avicon31e {
    TARGET = avicon31e
    DEFINES += TARGET_AVICON31E
    message("avicon31e")
} else {
    error("Unknown target")
}

cross_compile {
    message("cross_compile")
}
linux-buildroot-g++ {
    message("Process project for i.MX51 device")
}
linux-g++ {
    message("linux-g++")
}
linux-g++-32 {
    message("linux-g++32")
}
linux-g++-64 {
    message("linux-g++-64")
}
android {
    message("android")
}
win32 {
    message("win32")
}
unix {
    message("unix")
}
mingw {
    message("mingw")
}
gcc {
    message("gcc")
}
debug {
    message("debug")
}
release {
    message("release")
}

include(./version.pri)

simulation {
    message("simulation")
    DEFINES += SIMULATION
}

message("CONFIG:"$$CONFIG)
clang {
    #`llvm-config --cxxflags --ldflags --system-libs --libs engine interpreter` -fexceptions
    QMAKE_CXXFLAGS += `llvm-config --cxxflags` -fsanitize=integer,undefined -fno-omit-frame-pointer
    QMAKE_LFLAGS += `llvm-config --ldflags --libs all` -fsanitize=integer,undefined -fno-omit-frame-pointer
}
CONFIG(debug,debug|release) {
    #DEBUG_SFX =
    DEFINES += DEFCORE_DEBUG
} else {
    #DEBUG_SFX =
    DEFINES += DEFCORE_DEBUG
}

# qwt
QWT_LIB = qwt$${DEBUG_SFX}
message("QWT_LIB:" $${QWT_LIB})

QWT_VERSION = $$(QWT_VERSION)
message("QWT_VERSION:" $${QWT_VERSION})

QWT_LOCATION = $$(QWT_LOCATION)
message("QWT_LOCATION:" $${QWT_LOCATION})

QWT_INC_PATH = "NO QWT!"

cross_compile {
    android {
       contains(ANDROID_TARGET_ARCH,armeabi-v7a) {
            ANDROID_EXTRA_LIBS = $${QWT_LOCATION}/qwt-$${QWT_VERSION}.android/lib/libqwt.so
        }
        QWT_INC_PATH = $${QWT_LOCATION}/qwt-$${QWT_VERSION}.android/include
        LIBS += -L$${QWT_LOCATION}/qwt-$${QWT_VERSION}.android/lib -l$${QWT_LIB}
    } win32 {
        QWT_INC_PATH = $${QWT_LOCATION}/qwt-$${QWT_VERSION}.windows/include
        LIBS += -L$${QWT_LOCATION}/qwt-$${QWT_VERSION}.windows/lib -l$${QWT_LIB}
    } else {
        ROOTFS = $$(ROOTFS)
        message("ROOTFS:" $${ROOTFS})
        QWT_INC_PATH = $${ROOTFS}$${QWT_LOCATION}/qwt-$${QWT_VERSION}/include
        LIBS += -Wl,-rpath,$${QWT_LOCATION}/qwt-$${QWT_VERSION}/lib \
                        -L$${ROOTFS}$${QWT_LOCATION}/qwt-$${QWT_VERSION}/lib -L$${QWT_LOCATION}/qwt-$${QWT_VERSION}/lib -l$${QWT_LIB}
    }
} else {
    equals(QWT_LOCATION, "") {
        QWT_INC_PATH = /usr/include/qwt
        LIBS += -l$${QWT_LIB}
    } else {
        QWT_INC_PATH = $${QWT_LOCATION}/qwt-$${QWT_VERSION}/include
        INCLUDEPATH += $${QWT_INC_PATH}
        LIBS += -L$${QWT_LOCATION}/qwt-$${QWT_VERSION}/lib -l$${QWT_LIB}
    }
    LIBS += -ldl
}

INCLUDEPATH += $${QWT_INC_PATH}
message("QWT_INCLUDE_PATH:" $$QWT_INC_PATH)

# geoposition
avicon15:android {
message("GEOPOSITION ANDROID")
INCLUDEPATH += \
    core/geoposition/android
HEADERS += \
    core/androidJNI.h \
    core/geoposition/android/geoposition.h

SOURCES += \
    core/androidJNI.cpp \
    core/geoposition/android/geoposition.cpp
}

avicon15 | avicondb {
    linux-g++ | linux-g++-32 {
    message("GEOPOSITION FAKE")
    INCLUDEPATH += \
        core/geoposition/fake
    HEADERS += core/geoposition/fake/geoposition.h
    SOURCES += core/geoposition/fake/geoposition.cpp
    }
}

avicon31 {
    linux-buildroot-g++ | linux-imx51-g++ | linux-g++ | linux-g++-32 {
    message("GEOPOSITION LINUX")
    QT += serialport
    INCLUDEPATH += \
        core/geoposition
    SOURCES += \
        core/geoposition/geoposition.cpp \
        core/geoposition/locationutils.cpp \
        core/geoposition/nmeareader.cpp \
        core/geoposition/nmeapositioninfosource.cpp \
        core/geoposition/nmeasatelliteinfosource.cpp

    HEADERS += \
        core/geoposition/geoposition.h \
        core/geoposition/locationutils.h \
        core/geoposition/nmeareader.h \
        core/geoposition/nmeapositioninfosource.h \
        core/geoposition/nmeasatelliteinfosource.h
    }
}

# imx51 board specific features
avicon31:linux-buildroot-g++ | linux-imx51-g++ {
    message("BLUETOOTH QT")
    SOURCES += \
        core/externalkeyboard/externalkeyboard.cpp \
        core/screen/brightness_utsvu.cpp \
        core/screen/screen_utsvu.cpp \
        core/bluetooth/bluetoothmanagerqt.cpp
    HEADERS += \
        core/bluetooth/bluetoothmanagerqt.h
    message("USE ALSA!")
}

if (avicon15) {
    if (android-g++) {
        message("BLUETOOTH ANDROID")
        HEADERS += \
            core/bluetooth/bluetoothmanagerqt.h \
            core/bluetooth/bluetoothmanagerandroid.h
        SOURCES += \
            core/bluetooth/bluetoothmanagerqt.cpp \
            core/bluetooth/bluetoothmanagerandroid.cpp
    }
}

avicon31:win32 {
    SOURCES += \
        core/externalkeyboard/externalkeyboardfake.cpp \
        core/screen/brightness_fake.cpp \
        core/screen/screen_fake.cpp
}
avicon31:linux-g++-32 {
    message("BLUETOOTH FAKE")
    SOURCES += \
        core/externalkeyboard/externalkeyboardfake.cpp \
        core/screen/brightness_fake.cpp \
        core/screen/screen_fake.cpp \
        core/bluetooth/fakebluetoothmanager.cpp
    HEADERS += \
        core/bluetooth/fakebluetoothmanager.h
}

avicondbhs | avicondb | avicon31 {
    linux-g++-32 | linux-g++ {
        message("BRIGHTNESS ADVANTECH")
        message("BLUETOOTH FAKE")
        SOURCES += \
            core/externalkeyboard/externalkeyboardfake.cpp \
            core/screen/brightness_advantech.cpp \
            core/screen/screen_fake.cpp \
            core/bluetooth/fakebluetoothmanager.cpp
        HEADERS += \
            core/bluetooth/fakebluetoothmanager.h
    }
}

clang {
    SOURCES += \
        core/externalkeyboard/externalkeyboardfake.cpp \
        core/screen/brightness_fake.cpp \
        core/screen/screen_fake.cpp \
        core/bluetooth/bluetoothmanagerqt.cpp
    HEADERS += \
        core/bluetooth/bluetoothmanagerqt.h
}
avicon15:android {
    SOURCES += \
        core/externalkeyboard/externalkeyboardfake.cpp \
        core/screen/brightness_fake.cpp \
        core/screen/screen_fake.cpp
}
avicon31e:win32 {
    SOURCES += \
        core/externalkeyboard/externalkeyboardfake.cpp \
        core/screen/brightness_fake.cpp \
        core/screen/screen_fake.cpp
}
# Av31 on Win
avicon31:win32 {
INCLUDEPATH += defcore/av11
SOURCES += \
    defcore/prot_umu/prot_umu_usbcan_win.cpp \
    defcore/datatransfers/datatransfer_usbcan_win.cpp \
    defcore/sockets/socket_usbcan_win.cpp
HEADERS += \
    defcore/prot_umu/prot_umu_usbcan_win.h \
    defcore/datatransfers/datatransfer_usbcan_win.h \
    defcore/sockets/socket_usbcan_win.h \
    defcore/av11/av11_21.h
}
avicon31e:win32 {
INCLUDEPATH += defcore/av11
SOURCES += \
    defcore/prot_umu/prot_umu_usbcan_win.cpp \
    defcore/datatransfers/datatransfer_usbcan_win.cpp \
    defcore/sockets/socket_usbcan_win.cpp
HEADERS += \
    defcore/prot_umu/prot_umu_usbcan_win.h \
    defcore/datatransfers/datatransfer_usbcan_win.h \
    defcore/sockets/socket_usbcan_win.h \
    defcore/av11/av11_21.h
}

# Avicon17 Rail Head Scanner

INCLUDEPATH += ui/widgets/av17
SOURCES += \
    ui/widgets/av17/av17defview.cpp \
    ui/widgets/av17/av17page.cpp \
    ui/widgets/av17/av17plot.cpp \
    ui/widgets/av17/DataFileUnit.cpp \
    ui/widgets/av17/av17projections.cpp
HEADERS += \
    ui/widgets/av17/av17defview.h \
    ui/widgets/av17/av17page.h \
    ui/widgets/av17/av17plot.h \
    ui/widgets/av17/DataFileUnit.h \
    ui/widgets/av17/av17projections.h
FORMS += \
    ui/widgets/av17/av17defview.ui \
    ui/widgets/av17/av17page.ui

# powermanagement
INCLUDEPATH += core/powermanagement
SOURCES += \
    core/powermanagement/powermanagement.cpp

HEADERS += \
    core/powermanagement/battery.h \
    core/powermanagement/powermanagement.h

FORMS += \
    ui/widgets/trackmarks/trackmarksrussian.ui \
    ui/widgets/trackmarks/trackmarksrussianselector.ui \
    ui/widgets/trackmarks/trackmarksmetricselector.ui \
    ui/widgets/trackmarks/trackmarksmetric1km.ui
linux-buildroot-g++ | linux-imx51-g++ {
    DEFINES += USE_UTSVU_POWERMANAGEMENT
    SOURCES += core/powermanagement/utsvupowermanagement.cpp
    HEADERS += core/powermanagement/utsvupowermanagement.h
}

avicondbhs | avicondb {
    linux-g++-32 | linux-g++ {
        message("POWERMANAGMENT ADVANTECH")
        DEFINES += USE_ADVANTECH_POWERMANAGEMENT
        SOURCES += core/powermanagement/advantechpowermanagement.cpp
        HEADERS += core/powermanagement/advantechpowermanagement.h
    }
}

# camera
    SOURCES += ui/widgets/videocamerapage.cpp
    HEADERS += ui/widgets/videocamerapage.h
    FORMS += ui/widgets/videocamerapage.ui

linux-buildroot-g++ | linux-imx51-g++ {
    INCLUDEPATH += \
        core/mxccamera \
        ui/widgets/camera/mxccamerapage
    SOURCES += \
        core/mxccamera/mxccamera.cpp \
        core/mxccamera/mxccameraimagecapture.cpp \
        core/mxccamera/mxcmediarecorder.cpp \
        ui/widgets/camera/mxccamerapage/camerapage.cpp
    HEADERS += \
        core/mxccamera/mxccamera.h \
        core/mxccamera/mxccameraimagecapture.h \
        core/mxccamera/mxcmediarecorder.h \
        ui/widgets/camera/mxccamerapage/camerapage.h
    FORMS += \
        ui/widgets/camera/mxccamerapage/camerapage.ui
}

linux-g++|linux-g++-32|win32-g++|clang {
    message("QCAMERA")
    QT += multimediawidgets
    INCLUDEPATH += ui/widgets/camera/qcamerapage
    HEADERS += ui/widgets/camera/qcamerapage/camerapage.h
    SOURCES += ui/widgets/camera/qcamerapage/camerapage.cpp
    FORMS += ui/widgets/camera/qcamerapage/camerapage.ui
}

if (android-g++) {
    message("CAMERA ANDROID")
    CONFIG += mobility
    QT += quickwidgets multimedia-private multimediawidgets
    INCLUDEPATH += \
        ui/widgets/camera/android \
        core/androidcamera
    HEADERS += \
        ui/widgets/camera/android/camerapage.h \
        core/androidcamera/cameraworker.h
        core/androidcamera/camera_advanced.h
    SOURCES += \
        ui/widgets/camera/android/camerapage.cpp \
        core/androidcamera/cameraworker.cpp \
        core/androidcamera/camera_advanced.cpp
    FORMS += ui/widgets/camera/android/camerapage.ui       
}

# audio recorder & player
INCLUDEPATH += \
    ui/widgets/audiocomments/audioplayerpage \
    ui/widgets/audiocomments/audiorecorderpage \
    ui/widgets/audiocomments
HEADERS += \
    ui/widgets/audiocomments/audioplayerpage/audioplayerpage.h \
    ui/widgets/audiocomments/audiorecorderpage/audiorecorderpage.h \
    ui/widgets/audiocomments/audiolevel.h
SOURCES += \
    ui/widgets/audiocomments/audioplayerpage/audioplayerpage.cpp \
    ui/widgets/audiocomments/audiorecorderpage/audiorecorderpage.cpp \
    ui/widgets/audiocomments/audiolevel.cpp
FORMS += \
    ui/widgets/audiocomments/audioplayerpage/audioplayerpage.ui \
    ui/widgets/audiocomments/audiorecorderpage/audiorecorderpage.ui

# video player
if (android-g++) {
    message("VIDEOPLAYER FOR ANDROID")
    QT += quickwidgets
    INCLUDEPATH += ui/widgets/video/videoplayerandroid
    HEADERS += ui/widgets/video/videoplayerandroid/videoplayerpage.h
    SOURCES += ui/widgets/video/videoplayerandroid/videoplayerpage.cpp
    FORMS += ui/widgets/video/videoplayerandroid/videoplayerpage.ui
}
else {
    message("VIDEOPLAYER FOR LINUX")
    QT += multimediawidgets
    INCLUDEPATH += ui/widgets/video/videoplayer
    HEADERS += ui/widgets/video/videoplayer/videoplayerpage.h
    SOURCES += ui/widgets/video/videoplayer/videoplayerpage.cpp
    FORMS += ui/widgets/video/videoplayer/videoplayerpage.ui
}
#QT += multimediawidgets
#INCLUDEPATH += \
#    ui/widgets/video/videoplayer
#HEADERS += \
#    ui/widgets/video/videoplayer/videoplayerpage.h
#SOURCES += \
#    ui/widgets/video/videoplayer/videoplayerpage.cpp
#FORMS += \
#    ui/widgets/video/videoplayer/videoplayerpage.ui

# GPIO
INCLUDEPATH += core/gpio
HEADERS += core/gpio/gpio.h

linux-buildroot-g++ | linux-imx51-g++ {
    SOURCES += core/gpio/gpio.cpp
} else {
    SOURCES += core/gpio/gpio_fake.cpp
}

# wifi
if (linux-g++ | linux-g++-64 | linux-buildroot-g++ | linux-imx51-g++ | gcc) {
    message("WIFI FOR LINUX")
    INCLUDEPATH += \
        core/wifimanager \
        ui/widgets/wifi/linux \
        ui/widgets/wifi/android
    HEADERS += \
        core/wifimanager/wifimanager.h \
        ui/widgets/wifi/linux/wifipage.h \
        ui/widgets/wifi/linux/wifimanagerpage.h
    SOURCES += \
        core/wifimanager/wifimanager.cpp \
        ui/widgets/wifi/linux/wifipage.cpp \
        ui/widgets/wifi/linux/wifimanagerpage.cpp
    FORMS += \
        ui/widgets/wifi/linux/wifipage.ui \
        ui/widgets/wifi/linux/wifimanagerpage.ui
}
else {
    message("WIFI FAKE!!!")
    INCLUDEPATH += ui/widgets/wifi/fake
    HEADERS += ui/widgets/wifi/fake/wifimanagerpage.h
    SOURCES += ui/widgets/wifi/fake/wifimanagerpage.cpp
    FORMS +=   ui/widgets/wifi/fake/wifimanagerpage.ui
}

if (avicon15:android) {
    message("WIFI FOR ANDROID")
    INCLUDEPATH -= \
        core/wifimanager \
        ui/widgets/wifi/linux \
        ui/widgets/wifi/android
    HEADERS -= \
        core/wifimanager/wifimanager.h \
        ui/widgets/wifi/linux/wifipage.h \
        ui/widgets/wifi/linux/wifimanagerpage.h
    SOURCES -= \
        core/wifimanager/wifimanager.cpp \
        ui/widgets/wifi/linux/wifipage.cpp \
        ui/widgets/wifi/linux/wifimanagerpage.cpp
    FORMS -= \
        ui/widgets/wifi/linux/wifipage.ui \
        ui/widgets/wifi/linux/wifimanagerpage.ui

    INCLUDEPATH += ui/widgets/wifi/android
    HEADERS += ui/widgets/wifi/android/wifimanagerpage.h
    SOURCES += ui/widgets/wifi/android/wifimanagerpage.cpp
    FORMS += ui/widgets/wifi/android/wifimanagerpage.ui
}


# virtual keyboards
INCLUDEPATH += \
    ui/widgets/virtualkeyboards
SOURCES += \
    ui/widgets/virtualkeyboards/virtualkeyboards.cpp \
    ui/widgets/virtualkeyboards/keypushbutton.cpp \
    ui/widgets/virtualkeyboards/stickypushbutton.cpp \
    ui/widgets/virtualkeyboards/symbolkeypushbutton.cpp \
    ui/widgets/virtualkeyboards/envirtualkeyboard/envirtualkeyboard.cpp \
    ui/widgets/virtualkeyboards/fnvirtualkeyboard/fnvirtualkeyboard.cpp \
    ui/widgets/virtualkeyboards/ruvirtualkeyboard/ruvirtualkeyboard.cpp \
    ui/widgets/virtualkeyboards/devirtualkeyboard/devirtualkeyboard.cpp
HEADERS += \
    ui/widgets/virtualkeyboards/virtualkeyboards.h \
    ui/widgets/virtualkeyboards/virtualkeyboardbase.h \
    ui/widgets/virtualkeyboards/keypushbutton.h \
    ui/widgets/virtualkeyboards/stickypushbutton.h \
    ui/widgets/virtualkeyboards/symbolkeypushbutton.h \
    ui/widgets/virtualkeyboards/envirtualkeyboard/envirtualkeyboard.h \
    ui/widgets/virtualkeyboards/fnvirtualkeyboard/fnvirtualkeyboard.h \
    ui/widgets/virtualkeyboards/ruvirtualkeyboard/ruvirtualkeyboard.h \
    ui/widgets/virtualkeyboards/devirtualkeyboard/devirtualkeyboard.h
FORMS += \
    ui/widgets/virtualkeyboards/virtualkeyboards.ui \
    ui/widgets/virtualkeyboards/envirtualkeyboard/envirtualkeyboard.ui \
    ui/widgets/virtualkeyboards/fnvirtualkeyboard/fnvirtualkeyboard.ui \
    ui/widgets/virtualkeyboards/ruvirtualkeyboard/ruvirtualkeyboard.ui \
    ui/widgets/virtualkeyboards/devirtualkeyboard/devirtualkeyboard.ui

SOURCES += \
    core/colorschememanager.cpp \
    core/colorscheme.cpp \
    core/core.cpp \
    core/defcore.cpp \
    core/registration.cpp \
    core/settings.cpp \
    core/checkcalibration.cpp \
    core/limitsforsens.cpp \
    core/audio/audiooutput.cpp \
    core/audio/audiogenerator.cpp \
    core/audio/alarmtone.cpp \
    core/audio/alarmtone_p.cpp \
    core/models/channelsmodel.cpp \
    core/models/channelsproxyfiltermodel.cpp \
    core/models/channelsproxytablemodel.cpp \
    core/models/tapemodel.cpp \
    core/appdatapath.cpp \
    core/direction.cpp \
    core/defectmarker.cpp \
    core/ftp/ftpuploader.cpp \
    core/ftp/ftpclient.cpp \
    core/temperature/fake/temperature_fake.cpp \
    core/temperature/generic/genericTempSensor.cpp \
    core/temperature/lm75/lm75.cpp \
    core/temperature/ds1820/ds18s20.cpp \
    core/temperature/temperaturesensormanager.cpp \
    core/powermanagement/battery.cpp \
    core/channelscontroller.cpp \
    core/audio/sounddata.cpp \
    core/passwordmanager.cpp \
    core/trackmarks/tmrussian.cpp \
    core/trackmarks/trackmarks.cpp \
    core/trackmarks/tmmetric.cpp \
    core/trackmarks/tmmetric1km.cpp \
    core/remotecontrol/remotecontrol.cpp \
    core/filemanager.cpp \
    ui/widgets/accessories.cpp \
    ui/widgets/note.cpp \
    ui/widgets/clock.cpp \
    ui/widgets/setdate.cpp \
    ui/widgets/customLabel/customqlabel.cpp \
    ui/widgets/temperature.cpp \
    ui/widgets/soundsettings.cpp \
    ui/widgets/internationalization/languageswitcher.cpp \
    ui/widgets/ascanplot/ascanplot.cpp \
    ui/widgets/ascanplot/gateintervalsymbol.cpp \
    ui/widgets/bscangates.cpp \
    ui/widgets/electrictestpage.cpp \
    ui/widgets/mscanplot/mscanplot.cpp \
    ui/widgets/statusbar.cpp \
    ui/widgets/spinboxes/spinboxbase.cpp \
    ui/widgets/spinboxes/spinboxlist.cpp \
    ui/widgets/spinboxes/spinboxnumber.cpp \
    ui/widgets/spinboxes/spinboxlabel.cpp \
    ui/widgets/spinboxes/spinboxcolor.cpp \
    ui/widgets/spinboxes/limitsforsensspinboxnumber.cpp \
    ui/widgets/controlsarea/controlsarea.cpp \
    ui/widgets/controlsarea/controlswidget.cpp \
    ui/widgets/lateralpanels/lateralpanelfsm.cpp \
    ui/widgets/lateralpanels/handlateralpanelview.cpp \
    ui/widgets/lateralpanels/lateralbuttonitem.cpp \
    ui/widgets/lateralpanels/scanlateralbuttonitem.cpp \
    ui/widgets/lateralpanels/scanlateralbuttonsview.cpp \
    ui/widgets/lateralpanels/scantapeacousticcontactitem.cpp \
    ui/widgets/lateralpanels/scantapeacousticcontactview.cpp \
    ui/widgets/tablesettingsview.cpp \
    ui/widgets/tablesettingsitem.cpp \
    ui/widgets/styles/styles.cpp \
    ui/widgets/messagepage.cpp \
    ui/widgets/pushbuttonwithlongpress/pushbuttonwithlongpress.cpp \
    ui/widgets/handchannelssettingsview.cpp \
    ui/widgets/handchannelssettingsitem.cpp \
    ui/widgets/encoderpage.cpp \
    ui/widgets/indicatorwifi.cpp \
    ui/widgets/imageviewerpage.cpp \
    ui/widgets/screenshotsserviceview.cpp \
    ui/widgets/scannerencoderspage.cpp \
    ui/widgets/playlistpage.cpp \
    ui/widgets/report.cpp \
    ui/widgets/wrongcalibrationmsg.cpp \
    ui/widgets/waitmessagepage.cpp \
    ui/widgets/options/externaltemperatureoptions.cpp \
    ui/widgets/options/screenheateroptions.cpp \
    ui/widgets/options/boltjointoptions.cpp \
    ui/widgets/options/sensautoreset.cpp \
    ui/widgets/systempasswordform.cpp \
    ui/widgets/options/ekasuioptions.cpp \
    ui/widgets/options/optionslistpage.cpp \
    ui/widgets/lateralpanels/handlateralbuttonsview.cpp \
    ui/widgets/batterywidget.cpp \
    ui/widgets/satellitecoordinatespage.cpp \
    ui/widgets/settime.cpp \
    ui/widgets/notifier.cpp \
    ui/widgets/quickbar.cpp \
    ui/widgets/lateralpanels/handlateralpanelitem.cpp \
    ui/widgets/lateralpanels/scanlateralpanelitem.cpp \
    ui/widgets/lateralpanels/scanlateralpanelview.cpp \
    ui/widgets/lateralpanels/scanlateralpaneltapeitem.cpp \
    ui/widgets/satellites.cpp \
    ui/widgets/mscanpage.cpp \
    ui/widgets/splashscreen.cpp \
    ui/widgets/bscanplot.cpp \
    ui/widgets/bscantape.cpp \
    ui/widgets/bscanpage.cpp \
    ui/widgets/memorypage.cpp \
    ui/widgets/main.cpp \
    ui/widgets/ascanpage.cpp \
    ui/widgets/mainwindow.cpp \
    ui/widgets/bluetooth/bluetoothmanagerpage.cpp \
    ui/widgets/selectors/operatorselectionwidget.cpp \
    ui/widgets/selectors/fileplacementwidget.cpp \
    ui/widgets/misc/popupdialogwidget.cpp \
    ui/widgets/selectors/lineselectionwidget.cpp \
    ui/widgets/registration/registrationpage.cpp \
    ui/widgets/registration/defaultregistrationpage.cpp \
    ui/widgets/registration/ekasuiregistrationpage.cpp \
    ui/widgets/selectors/ekasui/ekasuidirectionselectionwidget.cpp \
    ui/widgets/selectors/ekasui/ekasuitracknumberselectionwidget.cpp \
    ui/widgets/selectors/ekasui/ekasuistationselectionwidget.cpp \
    ui/widgets/selectors/switchoperatorwidget.cpp \
    ui/widgets/selectors/ekasui/ekasuidefectselectionwidget.cpp \
    ui/widgets/defect/defectpage.cpp \
    ui/widgets/defect/ekasuidefectpage.cpp \
    ui/widgets/defect/scannerdefectregistrationpage.cpp \
    ui/widgets/defect/ekasuiscannerdefectregistrationpage.cpp \
    ui/widgets/indicatorbluetooth.cpp \
    ui/widgets/defect/defaultdefectpage.cpp \
    ui/widgets/defect/defaultscannerdefectregistrationpage.cpp \
    ui/widgets/options/deviceoptions.cpp \
    ui/widgets/misc/batteryplot.cpp \
    ui/widgets/options/batteryoptions.cpp \
    ui/widgets/misc/popuppasswordform.cpp \
    ui/widgets/permissionspage.cpp \
    ui/widgets/passwordmanagerpage.cpp \
    ui/widgets/bscanmovementcontrols.cpp \
    ui/widgets/trackmarks/trackmarksrussian.cpp \
    ui/widgets/trackmarks/trackmarksrussianselector.cpp \
    ui/widgets/bottomsignalampl.cpp \
    ui/widgets/trackmarks/trackmarksmetricselector.cpp \
    ui/widgets/trackmarks/trackmarksmetric1km.cpp \
    ui/widgets/colors.cpp \
    ui/widgets/testexternalkeyboardwidget.cpp \
    ui/widgets/selectors/notificationselectionwidget.cpp \
    ui/widgets/selectors/genericselectionwidget.cpp \
    core/notifications/defaultnotificationmanager.cpp \
    ui/widgets/options/notificationoptions.cpp \
    ui/widgets/options/keycombinationsoptions.cpp \
    ui/widgets/memoryimportpage.cpp \
    ui/widgets/memoryusagewidget.cpp \
    core/videocontroller/videoplayercontroller.cpp \
    ui/widgets/options/pathencoderemulatoroptions.cpp \
    ui/widgets/switchtypepage.cpp

INCLUDEPATH += \
    ui/widgets \
    ui/widgets/internationalization \
    ui/widgets/ascanplot \
    ui/widgets/mscanplot \
    ui/widgets/spinboxes \
    ui/widgets/controlsarea \
    ui/widgets/lateralpanels \
    ui/widgets/styles \
    ui/widgets/options \
    ui/widgets/selectors \
    ui/widgets/misc \
    ui/widgets/customLabel \
    core \
    core/audio \
    core/screen \
    core/externalkeyboard \
    core/models \
    core/trackmarks

HEADERS += \
    consts.h \
    debug.h \
    core/colorschememanager.h \
    core/colorscheme.h \
    core/appdatapath.h \
    core/audio/alarmtone.h \
    core/audio/alarmtone_p.h \
    core/audio/audiogenerator.h \
    core/audio/audiooutput.h \
    core/audio/sounddata.h \
    core/bluetooth/bluetoothmanager.h \
    core/channelscontroller.h \
    core/checkcalibration.h \
    core/closure.h \
    core/coredefinitions.h \
    ui/widgets/customLabel/customqlabel.h \
    core/core.h \
    core/defcore.h \
    core/defectmarker.h \
    core/direction.h \
    core/externalkeyboard/externalkeyboard.h \
    core/ftp/ftpclient.h \
    core/ftp/ftpuploader.h \
    core/limitsforsens.h \
    core/models/channelsmodel.h \
    core/models/channelsproxyfiltermodel.h \
    core/models/channelsproxytablemodel.h \
    core/models/modeltypes.h \
    core/models/roles.h \
    core/models/tapemodel.h \
    core/passwordmanager.h \
    core/registration.h \
    core/screen/screen.h \
    core/settings.h \
    core/temperature/fake/temperature_fake.h \
    core/temperature/generic/genericTempSensor.h \
    core/temperature/ds1820/ds18s20.h \
    core/temperature/lm75/lm75.h \
    core/temperature/temperaturesensor.h \
    core/temperature/temperaturesensormanager.h \
    core/trackmarks/tmmetric1km.h \
    core/trackmarks/tmmetric.h \
    core/trackmarks/tmrussian.h \
    core/trackmarks/trackmarks.h \
    core/remotecontrol/remotecontrol.h \
    core/filemanager.h \
    ui/widgets/accessories.h \
    ui/widgets/ascanpage.h \
    ui/widgets/ascanplot/ascanplot.h \
    ui/widgets/ascanplot/gateintervalsymbol.h \
    ui/widgets/batterywidget.h \
    ui/widgets/bluetooth/bluetoothmanagerpage.h \
    ui/widgets/bottomsignalampl.h \
    ui/widgets/bscangates.h \
    ui/widgets/bscanmovementcontrols.h \
    ui/widgets/bscanpage.h \
    ui/widgets/bscanplot.h \
    ui/widgets/bscantape.h \
    ui/widgets/clock.h \
    ui/widgets/colors.h \
    ui/widgets/controlsarea/controlsarea.h \
    ui/widgets/controlsarea/controlswidget.h \
    ui/widgets/defect/defaultdefectpage.h \
    ui/widgets/defect/defaultscannerdefectregistrationpage.h \
    ui/widgets/defect/defectpage.h \
    ui/widgets/defect/ekasuidefectpage.h \
    ui/widgets/defect/ekasuiscannerdefectregistrationpage.h \
    ui/widgets/defect/scannerdefectregistrationpage.h \
    ui/widgets/electrictestpage.h \
    ui/widgets/encoderpage.h \
    ui/widgets/handchannelssettingsitem.h \
    ui/widgets/handchannelssettingsview.h \
    ui/widgets/imageviewerpage.h \
    ui/widgets/indicatorbluetooth.h \
    ui/widgets/indicatorwifi.h \
    ui/widgets/internationalization/languageswitcher.h \
    ui/widgets/lateralpanels/handlateralbuttonsview.h \
    ui/widgets/lateralpanels/handlateralpanelitem.h \
    ui/widgets/lateralpanels/handlateralpanelview.h \
    ui/widgets/lateralpanels/lateralbuttonitem.h \
    ui/widgets/lateralpanels/lateralpanelfsm.h \
    ui/widgets/lateralpanels/scanlateralbuttonitem.h \
    ui/widgets/lateralpanels/scanlateralbuttonsview.h \
    ui/widgets/lateralpanels/scanlateralpanel.h \
    ui/widgets/lateralpanels/scanlateralpanelitem.h \
    ui/widgets/lateralpanels/scanlateralpaneltapeitem.h \
    ui/widgets/lateralpanels/scanlateralpanelview.h \
    ui/widgets/lateralpanels/scantapeacousticcontactitem.h \
    ui/widgets/lateralpanels/scantapeacousticcontactview.h \
    ui/widgets/mainwindow.h \
    ui/widgets/memorypage.h \
    ui/widgets/messagepage.h \
    ui/widgets/misc/batteryplot.h \
    ui/widgets/misc/popupdialogwidget.h \
    ui/widgets/misc/popuppasswordform.h \
    ui/widgets/mscanpage.h \
    ui/widgets/mscanplot/mscanplot.h \
    ui/widgets/note.h \
    ui/widgets/notifier.h \
    ui/widgets/options/batteryoptions.h \
    ui/widgets/options/boltjointoptions.h \
    ui/widgets/options/deviceoptions.h \
    ui/widgets/options/ekasuioptions.h \
    ui/widgets/options/externaltemperatureoptions.h \
    ui/widgets/options/screenheateroptions.h \
    ui/widgets/options/sensautoreset.h \
    ui/widgets/options/optionslistpage.h \
    ui/widgets/passwordmanagerpage.h \
    ui/widgets/permissionspage.h \
    ui/widgets/playlistpage.h \
    ui/widgets/pushbuttonwithlongpress/pushbuttonwithlongpress.h \
    ui/widgets/quickbar.h \
    ui/widgets/registration/defaultregistrationpage.h \
    ui/widgets/registration/ekasuiregistrationpage.h \
    ui/widgets/registration/registrationpage.h \
    ui/widgets/report.h \
    ui/widgets/satellitecoordinatespage.h \
    ui/widgets/satellites.h \
    ui/widgets/scannerencoderspage.h \
    ui/widgets/screenshotsserviceview.h \
    ui/widgets/selectors/ekasui/ekasuidefectselectionwidget.h \
    ui/widgets/selectors/ekasui/ekasuidirectionselectionwidget.h \
    ui/widgets/selectors/ekasui/ekasuistationselectionwidget.h \
    ui/widgets/selectors/ekasui/ekasuitracknumberselectionwidget.h \
    ui/widgets/selectors/lineselectionwidget.h \
    ui/widgets/selectors/operatorselectionwidget.h \
    ui/widgets/selectors/switchoperatorwidget.h \
    ui/widgets/selectors/fileplacementwidget.h \
    ui/widgets/setdate.h \
    ui/widgets/settime.h \
    ui/widgets/soundsettings.h \
    ui/widgets/spinboxes/limitsforsensspinboxnumber.h \
    ui/widgets/spinboxes/spinboxbase.h \
    ui/widgets/spinboxes/spinboxcolor.h \
    ui/widgets/spinboxes/spinboxlabel.h \
    ui/widgets/spinboxes/spinboxlist.h \
    ui/widgets/spinboxes/spinboxnumber.h \
    ui/widgets/splashscreen.h \
    ui/widgets/statusbar.h \
    ui/widgets/styles/styles.h \
    ui/widgets/systempasswordform.h \
    ui/widgets/tablesettingsitem.h \
    ui/widgets/tablesettingsview.h \
    ui/widgets/temperature.h \
    ui/widgets/trackmarks/trackmarksmetric1km.h \
    ui/widgets/trackmarks/trackmarksmetricselector.h \
    ui/widgets/trackmarks/trackmarkspage.h \
    ui/widgets/trackmarks/trackmarksrussian.h \
    ui/widgets/trackmarks/trackmarksrussianselector.h \
    ui/widgets/trackmarks/trackmarksselector.h \
    ui/widgets/waitmessagepage.h \
    ui/widgets/wrongcalibrationmsg.h \
    ui/widgets/testexternalkeyboardwidget.h \
    ui/widgets/selectors/notificationselectionwidget.h \
    ui/widgets/selectors/genericselectionwidget.h \
    core/notifications/notificationmanager.h \
    core/notifications/defaultnotificationmanager.h \
    ui/widgets/options/notificationoptions.h \
    ui/widgets/options/keycombinationsoptions.h \
    ui/widgets/memoryimportpage.h \
    ui/widgets/memoryusagewidget.h \
    core/videocontroller/videoplayercontroller.h \
    core/videocontroller/videocontrolprotocol.h \
    ui/widgets/options/pathencoderemulatoroptions.h \
    ui/widgets/switchtypepage.h

FORMS += \
    ui/widgets/notifier.ui \
    ui/widgets/note.ui\
    ui/widgets/mainwindow.ui \
    ui/widgets/setdate.ui \
    ui/widgets/settime.ui \
    ui/widgets/temperature.ui \
    ui/widgets/soundsettings.ui \
    ui/widgets/satellitecoordinatespage.ui \
    ui/widgets/satellites.ui \
    ui/widgets/ascanpage.ui \
    ui/widgets/bscanpage.ui \
    ui/widgets/mscanpage.ui \
    ui/widgets/spinboxes/spinboxbase.ui \
    ui/widgets/quickbar.ui \
    ui/widgets/report.ui \
    ui/widgets/controlsarea/controlsarea.ui \
    ui/widgets/lateralpanels/scanlateralpanelitem.ui \
    ui/widgets/lateralpanels/scanlateralpaneltapeitem.ui \
    ui/widgets/lateralpanels/handlateralpanelitem.ui \
    ui/widgets/lateralpanels/handlateralpanelview.ui \
    ui/widgets/lateralpanels/scantapeacousticcontactitem.ui \
    ui/widgets/tablesettingsview.ui \
    ui/widgets/tablesettingsitem.ui \
    ui/widgets/bscantape.ui \
    ui/widgets/messagepage.ui \
    ui/widgets/handchannelssettingsview.ui \
    ui/widgets/handchannelssettingsitem.ui \
    ui/widgets/encoderpage.ui \
    ui/widgets/memorypage.ui \
    ui/widgets/indicatorwifi.ui \
    ui/widgets/imageviewerpage.ui \
    ui/widgets/screenshotsserviceview.ui \
    ui/widgets/scannerencoderspage.ui \
    ui/widgets/wrongcalibrationmsg.ui \
    ui/widgets/waitmessagepage.ui \
    ui/widgets/electrictestpage.ui \
    ui/widgets/bscanmovementcontrols.ui \
    ui/widgets/options/externaltemperatureoptions.ui \
    ui/widgets/options/screenheateroptions.ui \
    ui/widgets/options/boltjointoptions.ui \
    ui/widgets/options/sensautoreset.ui \
    ui/widgets/systempasswordform.ui \
    ui/widgets/options/ekasuioptions.ui \
    ui/widgets/options/optionslistpage.ui \
    ui/widgets/bluetooth/bluetoothmanagerpage.ui \
    ui/widgets/selectors/operatorselectionwidget.ui \
    ui/widgets/misc/popupdialogwidget.ui \
    ui/widgets/selectors/lineselectionwidget.ui  \
    ui/widgets/selectors/fileplacementwidget.ui  \
    ui/widgets/registration/defaultregistrationpage.ui \
    ui/widgets/registration/ekasuiregistrationpage.ui \
    ui/widgets/selectors/ekasui/ekasuidirectionselectionwidget.ui \
    ui/widgets/selectors/ekasui/ekasuitracknumberselectionwidget.ui \
    ui/widgets/selectors/ekasui/ekasuistationselectionwidget.ui \
    ui/widgets/selectors/switchoperatorwidget.ui \
    ui/widgets/selectors/ekasui/ekasuidefectselectionwidget.ui \
    ui/widgets/defect/ekasuidefectpage.ui \
    ui/widgets/defect/ekasuiscannerdefectregistrationpage.ui \
    ui/widgets/indicatorbluetooth.ui \
    ui/widgets/defect/defaultdefectpage.ui \
    ui/widgets/defect/defaultscannerdefectregistrationpage.ui \
    ui/widgets/options/deviceoptions.ui \
    ui/widgets/misc/batteryplot.ui \
    ui/widgets/options/batteryoptions.ui \
    ui/widgets/misc/popuppasswordform.ui \
    ui/widgets/permissionspage.ui \
    ui/widgets/passwordmanagerpage.ui \
    ui/widgets/batterywidget.ui \
    ui/widgets/testexternalkeyboardwidget.ui \
    ui/widgets/selectors/notificationselectionwidget.ui \
    ui/widgets/selectors/genericselectionwidget.ui \
    ui/widgets/options/notificationoptions.ui \
    ui/widgets/options/keycombinationsoptions.ui \
    ui/widgets/memoryimportpage.ui \
    ui/widgets/memoryusagewidget.ui \
    ui/widgets/options/pathencoderemulatoroptions.ui \
    ui/widgets/switchtypepage.ui

# defcore
INCLUDEPATH += defcore defcore/datacontainer

avicon15 {
    SOURCES += \
        defcore/DeviceConfig_Av15_usbcan.cpp \
        defcore/sockets/socket_can_fake.cpp
    HEADERS += \
        defcore/DeviceConfig_Av15_usbcan.h
}

avicon15:android {
    message("avicon15_for_android")
    SOURCES += \
        defcore/sockets/socket_usbcan_and.cpp \
        defcore/datatransfers/datatransfer_usbcan_and.cpp
    HEADERS += \
        defcore/sockets/socket_usbcan_and.h \
        defcore/datatransfers/datatransfer_usbcan_and.h
}

avicondbhs {
    SOURCES += \
        defcore/DeviceConfig_DB_can.cpp \
        defcore/sockets/socket_can.cpp
    HEADERS += \
        defcore/DeviceConfig_DB_can.h \
        defcore/sockets/socket_can.h
}

avicondb {
SOURCES += \
        defcore/DeviceConfig_DB_lan.cpp \
        defcore/sockets/socket_can_fake.cpp
    HEADERS += \
        defcore/DeviceConfig_DB_lan.h
}

avicon31 {
    SOURCES += \
        defcore/DeviceConfig_Avk31.cpp \
        defcore/sockets/socket_can_fake.cpp \
        defcore/DeviceConfig_HeadScan.cpp
    HEADERS += \
        defcore/DeviceConfig_Avk31.h \
        defcore/DeviceConfig_HeadScan.h
}

avicon31e {
    SOURCES += \
        defcore/DeviceConfig_Avk31E.cpp \
        defcore/sockets/socket_can_fake.cpp
    HEADERS += \
        defcore/DeviceConfig_Avk31E.h
}

SOURCES += \
    defcore/CriticalSection_Lin.cpp \
    defcore/EventManager_Qt.cpp \
    defcore/LinThread.cpp \
    defcore/ThreadClassList_Lin.cpp \
    defcore/ChannelsTable.cpp \
    defcore/Device.cpp \
    defcore/DeviceCalibration.cpp \
    defcore/platforms.cpp \
    defcore/ThreadClassList.cpp \
    defcore/datacontainer/DataContainer.cpp \
    defcore/datacontainer/dc_functions.cpp \
    defcore/TickCount.cpp \
    defcore/DeviceConfig.cpp \
    defcore/sockets/isocket.cpp \
    defcore/sockets/socket_lan.cpp \
    defcore/datatransfers/idatatransfer.cpp \
    defcore/datatransfers/datatransfer_lan.cpp \
    defcore/datatransfers/datatransfer_can.cpp \
    defcore/prot_umu/iumu.cpp \
    defcore/prot_umu/prot_umu_lan.cpp \
    defcore/prot_umu/prot_umu_rawcan.cpp \
    defcore/prot_umu/prot_umu_padcan.cpp \
    defcore/prot_umu/prot_umu_usbcan_and.cpp \
    defcore/ekasui/EK_ASUIdata.cpp \
    defcore/datacontainer/filereader/FileReader.cpp


HEADERS += \
    defcore/CriticalSection_Lin.h \
    defcore/EventManager_Qt.h \
    defcore/LinThread.h \
    defcore/ThreadClassList_Lin.h \
    defcore/ChannelsTable.h \
    defcore/CriticalSection.h \
    defcore/Definitions.h \
    defcore/Device.h \
    defcore/DeviceCalibration.h \
    defcore/DeviceCalibration_definitions.h \
    defcore/DeviceConfig.h \
    defcore/EventManager.h \
    defcore/platforms.h \
    defcore/systemdetection.h \
    defcore/ThreadClassList.h \
    defcore/datacontainer/DataContainer.h \
    defcore/datacontainer/dc_functions.h \
    defcore/datacontainer/dc_definitions.h \
    defcore/datacontainer/regar_interface.h \
    defcore/TickCount.h \
    defcore/sockets/isocket.h \
    defcore/sockets/socket_lan.h \
    defcore/sockets/socket_can.h \
    defcore/datatransfers/idatatransfer.h \
    defcore/datatransfers/datatransfer_lan.h \
    defcore/datatransfers/datatransfer_can.h \
    defcore/prot_umu/utils_can.h \
    defcore/prot_umu/iumu.h \
    defcore/prot_umu/prot_umu_lan.h \
    defcore/prot_umu/prot_umu_lan_constants.h \
    defcore/prot_umu/prot_umu_rawcan.h \
    defcore/prot_umu/prot_umu_padcan.h \
    defcore/prot_umu/prot_umu_can_constants.h \
    defcore/prot_umu/prot_umu_usbcan_and.h \
    defcore/ekasui/EK_ASUIdata.h \
    defcore/datacontainer/filereader/FileReader.h

win32 {
    LIBS += -lws2_32
}

RESOURCES += \
    ui/resources/resources.qrc \
    ui/widgets/internationalization/internationalization.qrc \
    ui/resources/qml.qrc

avicondb {
    RESOURCES += ui/resources/schemes_db.qrc
} else:avicon31 {
    RESOURCES += ui/resources/schemes_avicon31.qrc
} else:avicon15 {
    message("AVICON15 RESOURCES")
    RESOURCES += ui/resources/schemes_avicon15.qrc
}

TRANSLATIONS = \
    ui/widgets/internationalization/russian.ts

android {
        QT = quick $$QT printsupport concurrent androidextras

        NEW_BUM_DRIVER_PATH = $$(NAUTIZ_BUM_DRIVER_PATH)
        DEVICE_TARGET = Android
        INCLUDEPATH += $$NEW_BUM_DRIVER_PATH/BumDriver
        CONFIG(release, debug|release): BUM_DRIVER_LIB = \
                                $$NEW_BUM_DRIVER_PATH/build-BumDriver-$${DEVICE_TARGET}-Release/libBumDriver.a
        CONFIG(debug, debug|release): BUM_DRIVER_LIB = \
                                $$NEW_BUM_DRIVER_PATH/build-BumDriver-$${DEVICE_TARGET}-Debug/libBumDriver.a
        message("BUM_DRIVER_LIB:"$$BUM_DRIVER_LIB)
        LIBS += $$BUM_DRIVER_LIB
        PRE_TARGETDEPS += $$BUM_DRIVER_LIB

        DISTFILES += \
            android/AndroidManifest.xml \
            android/gradle/wrapper/gradle-wrapper.jar \
            android/gradlew \
            android/res/values/libs.xml \
            android/build.gradle \
            android/gradle/wrapper/gradle-wrapper.properties \
            android/gradlew.bat \
            android/src/com/radioavionica/avicon15/System.java \
            android/src/com/radioavionica/avicon15/WifiAccess.java \
            android/src/com/radioavionica/avicon15/PositionClass.java \
            android/src/com/radioavionica/avicon15/BluetoothClass.java

        ANDROID_PACKAGE_SOURCE_DIR = $$PWD/android
        OTHER_FILES += android/src/com/mycompanyname/myappname/System.java
}

DISTFILES += \
    android/src/com/radioavionica/avicon15/CameraClass.java
