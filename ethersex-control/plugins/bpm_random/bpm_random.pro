# -------------------------------------------------
# Project created by QtCreator 2009-03-21T21:41:37
# -------------------------------------------------
QT += network
TARGET = bpm_random
TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../
DESTDIR = ../
SOURCES += bpm_random.cpp \
    ../../stellaconnection.cpp \
    modelusedchannels.cpp
HEADERS += bpm_random.h \
    ../../stellaconnection.h \
    ../../plugins_interface.h \
    modelusedchannels.h
FORMS += bpm_random.ui
RESOURCES += ../../app.qrc
