# -------------------------------------------------
# Project created by QtCreator 2009-02-28T18:28:31
# -------------------------------------------------
QT += network
TARGET = StellaLight
TEMPLATE = app
SOURCES += main.cpp \
    mainwindow.cpp \
    stellaconnection.cpp \
    stellaconnecting.cpp
HEADERS += mainwindow.h \
    stella.h \
    stellaconnection.h \
    plugins_interface.h \
    stellaconnecting.h
FORMS += mainwindow.ui \
    stellaconnecting.ui
RESOURCES += app.qrc
RC_FILE = app.rc
OTHER_FILES += COPYING \
    app.rc \
    CHANGELOG \
    README
