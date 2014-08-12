#pragma once

#include "hdf5member.h"

#include <H5Cpp.h>


using H5::H5File;
using std::string;

class HDF5Wrapper : public HDF5Member
{
public:

    HDF5Wrapper(const string filename);

    virtual ~HDF5Wrapper();


    void initialize(const uint flag);

    void initialize()
    {
        initialize(H5F_ACC_RDWR);
    }

    void finalize();

    enum class state
    {
        OPEN,
        CLOSED
    };


private:

    const string m_filename;

    H5File *m_file;

    state m_state;

    void _onFileOpen();

};
