TEMPLATE = lib

TARGET = ../lib/HDF5Wrapper

include(../defaults.pri)


QMAKE_LFLAGS_DEBUG += -g


SOURCES += hdf5wrapper.cpp \
    hdf5member.cpp

HEADERS += hdf5wrapper.h \
    hdf5member.h


QMAKE_PRE_LINK += $(MKDIR) $$PWD/../lib $$shadowed($$PWD)/../lib

!equals(PWD, $${OUT_PWD}) {
    QMAKE_POST_LINK += $(COPY_DIR) $$OUT_PWD/../lib $$PWD/../
}
