# -------------------------------------------------
# Project created by QtCreator 2009-03-21T21:41:37
# -------------------------------------------------
QT += network
TARGET = moodlight
TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../
DESTDIR = ../
SOURCES += moodlight.cpp \
    ../../stellaconnection.cpp \
    modelusedchannels.cpp
HEADERS += moodlight.h \
    ../../stellaconnection.h \
    ../../plugins_interface.h \
    modelusedchannels.h
FORMS += moodlight.ui
RESOURCES += ../../app.qrc
