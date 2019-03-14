find_package(Qt5Core REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Multimedia REQUIRED)
find_package(Qt5MultimediaWidgets REQUIRED)
find_package(Qt5Positioning REQUIRED)
find_package(Qt5Svg REQUIRED)
find_package(Qt5Sql REQUIRED)
find_package(Qt5Xml REQUIRED)
find_package(qwt REQUIRED)

set(
    SOURCES
    av17/av17defview.cpp
    av17/av17page.cpp
    av17/av17plot.cpp
    av17/DataFileUnit.cpp
    av17/av17projections.cpp
    videocamerapage.cpp
    audiocomments/audioplayerpage/audioplayerpage.cpp
    audiocomments/audiorecorderpage/audiorecorderpage.cpp
    audiocomments/audiolevel.cpp
    video/videoplayer/videoplayerpage.cpp
    virtualkeyboards/virtualkeyboards.cpp
    virtualkeyboards/keypushbutton.cpp
    virtualkeyboards/stickypushbutton.cpp
    virtualkeyboards/symbolkeypushbutton.cpp
    virtualkeyboards/envirtualkeyboard/envirtualkeyboard.cpp
    virtualkeyboards/fnvirtualkeyboard/fnvirtualkeyboard.cpp
    virtualkeyboards/ruvirtualkeyboard/ruvirtualkeyboard.cpp
    virtualkeyboards/devirtualkeyboard/devirtualkeyboard.cpp
    accessories.cpp
    note.cpp
    clock.cpp
    setdate.cpp
    temperature.cpp
    soundsettings.cpp
    internationalization/languageswitcher.cpp
    ascanplot/ascanplot.cpp
    ascanplot/gateintervalsymbol.cpp
    bscangates.cpp
    electrictestpage.cpp
    mscanplot/mscanplot.cpp
    statusbar.cpp
    spinboxes/spinboxbase.cpp
    spinboxes/spinboxlist.cpp
    spinboxes/spinboxnumber.cpp
    spinboxes/spinboxlabel.cpp
    spinboxes/spinboxcolor.cpp
    spinboxes/limitsforsensspinboxnumber.cpp
    controlsarea/controlsarea.cpp
    controlsarea/controlswidget.cpp
    lateralpanels/lateralpanelfsm.cpp
    lateralpanels/handlateralpanelview.cpp
    lateralpanels/lateralbuttonitem.cpp
    lateralpanels/scanlateralbuttonitem.cpp
    lateralpanels/scanlateralbuttonsview.cpp
    lateralpanels/scantapeacousticcontactitem.cpp
    lateralpanels/scantapeacousticcontactview.cpp
    tablesettingsview.cpp
    tablesettingsitem.cpp
    styles/styles.cpp
    messagepage.cpp
    pushbuttonwithlongpress/pushbuttonwithlongpress.cpp
    handchannelssettingsview.cpp
    handchannelssettingsitem.cpp
    encoderpage.cpp
    indicatorwifi.cpp
    imageviewerpage.cpp
    screenshotsserviceview.cpp
    scannerencoderspage.cpp
    playlistpage.cpp
    report.cpp
    wrongcalibrationmsg.cpp
    waitmessagepage.cpp
    options/externaltemperatureoptions.cpp
    options/screenheateroptions.cpp
    options/boltjointoptions.cpp
    options/sensautoreset.cpp
    systempasswordform.cpp
    options/ekasuioptions.cpp
    lateralpanels/handlateralbuttonsview.cpp
    batterywidget.cpp
    satellitecoordinatespage.cpp
    settime.cpp
    notifier.cpp
    quickbar.cpp
    lateralpanels/handlateralpanelitem.cpp
    lateralpanels/scanlateralpanelitem.cpp
    lateralpanels/scanlateralpanelview.cpp
    lateralpanels/scanlateralpaneltapeitem.cpp
    satellites.cpp
    mscanpage.cpp
    splashscreen.cpp
    bscanplot.cpp
    bscantape.cpp
    bscanpage.cpp
    memorypage.cpp
    main.cpp
    ascanpage.cpp
    mainwindow.cpp
    bluetooth/bluetoothmanagerpage.cpp
    selectors/operatorselectionwidget.cpp
    misc/popupdialogwidget.cpp
    selectors/lineselectionwidget.cpp
    registration/registrationpage.cpp
    registration/defaultregistrationpage.cpp
    registration/ekasuiregistrationpage.cpp
    selectors/ekasui/ekasuidirectionselectionwidget.cpp
    selectors/ekasui/ekasuitracknumberselectionwidget.cpp
    selectors/ekasui/ekasuistationselectionwidget.cpp
    selectors/switchoperatorwidget.cpp
    selectors/ekasui/ekasuidefectselectionwidget.cpp
    defect/defectpage.cpp
    defect/ekasuidefectpage.cpp
    defect/scannerdefectregistrationpage.cpp
    defect/ekasuiscannerdefectregistrationpage.cpp
    indicatorbluetooth.cpp
    defect/defaultdefectpage.cpp
    defect/defaultscannerdefectregistrationpage.cpp
    options/deviceoptions.cpp
    misc/batteryplot.cpp
    options/batteryoptions.cpp
    misc/popuppasswordform.cpp
    permissionspage.cpp
    passwordmanagerpage.cpp
    bscanmovementcontrols.cpp
    trackmarks/trackmarksrussian.cpp
    trackmarks/trackmarksrussianselector.cpp
    bottomsignalampl.cpp
    trackmarks/trackmarksmetricselector.cpp
    trackmarks/trackmarksmetric1km.cpp
    colors.cpp
    testexternalkeyboardwidget.cpp
    selectors/notificationselectionwidget.cpp
    selectors/genericselectionwidget.cpp
    options/notificationoptions.cpp
    memoryimportpage.cpp
    selectors/fileplacementwidget.cpp
    options/keycombinationsoptions.cpp
    memoryusagewidget.cpp
    options/optionslistpage.cpp
    options/pathencoderemulatoroptions.cpp
    switchtypepage.cpp
    )
