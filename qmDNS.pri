CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    qmdns.cpp

HEADERS += \
    qmdns.h

linux {
    LIBS+= -lavahi-client -lavahi-common
}
