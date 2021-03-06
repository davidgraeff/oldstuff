#-------------------------------------------------
#
# Project created by QtCreator 2012-02-07T16:21:10
#
#-------------------------------------------------

QT       += core gui network

TARGET = windowsclient
TEMPLATE = app

INCLUDEPATH += ../_sharedsrc ../../

SOURCES += main.cpp \
        configurationwidget.cpp \
        json.cpp \
        actioncontroller.cpp \
        networkcontroller.cpp

HEADERS  += configurationwidget.h \
        networkcontroller.h \
        json.h \
        actioncontroller.h

FORMS    += configurationwidget.ui

win32 {
    DEFINES += _WIN32_WINNT=0x0500
    LIBS += -luser32
    CONFIG += embed_manifest_exe
    RC_FILE = winres.rc
}

RESOURCES += \
    res.qrc

OTHER_FILES += \
    windowsclient.exe.manifest winres.rc
