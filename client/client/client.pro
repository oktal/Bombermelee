CONFIG += debug \
    qt \
    console
INCLUDEPATH += C:\Qt\2010.02.1\mingw\include \
    LIBPATH \
    = \
    -L"C:\Program Files\CodeBlocks\MinGW\lib"
LIBS += -lsfml-audio-d \
    -lsfml-graphics-d \
    -lsfml-window-d \
    -lsfml-system-d \
    -lsfml-main
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
    climitedbonus.h
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
    climitedbonus.cpp
QT += network
QT += multimedia
