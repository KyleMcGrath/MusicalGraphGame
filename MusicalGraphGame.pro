# Produced by Kyle McGrath in 2020
# All rights reserved.

QT       += core gui multimedia opengl network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MusicalGraphGame
TEMPLATE = app


QMAKE_CFLAGS -= -o
QMAKE_CXXFLAGS -= -o
QMAKE_LFLAGS -= -o

QMAKE_CFLAGS -= -o2
QMAKE_CXXFLAGS -= -o2
QMAKE_LFLAGS -= -o2

QMAKE_CFLAGS = -m64 -o3
QMAKE_CXXFLAGS = -m64 -o3
QMAKE_LFLAGS = -m64 -o3

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    src/view/main/dialog.cpp \
    src/main.cpp \
    src/view/main/mainwindow.cpp \
    src/model/modulargraph.cpp \
    src/view/graph/node.cpp \
    src/view/main/graphicsscene.cpp \
    src/view/main/graphicsview.cpp \
    src/view/main/welcome.cpp \
    src/view/main/about.cpp \
    src/view/main/scoreboard.cpp \
    src/view/main/summary.cpp \
    src/view/graph/edgeset.cpp \
    src/view/main/graphicsviewnogesture.cpp \
    src/view/main/gamedetails.cpp \
    src/view/main/gamelist.cpp \
    src/view/main/movmodboard.cpp \
    src/view/main/progressbar.cpp \
    src/view/clock/clock.cpp \
    src/view/main/sboard.cpp \
    src/view/main/summary/bar.cpp

HEADERS += \
    include/view/main/dialog.h \
    include/view/main/mainwindow.h \
    include/model/modulargraph.h \
    include/view/graph/node.h \
    include/view/main/graphicsscene.h \
    include/view/main/graphicsview.h \
    include/view/main/welcome.h \
    include/view/main/about.h \
    include/view/main/scoreboard.h \
    include/view/main/summary.h \
    include/view/graph/edgeset.h \
    include/view/main/graphicsviewnogesture.h \
    include/model/embeddings.h \
    include/view/main/gamedetails.h \
    include/view/main/gamelist.h \
    include/model/mediastatics.h \
    include/view/main/movmodboard.h \
    include/view/main/progressbar.h \
    include/view/clock/clock.h \
    include/view/main/sboard.h \
    include/view/main/summary/bar.h

FORMS += \
    ui/dialog.ui \
    ui/mainwindow.ui \
    ui/modulargraph.ui \
    ui/welcome.ui \
    ui/about.ui \
    ui/summary.ui \
    ui/gamedetails.ui \
    ui/gamelist.ui

RESOURCES += \
    res.qrc

ios {
    QMAKE_INFO_PLIST =

}
