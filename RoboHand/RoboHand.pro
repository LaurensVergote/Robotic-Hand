QT      += opengl widgets

HEADERS += \
    glwidget.h \
    window.h \
    gesture.h \
    hand.h \
    geometry.h \
    qmesh.h \
    qnode.h \
    pickingray.h \
    savenode.h \
    serialcommunicator.h

SOURCES += \
    glwidget.cpp \
    window.cpp \
    main.cpp \
    gesture.cpp \
    hand.cpp \
    geometry.cpp \
    qmesh.cpp \
    qnode.cpp \
    pickingray.cpp \
    savenode.cpp \
    serialcommunicator.cpp

win32: LIBS += -L$$PWD/../Assimp/lib/x64/ -lassimp

INCLUDEPATH += $$PWD/../Assimp/include
DEPENDPATH += $$PWD/../Assimp/include

win32: LIBS += -L$$PWD/../../../../../../../SDK/Python/libs/ -lpython27

INCLUDEPATH += $$PWD/../../../../../../../SDK/Python/include
DEPENDPATH += $$PWD/../../../../../../../SDK/Python/include

win32: PRE_TARGETDEPS += $$PWD/../../../../../../../SDK/Python/libs/python27.lib
