INCLUDEPATH += /usr/local/hdf5/include

LIBS += -L../lib -lHDF5Wrapper -L/usr/local/hdf5/lib -lhdf5_cpp -lhdf5

!noccache {
    QMAKE_CXX = ccache $$QMAKE_CXX
}

INCLUDEPATH += $$PWD/utils

COMMON_CXXFLAGS = -std=c++11

QMAKE_CXXFLAGS += \
    $$COMMON_CXXFLAGS

QMAKE_CXXFLAGS_DEBUG += \
    $$COMMON_CXXFLAGS

QMAKE_CXXFLAGS_RELEASE += \
    $$COMMON_CXXFLAGS \
    -O3 \
    -DNDEBUG \
    -DARMA_NO_DEBUG

QMAKE_LFLAGS_RELEASE -= -O1
QMAKE_LFLAGS_RELEASE += -O3

DEFINES += \
    ARMA_MAT_PREALLOC=3

