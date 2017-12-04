CONFIG += c++11

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    $$PWD/qmdns.cpp \
    $$PWD/qmdnsservice.cpp \
    $$PWD/qmdnsbrowser.cpp \
    $$PWD/utils.cpp

HEADERS += \
    $$PWD/qmdns.h \
    $$PWD/qmdnsservice.h \
    $$PWD/qmdnsbrowser.h \
    $$PWD/utils.h

linux {
    LIBS+= -lavahi-client -lavahi-common
}
