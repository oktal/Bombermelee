# -------------------------------------------------
# Project created by QtCreator 2010-03-31T11:27:25
# -------------------------------------------------
QT += network
CONFIG += debug \
    qt \
    console
TARGET = server
TEMPLATE = app
CONFIG += release
SOURCES += main.cpp \
    cserver.cpp \
    cmapgen.cpp \
    cclient.cpp \
    cnetworkmanager.cpp
HEADERS += cserver.h \
    cmapgen.h \
    cclient.h \
    cnetworkmanager.h
