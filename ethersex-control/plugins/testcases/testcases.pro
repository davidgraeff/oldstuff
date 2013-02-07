# -------------------------------------------------
# Project created by QtCreator 2009-03-21T21:41:37
# -------------------------------------------------
QT += network
TARGET = testcases
TEMPLATE = lib
CONFIG         += plugin
INCLUDEPATH    += ../../
DESTDIR         = ../
SOURCES += testcases.cpp ../../stellaconnection.cpp
HEADERS += testcases.h ../../stellaconnection.h ../../plugins_interface.h
FORMS += testcases.ui
RESOURCES += ../../app.qrc
