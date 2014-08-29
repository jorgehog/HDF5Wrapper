INCLUDEPATH += /usr/local/hdf5/include

LIBS +=-L/usr/local/hdf5/lib -lhdf5_cpp -lhdf5

#!noccache {
#    QMAKE_CXX = ccache $$QMAKE_CXX
#}

INCLUDEPATH += $$PWD/utils

COMMON_CXXFLAGS = -std=c++11


mpi {
    DEFINES += H5_USE_MPI
    QMAKE_CXX = mpicxx

    QMAKE_LINK = $$QMAKE_CXX

    QMAKE_LFLAGS += $$system(mpicxx --showme:link)
    COMMON_CXXFLAGS += $$system(mpicxx --showme:compile) -DMPICH_IGNORE_CXX_SEEK

    LIBS += -lboost_mpi
}

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

