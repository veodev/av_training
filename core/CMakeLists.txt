find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5Positioning REQUIRED)
find_package(Qt5Sensors REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5SerialPort REQUIRED)
find_package(Qt5Bluetooth REQUIRED)

set(
    SOURCES
    core.cpp
    defcore.cpp
    registration.cpp
    settings.cpp
    checkcalibration.cpp
    limitsforsens.cpp
    audio/audiooutput.cpp
    audio/audiogenerator.cpp
    audio/alarmtone.cpp
    audio/alarmtone_p.cpp
    models/channelsmodel.cpp
    models/channelsproxyfiltermodel.cpp
    models/channelsproxytablemodel.cpp
    models/tapemodel.cpp
    appdatapath.cpp
    direction.cpp
    defectmarker.cpp
    ftp/ftpuploader.cpp
    temperature/fake/temperature_fake.cpp
    temperature/generic/genericTempSensor.cpp
    temperature/lm75/lm75.cpp
    temperature/ds1820/ds18s20.cpp
    temperature/temperaturesensormanager.cpp
    powermanagement/battery.cpp
    channelscontroller.cpp
    audio/sounddata.cpp
    passwordmanager.cpp
    trackmarks/tmrussian.cpp
    trackmarks/trackmarks.cpp
    trackmarks/tmmetric.cpp
    trackmarks/tmmetric1km.cpp
    notifications/defaultnotificationmanager.cpp
    powermanagement/powermanagement.cpp
    geoposition/locationutils.cpp
    geoposition/nmeareader.cpp
    geoposition/nmeapositioninfosource.cpp
    geoposition/nmeasatelliteinfosource.cpp
    geoposition/geoposition.cpp
    gpio/gpio.cpp
    remotecontrol/remotecontrol.cpp
    colorschememanager.cpp
    colorscheme.cpp
    filemanager.cpp
    videocontroller/videoplayercontroller.cpp
    )
set(HEADERS
    appdatapath.h
    audio/alarmtone.h
    audio/alarmtone_p.h
    audio/audiogenerator.h
    audio/audiooutput.h
    audio/sounddata.h
    bluetooth/bluetoothmanager.h
    channelscontroller.h
    checkcalibration.h
    closure.h
    coredefinitions.h
    core.h
    defcore.h
    defectmarker.h
    direction.h
    externalkeyboard/externalkeyboard.h
    ftp/ftpuploader.h
    limitsforsens.h
    models/channelsmodel.h
    models/channelsproxyfiltermodel.h
    models/channelsproxytablemodel.h
    models/modeltypes.h
    models/roles.h
    models/tapemodel.h
    passwordmanager.h
    registration.h
    screen/screen.h
    settings.h
    temperature/fake/temperature_fake.h
    temperature/generic/genericTempSensor.h
    temperature/temperaturesensor.h
    temperature/ds1820/ds18s20.h
    temperature/lm75/lm75.h
    temperature/temperaturesensormanager.h
    trackmarks/tmmetric1km.h
    trackmarks/tmmetric.h
    trackmarks/tmrussian.h
    trackmarks/trackmarks.h
    powermanagement/battery.h
    powermanagement/powermanagement.h
    geoposition/geoposition.h
    geoposition/locationutils.h
    geoposition/nmeareader.h
    geoposition/nmeapositioninfosource.h
    geoposition/nmeasatelliteinfosource.h
    remotecontrol/remotecontrol.h
    colorschememanager.h
    colorscheme.h
    filemanager.h
    videocontroller/videoplayercontroller.h
    videocontroller/videocontrolprotocol.h
    )

#Wifi Core
if(${TARGET_SELECTED_ARCH} STREQUAL "buildroot-imx" )
    set(SOURCES ${SOURCES} wifimanager/wifimanager.cpp)
    set(HEADERS ${HEADERS} wifimanager/wifimanager.h)
endif()

#Platform specific stuff
if(${TARGET_SELECTED_ARCH} STREQUAL "buildroot-imx" )
    message(STATUS "Configured for buildroot-imx")
    set(SOURCES ${SOURCES} screen/brightness_utsvu.cpp
        screen/screen_utsvu.cpp
        bluetooth/bluetoothmanagerqt.cpp
        externalkeyboard/externalkeyboard.cpp
        mxccamera/mxccamera.cpp
        mxccamera/mxccameraimagecapture.cpp
        mxccamera/mxcmediarecorder.cpp
        powermanagement/utsvupowermanagement.cpp)
    set(HEADERS ${HEADERS} bluetooth/bluetoothmanagerqt.h
        mxccamera/mxccamera.h
        mxccamera/mxccameraimagecapture.h
        mxccamera/mxcmediarecorder.h
        powermanagement/utsvupowermanagement.h)
    set(SOURCES_DB ${SOURCES})
    set(HEADERS_DB ${HEADERS})
    add_definitions(-DUSE_UTSVU_POWERMANAGEMENT)
