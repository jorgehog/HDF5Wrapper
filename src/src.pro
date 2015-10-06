TEMPLATE = lib

TARGET = ../lib/HDF5Wrapper

include(../defaults.pri)

QMAKE_LFLAGS_DEBUG += -g

noarma {
    DEFINES += HDF5_NO_ARMA
    LIBS += -larmadillo
}

SOURCES += \
    root.cpp \
    member.cpp

HEADERS += \
    ctopredtype.h \
    root.h \
    member.h \
    intermediate.h


QMAKE_PRE_LINK += $(MKDIR) $$PWD/../lib $$shadowed($$PWD)/../lib

!equals(PWD, $${OUT_PWD}) {
    QMAKE_POST_LINK += $(COPY_DIR) $$OUT_PWD/../lib $$PWD/../
}
