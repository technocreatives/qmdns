CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    qmdns.cpp \
    $$PWD/qmdnsservice.cpp

HEADERS += \
    qmdns.h \
    $$PWD/qmdnsservice.h

linux {
    LIBS+= -lavahi-client -lavahi-common
}
