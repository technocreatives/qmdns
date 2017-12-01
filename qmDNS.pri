CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    qmdns.cpp \
    $$PWD/qmdnsservice.cpp \
    $$PWD/qmdnsbrowser.cpp \
    $$PWD/utils.cpp

HEADERS += \
    qmdns.h \
    $$PWD/qmdnsservice.h \
    $$PWD/qmdnsbrowser.h \
    $$PWD/utils.h

linux {
    LIBS+= -lavahi-client -lavahi-common
}
