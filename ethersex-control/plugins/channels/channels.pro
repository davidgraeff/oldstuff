# -------------------------------------------------
# Project created by QtCreator 2009-03-22T15:44:13
# -------------------------------------------------
QT += network
TARGET = channels
TEMPLATE = lib
CONFIG += plugin
INCLUDEPATH += ../../
DESTDIR = ../
DEFINES += CHANNELS_LIBRARY
SOURCES += channels.cpp \
    ../../stellaconnection.cpp \
    pwmchannel.cpp
HEADERS += channels.h \
    ../../stellaconnection.h \
    ../../plugins_interface.h \
    pwmchannel.h
RESOURCES += ../../app.qrc
FORMS += channels.ui \
    pwmchannel.ui
