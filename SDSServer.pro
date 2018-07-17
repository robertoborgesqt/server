#-------------------------------------------------
#
# Project created by QtCreator 2018-05-24T17:15:44
#
#-------------------------------------------------

QT       += core gui network
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
TARGET = SDSServer
TEMPLATE = app

PKGCONFIG += opencv
INCLUDEPATH += /usr/local//Cellar/opencv/3.4.1_5/include \
               /usr/local//Cellar/opencv/3.4.1_5/include/opencv2/core \
               ../SDSCommon

LIBS += -L/usr/local/Cellar/opencv/3.4.1_5/lib \
        -lopencv_videoio \
        -lopencv_core  \
        -lopencv_highgui \
        -lopencv_imgcodecs \
        -lopencv_imgproc \
        -lopencv_features2d  \
        -lopencv_calib3d

DEFINES += QT_DEPRECATED_WARNINGS SDS_CAMERA_SERVER

SOURCES += \
        main.cpp \
        form.cpp \
        camera.cpp \
        config.cpp \
        ../SDSCommon/csocketdevice.cpp \
        ../SDSCommon/cproxybase.cpp \
        ../SDSCommon/cproxytcp.cpp \
    tcpsender.cpp \
    ../SDSCommon/ccrc.cpp

HEADERS += \
        form.h \
        camera.h \
        config.h \
        ../SDSCommon/csocketdevice.h \
        ../SDSCommon/cproxybase.h \
        ../SDSCommon/cproxytcp.h \
    tcpsender.h \
    ../SDSCommon/ccrc.h

FORMS += \
        form.ui \
        window.ui \
        camera.ui \
        config.ui

RESOURCES += \
        recursos.qrc
