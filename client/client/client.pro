CONFIG += release
OBJECTS_DIR = obj
MOC_DIR = obj
win32:INCLUDEPATH += C:\Qt\2010.02.1\mingw\include \
    LIBPATH \
    = \
    -L"C:\Qt\2010.02.1\mingw\lib"
debug:LIBS += -lsfml-audio-d \
    -lsfml-graphics-d \
    -lsfml-window-d \
    -lsfml-system-d \
    -lsfml-main
release:LIBS += -lsfml-audio \
    -lsfml-graphics \
    -lsfml-window \
    -lsfml-system \
    -lsfml-main
win32:LIBS += -lws2_32
DEFINES += SFML_DYNAMIC
HEADERS += cconnect.h \
    QSFMLCanvas.hpp \
    cclient.h \
    cgameboard.h \
    Frame.h \
    Anim.h \
    Animated.h \
    cplayer.h \
    cmap.h \
    cimagemanager.h \
    cexplosion.h \
    cparticle.h \
    cbonus.h \
    cbonuscanvas.h \
    climitedbonus.h \
    cbomb.h \
    cnetworkmanager.h \
    constants.h
SOURCES += cconnect.cpp \
    main.cpp \
    QSFMLCanvas.cpp \
    cgameboard.cpp \
    cclient.cpp \
    Frame.cpp \
    Anim.cpp \
    Animated.cpp \
    cplayer.cpp \
    cmap.cpp \
    cimagemanager.cpp \
    cparticle.cpp \
    cexplosion.cpp \
    cbonus.cpp \
    cbonuscanvas.cpp \
    climitedbonus.cpp \
    cbomb.cpp \
    cnetworkmanager.cpp
QT += network
QT += multimedia
