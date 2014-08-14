TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

include(../defaults.pri)

LIBS += -L../lib -lHDF5Wrapper

SOURCES += testsmain.cpp

INCLUDEPATH += ../include
