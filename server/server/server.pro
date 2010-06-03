# -------------------------------------------------
# Project created by QtCreator 2010-03-31T11:27:25
# -------------------------------------------------
QT += network
OBJECTS_DIR = obj
MOC_DIR = obj
TARGET = server
TEMPLATE = app
CONFIG += release
SOURCES += main.cpp \
    cserver.cpp \
    cmapgen.cpp \
    cclient.cpp \
    cnetworkmanager.cpp \
    cloger.cpp
HEADERS += cserver.h \
    cmapgen.h \
    cclient.h \
    cnetworkmanager.h \
    cloger.h

win32:LIBS += -lws2_32