set(HEADERS

    av17/av17defview.h
    av17/av17page.h
    av17/av17plot.h
    av17/DataFileUnit.h
    av17/av17projections.h
    videocamerapage.h
    audiocomments/audioplayerpage/audioplayerpage.h
    audiocomments/audiorecorderpage/audiorecorderpage.h
    audiocomments/audiolevel.h
    video/videoplayer/videoplayerpage.h
    virtualkeyboards/virtualkeyboards.h
    virtualkeyboards/virtualkeyboardbase.h
    virtualkeyboards/keypushbutton.h
    virtualkeyboards/stickypushbutton.h
    virtualkeyboards/symbolkeypushbutton.h
    virtualkeyboards/envirtualkeyboard/envirtualkeyboard.h
    virtualkeyboards/fnvirtualkeyboard/fnvirtualkeyboard.h
    virtualkeyboards/ruvirtualkeyboard/ruvirtualkeyboard.h
    virtualkeyboards/devirtualkeyboard/devirtualkeyboard.h
    accessories.h
    ascanpage.h
    ascanplot/ascanplot.h
    ascanplot/gateintervalsymbol.h
    batterywidget.h
    bluetooth/bluetoothmanagerpage.h
    bottomsignalampl.h
    bscangates.h
    bscanmovementcontrols.h
    bscanpage.h
    bscanplot.h
    bscantape.h
    clock.h
    colors.h
    controlsarea/controlsarea.h
    controlsarea/controlswidget.h
    defect/defaultdefectpage.h
    defect/defaultscannerdefectregistrationpage.h
    defect/defectpage.h
    defect/ekasuidefectpage.h
    defect/ekasuiscannerdefectregistrationpage.h
    defect/scannerdefectregistrationpage.h
    electrictestpage.h
    encoderpage.h
    handchannelssettingsitem.h
    handchannelssettingsview.h
    imageviewerpage.h
    indicatorbluetooth.h
    indicatorwifi.h
    internationalization/languageswitcher.h
    lateralpanels/handlateralbuttonsview.h
    lateralpanels/handlateralpanelitem.h
    lateralpanels/handlateralpanelview.h
    lateralpanels/lateralbuttonitem.h
    lateralpanels/lateralpanelfsm.h
    lateralpanels/scanlateralbuttonitem.h
    lateralpanels/scanlateralbuttonsview.h
    lateralpanels/scanlateralpanel.h
    lateralpanels/scanlateralpanelitem.h
    lateralpanels/scanlateralpaneltapeitem.h
    lateralpanels/scanlateralpanelview.h
    lateralpanels/scantapeacousticcontactitem.h
    lateralpanels/scantapeacousticcontactview.h
    mainwindow.h
    memorypage.h
    messagepage.h
    misc/batteryplot.h
    misc/popupdialogwidget.h
    misc/popuppasswordform.h
    mscanpage.h
    mscanplot/mscanplot.h
    note.h
    notifier.h
    options/batteryoptions.h
    options/boltjointoptions.h
    options/deviceoptions.h
    options/ekasuioptions.h
    options/externaltemperatureoptions.h
    options/screenheateroptions.h
    options/sensautoreset.h
    passwordmanagerpage.h
    permissionspage.h
    playlistpage.h
    pushbuttonwithlongpress/pushbuttonwithlongpress.h
    quickbar.h
    registration/defaultregistrationpage.h
    registration/ekasuiregistrationpage.h
    registration/registrationpage.h
    report.h
    satellitecoordinatespage.h
    satellites.h
    scannerencoderspage.h
    screenshotsserviceview.h
    selectors/ekasui/ekasuidefectselectionwidget.h
    selectors/ekasui/ekasuidirectionselectionwidget.h
    selectors/ekasui/ekasuistationselectionwidget.h
    selectors/ekasui/ekasuitracknumberselectionwidget.h
    selectors/lineselectionwidget.h
    selectors/operatorselectionwidget.h
    selectors/switchoperatorwidget.h
    setdate.h
    settime.h
    soundsettings.h
    spinboxes/limitsforsensspinboxnumber.h
    spinboxes/spinboxbase.h
    spinboxes/spinboxcolor.h
    spinboxes/spinboxlabel.h
    spinboxes/spinboxlist.h
    spinboxes/spinboxnumber.h
    splashscreen.h
    statusbar.h
    styles/styles.h
    switchtypepage.h
    systempasswordform.h
    tablesettingsitem.h
    tablesettingsview.h
    temperature.h
    trackmarks/trackmarksmetric1km.h
    trackmarks/trackmarksmetricselector.h
    trackmarks/trackmarkspage.h
    trackmarks/trackmarksrussian.h
    trackmarks/trackmarksrussianselector.h
    trackmarks/trackmarksselector.h
    waitmessagepage.h
    wrongcalibrationmsg.h
    testexternalkeyboardwidget.h
    selectors/notificationselectionwidget.h
    selectors/genericselectionwidget.h
    options/notificationoptions.h
    memoryimportpage.h
    selectors/fileplacementwidget.h
    options/keycombinationsoptions.h
    memoryusagewidget.h
    options/optionslistpage.h
    options/pathencoderemulatoroptions.h
    switchtypepage.h
    )
