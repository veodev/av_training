find_package(Qt5Core REQUIRED)
find_package(Qt5Xml REQUIRED)

set(
    SOURCES
    CriticalSection_Lin.cpp
    EventManager_Qt.cpp
    LinThread.cpp
    ThreadClassList_Lin.cpp
    ChannelsTable.cpp
    Device.cpp
    DeviceCalibration.cpp
    platforms.cpp
    ThreadClassList.cpp
    datacontainer/DataContainer.cpp
    datacontainer/dc_functions.cpp
    TickCount.cpp
    DeviceConfig.cpp
    sockets/isocket.cpp
    sockets/socket_lan.cpp
    datatransfers/idatatransfer.cpp
    datatransfers/datatransfer_lan.cpp
    datatransfers/datatransfer_can.cpp
    prot_umu/iumu.cpp
    prot_umu/prot_umu_lan.cpp
    prot_umu/prot_umu_rawcan.cpp
    prot_umu/prot_umu_padcan.cpp
    prot_umu/prot_umu_usbcan_and.cpp
    ekasui/EK_ASUIdata.cpp
    datacontainer/filereader/FileReader.cpp
    )
set(HEADERS
    CriticalSection_Lin.h
    EventManager_Qt.h
    LinThread.h
    ThreadClassList_Lin.h
    ChannelsTable.h
    ChannelsIds.h
    CriticalSection.h
    Definitions.h
    Device.h
    Device_definitions.h
    DeviceCalibration.h
    DeviceCalibration_definitions.h
    DeviceConfig.h
    EventManager.h
    platforms.h
    systemdetection.h
    ThreadClassList.h
    datacontainer/DataContainer.h
    datacontainer/regar_interface.h
    datacontainer/dc_definitions.h
    datacontainer/dc_functions.h
    TickCount.h
    sockets/isocket.h
    sockets/socket_lan.h
    sockets/socket_can.h
    datatransfers/idatatransfer.h
    datatransfers/datatransfer_lan.h
    datatransfers/datatransfer_can.h
    prot_umu/utils_can.h
    prot_umu/iumu.h
    prot_umu/prot_umu_lan.h
    prot_umu/prot_umu_lan_constants.h
    prot_umu/prot_umu_rawcan.h
    prot_umu/prot_umu_padcan.h
    prot_umu/prot_umu_can_constants.h
    prot_umu/prot_umu_usbcan_and.h
    ekasui/EK_ASUIdata.h
    datacontainer/filereader/FileReader.h
    )

set(SOURCES_31 ${SOURCES}
    DeviceConfig_Avk31.cpp
    sockets/socket_can_fake.cpp
    DeviceConfig_HeadScan.cpp)
set(HEADERS_31 ${HEADERS}
    DeviceConfig_Avk31.h
    DeviceConfig_HeadScan.h)

set(SOURCES_DB ${SOURCES}
    DeviceConfig_DB_lan.cpp
    sockets/socket_can_fake.cpp
)
set(HEADERS_DB ${HEADERS}
    DeviceConfig_DB_lan.h
)

set(SOURCES_DBHS ${SOURCES}
    DeviceConfig_DB_can.cpp
    sockets/socket_can.cpp
)
set(HEADERS_DBHS ${HEADERS}
    DeviceConfig_DB_can.h
    sockets/socket_can.h
)

set(SOURCES_31E ${SOURCES}
    DeviceConfig_Avk31E.cpp
    sockets/socket_can_fake.cpp
    DeviceConfig_HeadScan.cpp)
set(HEADERS_31E ${HEADERS}
    DeviceConfig_Avk31E.h
    DeviceConfig_HeadScan.h)

if(${TARGET_SELECTED_ARCH} STREQUAL "android-nautiz-x8")
    set(SOURCES_15 ${SOURCES}
        sockets/socket_usbcan_and.cpp
        datatransfers/datatransfer_usbcan_and.cpp
    )
    set(HEADERS_15 ${HEADERS}
        sockets/socket_usbcan_and.h
        datatransfers/datatransfer_usbcan_and.h
    )
else()
    set(SOURCES_15 ${SOURCES}
        DeviceConfig_Av15_usbcan.cpp
        sockets/socket_can_fake.cpp
    )
    set(HEADERS_15 ${HEADERS}
        DeviceConfig_Av15_usbcan.h
    )
endif()

include_directories(${CMAKE_SOURCE_DIR})
message(STATUS "DEFCORE BUILD TYPE:" ${CMAKE_BUILD_TYPE})
message(STATUS "DEFCORE CXX_FLAGS:" ${CMAKE_CXX_FLAGS})
message(STATUS "DEFCORE DEBUG CXX_FLAGS:" ${CMAKE_CXX_FLAGS_DEBUG})
message(STATUS "DEFCORE RELEASE CXX_FLAGS:" ${CMAKE_CXX_FLAGS_RELEASE})

#TARGET defcore for Avicon-31
add_library(defcore_31 ${SOURCES_31} ${HEADERS_31})
target_compile_definitions(defcore_31 PUBLIC -DTARGET_AVICON31)
target_link_libraries(defcore_31 core_31 Qt5::Core Qt5::Xml pthread dl)

#TARGET defcore for Avicon-DB
add_library(defcore_db ${SOURCES_DB} ${HEADERS_DB})
target_compile_definitions(defcore_db PUBLIC -DTARGET_AVICONDB)
target_link_libraries(defcore_db core_db Qt5::Core Qt5::Xml pthread dl)

#TARGET defcore for Avicon-DBHS
add_library(defcore_dbhs ${SOURCES_DBHS} ${HEADERS_DBHS})
target_compile_definitions(defcore_dbhs PUBLIC -DTARGET_AVICONDBHS)
target_link_libraries(defcore_dbhs core_dbhs Qt5::Core Qt5::Xml pthread dl)

#TARGET defcore for Avicon-15
add_library(defcore_15 ${SOURCES_15} ${HEADERS_15})
target_compile_definitions(defcore_15 PUBLIC -DTARGET_AVICON15)
target_link_libraries(defcore_15 core_15 Qt5::Core Qt5::Xml pthread dl)

#TARGET defcore for Avicon-31E
add_library(defcore_31e ${SOURCES_31E} ${HEADERS_31E})
target_compile_definitions(defcore_31e PUBLIC -DTARGET_AVICON31E)
target_link_libraries(defcore_31e core_31e Qt5::Core Qt5::Xml pthread dl)
