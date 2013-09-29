# -------------------------------------------------
# Project created by QtCreator 2009-03-21T21:41:37
# -------------------------------------------------
QT += network
TARGET = cronjobs_stella
TEMPLATE = lib
CONFIG         += plugin
INCLUDEPATH    += ../../
DESTDIR         = ../
SOURCES += cronjobs.cpp ../../stellaconnection.cpp
HEADERS += cronjobs.h ../../stellaconnection.h ../../plugins_interface.h
FORMS += cronjobs.ui
RESOURCES += ../../app.qrc
