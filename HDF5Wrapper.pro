TEMPLATE = subdirs
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += ordered

SUBDIRS += src tests

OTHER_FILES += include/hdf5wrapper.h

OTHER_FILES += \
        /usr/local/hdf5/share/hdf5_examples/c++/h5tutr_crtdat.cpp \
        /usr/local/hdf5/share/hdf5_examples/c++/h5tutr_crtgrpd.cpp \
        /usr/local/hdf5/share/hdf5_examples/c++/writedata.cpp \
        /usr/local/hdf5/share/hdf5_examples/c++/h5tutr_crtgrpar.cpp \
        /usr/local/hdf5/share/hdf5_examples/c++/h5tutr_cmprss.cpp \
        /usr/local/hdf5/share/hdf5_examples/c++/h5tutr_crtgrp.cpp \
        /usr/local/hdf5/share/hdf5_examples/c++/h5group.cpp \
        /usr/local/hdf5/share/hdf5_examples/c++/compound.cpp \
        /usr/local/hdf5/share/hdf5_examples/c++/h5tutr_subset.cpp \
        /usr/local/hdf5/share/hdf5_examples/c++/extend_ds.cpp \
        /usr/local/hdf5/share/hdf5_examples/c++/h5tutr_extend.cpp \
        /usr/local/hdf5/share/hdf5_examples/c++/readdata.cpp \
        /usr/local/hdf5/share/hdf5_examples/c++/h5tutr_crtatt.cpp \
        /usr/local/hdf5/share/hdf5_examples/c++/create.cpp \
        /usr/local/hdf5/share/hdf5_examples/c++/chunks.cpp \
        /usr/local/hdf5/share/hdf5_examples/c++/h5tutr_rdwt.cpp

