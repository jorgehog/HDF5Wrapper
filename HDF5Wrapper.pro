TEMPLATE = subdirs
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt
CONFIG += ordered

SUBDIRS += src tests

OTHER_FILES += include/hdf5wrapper.h