elseif(${TARGET_SELECTED_ARCH} STREQUAL "android-nautiz-x8")
    message(STATUS "Configured for android-nautiz-x8")
    set(SOURCES ${SOURCES} screen/brightness_fake.cpp
        screen/screen_fake.cpp
        bluetooth/fakebluetoothmanager.cpp
        externalkeyboard/externalkeyboardfake.cpp)
    set(SOURCES ${SOURCES} core/androidJNI.cpp)
    set(HEADERS ${HEADERS} core/androidJNI.h)
    set(SOURCES_DB ${SOURCES})
    set(HEADERS_DB ${HEADERS})
else()
    message(STATUS "Configured for desktop / pkk")
    set(SOURCES_DB ${SOURCES} screen/brightness_ppk.cpp
        screen/screen_fake.cpp
        bluetooth/fakebluetoothmanager.cpp
        externalkeyboard/externalkeyboardfake.cpp)
    set(HEADERS_DB ${HEADERS} bluetooth/fakebluetoothmanager.cpp)
    set(SOURCES ${SOURCES} screen/brightness_fake.cpp
        screen/screen_fake.cpp
        bluetooth/fakebluetoothmanager.cpp
        externalkeyboard/externalkeyboardfake.cpp)
    set(HEADERS ${HEADERS} bluetooth/fakebluetoothmanager.cpp )
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/defcore)
include_directories(${CMAKE_SOURCE_DIR}/defcore/datacontainer)
include_directories(${CMAKE_SOURCE_DIR}/core)
include_directories(${CMAKE_SOURCE_DIR}/core/models)
include_directories(${CMAKE_SOURCE_DIR}/core/trackmarks)
include_directories(${CMAKE_SOURCE_DIR}/core/audio)
include_directories(${CMAKE_SOURCE_DIR}/core/screen)
include_directories(${CMAKE_SOURCE_DIR}/core/gpio)
include_directories(${CMAKE_SOURCE_DIR}/core/notifications)
include_directories(${CMAKE_SOURCE_DIR}/core/geoposition)
include_directories(${CMAKE_SOURCE_DIR}/core/videocontroller)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets)

add_library(core_31 ${SOURCES} ${HEADERS})
target_compile_definitions(core_31 PUBLIC -DTARGET_AVICON31)
target_link_libraries(core_31 defcore_31 Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::Network Qt5::Positioning Qt5::Sensors Qt5::Xml Qt5::Sql Qt5::SerialPort Qt5::Bluetooth dl)

add_library(core_db ${SOURCES_DB} ${HEADERS_DB})
target_compile_definitions(core_db PUBLIC -DTARGET_AVICONDB)
target_link_libraries(core_db defcore_db Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::Network Qt5::Positioning Qt5::Sensors Qt5::Xml Qt5::Sql Qt5::SerialPort Qt5::Bluetooth dl)

add_library(core_dbhs ${SOURCES_DB} ${HEADERS_DB})
target_compile_definitions(core_dbhs PUBLIC -DTARGET_AVICONDBHS)
target_link_libraries(core_dbhs defcore_dbhs Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::Network Qt5::Positioning Qt5::Sensors Qt5::Xml Qt5::Sql Qt5::SerialPort Qt5::Bluetooth dl)

add_library(core_15 ${SOURCES} ${HEADERS})
target_compile_definitions(core_15 PUBLIC -DTARGET_AVICON15)
target_link_libraries(core_15 defcore_15 Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::Network Qt5::Positioning Qt5::Sensors Qt5::Xml Qt5::Sql Qt5::SerialPort Qt5::Bluetooth dl)

add_library(core_31e ${SOURCES} ${HEADERS})
target_compile_definitions(core_31e PUBLIC -DTARGET_AVICON31E)
target_link_libraries(core_31e defcore_31e Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::Network Qt5::Positioning Qt5::Sensors Qt5::Xml Qt5::Sql Qt5::SerialPort Qt5::Bluetooth dl)


