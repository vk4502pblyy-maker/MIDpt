QT       += core gui serialport opengl
QT  += network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += resources_big

QMAKE_CXXFLAGS += /utf-8

win32: RC_ICONS = window2.ico

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    IO/txtio.cpp \
    dlpc350_common.cpp \
    dlpc350_firmware.cpp \
    element/eleiconbtn.cpp \
    element/eleiconbtnstatus.cpp \
    element/elestatusonly.cpp \
    element/pbtnsingleshot.cpp \
    element/pbtntoggle.cpp \
    function/camerathread.cpp \
    function/funcdlp3500ctl.cpp \
    function/funcdlpc350api.cpp \
    function/funcdmdctlusb.cpp \
    function/funcmovewm.cpp \
    function/funcpztctl.cpp \
    function/funcsysconst.cpp \
    function/functcpclient.cpp \
    function/functionmovement.cpp \
    function/functionmovesl.cpp \
    function/rawreader.cpp \
    function/storagethread.cpp \
    main.cpp \
    mainwindow.cpp \
    view/rawimgwidget.cpp \
    view/videowidget.cpp \
    view/viewdlp4500scan.cpp \
    view/viewdlpc350scan.cpp \
    view/viewdlpscan.cpp \
    view/viewfloatconfig.cpp \
    view/viewfloatmovement.cpp \
    view/viewtitlemsgbox.cpp

HEADERS += \
    IO/txtio.h \
    dlpc350_common.h \
    dlpc350_error.h \
    dlpc350_firmware.h \
    dlpc350_flashDevice.h \
    element/eleiconbtn.h \
    element/eleiconbtnstatus.h \
    element/elestatusonly.h \
    element/pbtnsingleshot.h \
    element/pbtntoggle.h \
    function/camerathread.h \
    function/funcdlp3500ctl.h \
    function/funcdlpc350api.h \
    function/funcdmdctlusb.h \
    function/funcmovewm.h \
    function/funcpztctl.h \
    function/funcsysconst.h \
    function/functcpclient.h \
    function/functionmovement.h \
    function/functionmovesl.h \
    function/rawreader.h \
    function/storagethread.h \
    mainwindow.h \
    view/rawimgwidget.h \
    view/videowidget.h \
    view/viewdlp4500scan.h \
    view/viewdlpc350scan.h \
    view/viewdlpscan.h \
    view/viewfloatconfig.h \
    view/viewfloatmovement.h \
    view/viewtitlemsgbox.h

FORMS += \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32: LIBS += -L$$PWD/plugin/movement/ -lMovement

INCLUDEPATH += $$PWD/plugin/movement
DEPENDPATH += $$PWD/plugin/movement

win32: LIBS += -L$$PWD/plugin/movementSL/ -lLTSMC

INCLUDEPATH += $$PWD/plugin/movementSL
DEPENDPATH += $$PWD/plugin/movementSL

RESOURCES += \
    mainSource.qrc

win32: LIBS += -L$$PWD/plugin/hidapi/ -lhidapi

INCLUDEPATH += $$PWD/plugin/hidapi
DEPENDPATH += $$PWD/plugin/hidapi

# Pylon SDK 配置 (请替换为你的实际安装路径)
PYLON_ROOT = "D:/pylon6/Development"

INCLUDEPATH += $$PYLON_ROOT/include
DEPENDPATH  += $$PYLON_ROOT/include

LIBS += -L$$PYLON_ROOT/lib/x64 \
        -lGCBase_MD_VC141_v3_1_Basler_pylon \
        -lGenApi_MD_VC141_v3_1_Basler_pylon \
        -lPylonBase_v6_3 \
        -lPylonUtility_v6_3
