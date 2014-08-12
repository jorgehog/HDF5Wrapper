#pragma once

#include <sys/types.h>
#include <H5Cpp.h>

using H5::H5File;
using std::string;

class HDF5Wrapper
{
public:

    HDF5Wrapper(const string filename);

    virtual ~HDF5Wrapper();

    void initialize()
    {
        initialize(H5F_ACC_RDWR);
    }

    void initialize(const uint flag);

private:

    const string m_filename;

    H5File *m_file;

};
