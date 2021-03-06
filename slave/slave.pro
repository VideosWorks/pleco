QMAKE_CXXFLAGS += -funsigned-char
QMAKE_CXXFLAGS += -Werror

CONFIG += qt debug warn_on link_prl link_pkgconfig
QT += network
PKGCONFIG += gstreamer-1.0 gstreamer-app-1.0 gstreamer-video-1.0
for(PKG, $$list($$unique(PKGCONFIG))) {
    !system(pkg-config --exists $$PKG):error($$PKG development files are missing)
}

INCLUDEPATH += ../common
LIBS += -L../common -lcommon

SOURCES += Slave.cpp
SOURCES += VideoSender.cpp
SOURCES += AudioSender.cpp
SOURCES += ControlBoard.cpp
SOURCES += main.cpp
SOURCES += Hardware.cpp
SOURCES += Camera.cpp

HEADERS += Slave.h
HEADERS += VideoSender.h
HEADERS += AudioSender.h
HEADERS += ControlBoard.h
HEADERS += Hardware.h
HEADERS += Camera.h

TARGET = slave
INSTALLS += target
target.path = $$PREFIX/bin
