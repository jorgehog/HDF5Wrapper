#include "hdf5wrapper.h"

#include <iostream>

using namespace H5;
using namespace std;

HDF5Wrapper::HDF5Wrapper(const std::string filename):
    m_filename(filename)
{

}

HDF5Wrapper::~HDF5Wrapper()
{
    delete m_file;
}


void HDF5Wrapper::initialize(const uint flag)
{
    Exception::dontPrint();

    try
    {
        m_file = new H5File(m_filename, flag);
        cout << "Opened HDF5 file " << m_filename << endl;
    }
    catch (const FileIException &exc)
    {
        (void) exc;

        cout << "Creating new HDF5 file " << m_filename << endl;
        m_file = new H5File(m_filename, H5F_ACC_TRUNC);

    }

}
