TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    hdf5wrapper.cpp

INCLUDEPATH +=  /usr/local/hdf5/include

LIBS += -L/usr/local/hdf5/lib -lhdf5_cpp -lhdf5

OTHER_FILES += \
    /usr/local/hdf5/share/hdf5_examples/c++/chunks.cpp \
    /usr/local/hdf5/share/hdf5_examples/c++/compound.cpp \
    /usr/local/hdf5/share/hdf5_examples/c++/create.cpp \
    /usr/local/hdf5/share/hdf5_examples/c++/extend_ds.cpp \
    /usr/local/hdf5/share/hdf5_examples/c++/h5group.cpp \
    /usr/local/hdf5/share/hdf5_examples/c++/h5tutr_cmprss.cpp \
    /usr/local/hdf5/share/hdf5_examples/c++/h5tutr_crtatt.cpp \
    /usr/local/hdf5/share/hdf5_examples/c++/h5tutr_crtdat.cpp \
    /usr/local/hdf5/share/hdf5_examples/c++/h5tutr_crtgrpar.cpp \
    /usr/local/hdf5/share/hdf5_examples/c++/h5tutr_crtgrp.cpp \
    /usr/local/hdf5/share/hdf5_examples/c++/h5tutr_crtgrpd.cpp \
    /usr/local/hdf5/share/hdf5_examples/c++/h5tutr_extend.cpp \
    /usr/local/hdf5/share/hdf5_examples/c++/h5tutr_rdwt.cpp \
    /usr/local/hdf5/share/hdf5_examples/c++/h5tutr_subset.cpp \
    /usr/local/hdf5/share/hdf5_examples/c++/readdata.cpp \
    /usr/local/hdf5/share/hdf5_examples/c++/writedata.cpp \

HEADERS += \
    hdf5wrapper.h