set(FORMS
    av17/av17defview.ui
    av17/av17page.ui
    trackmarks/trackmarksrussian.ui
    trackmarks/trackmarksrussianselector.ui
    trackmarks/trackmarksmetricselector.ui
    trackmarks/trackmarksmetric1km.ui
    videocamerapage.ui
    audiocomments/audioplayerpage/audioplayerpage.ui
    audiocomments/audiorecorderpage/audiorecorderpage.ui
    video/videoplayer/videoplayerpage.ui
    virtualkeyboards/virtualkeyboards.ui
    virtualkeyboards/envirtualkeyboard/envirtualkeyboard.ui
    virtualkeyboards/fnvirtualkeyboard/fnvirtualkeyboard.ui
    virtualkeyboards/ruvirtualkeyboard/ruvirtualkeyboard.ui
    virtualkeyboards/devirtualkeyboard/devirtualkeyboard.ui
    notifier.ui
    note.ui
    mainwindow.ui
    setdate.ui
    settime.ui
    temperature.ui
    soundsettings.ui
    satellitecoordinatespage.ui
    satellites.ui
    ascanpage.ui
    bscanpage.ui
    mscanpage.ui
    spinboxes/spinboxbase.ui
    quickbar.ui
    report.ui
    controlsarea/controlsarea.ui
    lateralpanels/scanlateralpanelitem.ui
    lateralpanels/scanlateralpaneltapeitem.ui
    lateralpanels/handlateralpanelitem.ui
    lateralpanels/handlateralpanelview.ui
    lateralpanels/scantapeacousticcontactitem.ui
    tablesettingsview.ui
    tablesettingsitem.ui
    bscantape.ui
    messagepage.ui
    handchannelssettingsview.ui
    handchannelssettingsitem.ui
    encoderpage.ui
    memorypage.ui
    indicatorwifi.ui
    imageviewerpage.ui
    screenshotsserviceview.ui
    scannerencoderspage.ui
    wrongcalibrationmsg.ui
    waitmessagepage.ui
    electrictestpage.ui
    bscanmovementcontrols.ui
    options/externaltemperatureoptions.ui
    options/screenheateroptions.ui
    options/boltjointoptions.ui
    options/sensautoreset.ui
    systempasswordform.ui
    options/ekasuioptions.ui
    bluetooth/bluetoothmanagerpage.ui
    selectors/operatorselectionwidget.ui
    misc/popupdialogwidget.ui
    selectors/lineselectionwidget.ui
    registration/defaultregistrationpage.ui
    registration/ekasuiregistrationpage.ui
    selectors/ekasui/ekasuidirectionselectionwidget.ui
    selectors/ekasui/ekasuitracknumberselectionwidget.ui
    selectors/ekasui/ekasuistationselectionwidget.ui
    selectors/switchoperatorwidget.ui
    selectors/ekasui/ekasuidefectselectionwidget.ui
    defect/ekasuidefectpage.ui
    defect/ekasuiscannerdefectregistrationpage.ui
    indicatorbluetooth.ui
    defect/defaultdefectpage.ui
    defect/defaultscannerdefectregistrationpage.ui
    options/deviceoptions.ui
    misc/batteryplot.ui
    options/batteryoptions.ui
    misc/popuppasswordform.ui
    permissionspage.ui
    passwordmanagerpage.ui
    batterywidget.ui
    testexternalkeyboardwidget.ui
    selectors/notificationselectionwidget.ui
    selectors/genericselectionwidget.ui
    options/notificationoptions.ui
    memoryimportpage.ui
    selectors/fileplacementwidget.ui
    options/keycombinationsoptions.ui
    memoryusagewidget.ui
    options/optionslistpage.ui
    options/pathencoderemulatoroptions.ui
    switchtypepage.ui
    )

# WiFi UI
if(${TARGET_SELECTED_ARCH} STREQUAL "buildroot-imx" )
    include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/wifi/linux)
    include_directories(${CMAKE_SOURCE_DIR}/core/wifimanager)
    include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/camera/mxccamerapage)
    include_directories(${CMAKE_SOURCE_DIR}/core/mxccamera)
    set(SOURCES ${SOURCES} wifi/linux/wifipage.cpp
        wifi/linux/wifimanagerpage.cpp
        camera/mxccamerapage/camerapage.cpp
        )
    set(HEADERS ${HEADERS} wifi/linux/wifipage.h
        wifi/linux/wifimanagerpage.h
        camera/mxccamerapage/camerapage.h)
    set(FORMS ${FORMS} wifi/linux/wifipage.ui
        wifi/linux/wifimanagerpage.ui
        camera/mxccamerapage/camerapage.ui)
else()
    include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/wifi/fake)
    include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/camera/qcamerapage)
    set(SOURCES ${SOURCES} wifi/fake/wifimanagerpage.cpp
        camera/qcamerapage/camerapage.cpp)
    set(HEADERS ${HEADERS} wifi/fake/wifimanagerpage.h
        camera/qcamerapage/camerapage.h)
    set(FORMS ${FORMS} wifi/fake/wifimanagerpage.ui
            camera/qcamerapage/camerapage.ui)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

include_directories(${CMAKE_SOURCE_DIR}/ui/widgets)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/internationalization)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/ascanplot)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/mscanplot)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/spinboxes)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/controlsarea)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/lateralpanels)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/styles)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/options)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/selectors)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/misc)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/av17)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/camera)

include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/audiocomments)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/audiocomments/audioplayerpage)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/audiocomments/audiorecorderpage)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/video)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/video/videoplayer)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/video/videoviewer)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/virtualkeyboards)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/virtualkeyboards/envirtualkeyboard)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/virtualkeyboards/fnvirtualkeyboard)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/virtualkeyboards/ruvirtualkeyboard)
include_directories(${CMAKE_SOURCE_DIR}/ui/widgets/virtualkeyboards/devirtualkeyboard)

include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/defcore)
include_directories(${CMAKE_SOURCE_DIR}/defcore/datacontainer)
include_directories(${CMAKE_SOURCE_DIR}/core/audio)
include_directories(${CMAKE_SOURCE_DIR}/core/models)
include_directories(${CMAKE_SOURCE_DIR}/core/trackmarks)
include_directories(${CMAKE_SOURCE_DIR}/core/geoposition)
include_directories(${CMAKE_SOURCE_DIR}/core/powermanagement)
include_directories(${CMAKE_SOURCE_DIR}/core/screen)
include_directories(${CMAKE_SOURCE_DIR}/core/externalkeyboard)

#TARGET UI for Avicon-31
add_library(ui_31 ${SOURCES} ${HEADERS} ${FORMS})
target_compile_definitions(ui_31 PUBLIC -DTARGET_AVICON31)
target_link_libraries(ui_31 qwt core_31 defcore_31 Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Positioning Qt5::Svg Qt5::Xml Qt5::Sql dl)

#TARGET UI for Avicon-DB
add_library(ui_db ${SOURCES} ${HEADERS} ${FORMS})
target_compile_definitions(ui_db PUBLIC -DTARGET_AVICONDB)
target_link_libraries(ui_db qwt core_db defcore_db Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Positioning Qt5::Svg Qt5::Xml Qt5::Sql dl)

#TARGET UI for Avicon-DBHS
add_library(ui_dbhs ${SOURCES} ${HEADERS} ${FORMS})
target_compile_definitions(ui_dbhs PUBLIC -DTARGET_AVICONDBHS)
target_link_libraries(ui_dbhs qwt core_dbhs defcore_dbhs Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Positioning Qt5::Svg Qt5::Xml Qt5::Sql dl)

#TARGET UI for Avicon-15
add_library(ui_15 ${SOURCES} ${HEADERS} ${FORMS})
target_compile_definitions(ui_15 PUBLIC -DTARGET_AVICON15)
target_link_libraries(ui_15 qwt core_15 defcore_15 Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Positioning Qt5::Svg Qt5::Xml Qt5::Sql dl)

#TARGET UI for Avicon-31E
add_library(ui_31e ${SOURCES} ${HEADERS} ${FORMS})
target_compile_definitions(ui_31e PUBLIC -DTARGET_AVICON31E)
target_link_libraries(ui_31e qwt core_31e defcore_31e Qt5::Core Qt5::Widgets Qt5::Multimedia Qt5::MultimediaWidgets Qt5::Positioning Qt5::Svg Qt5::Xml Qt5::Sql dl)
