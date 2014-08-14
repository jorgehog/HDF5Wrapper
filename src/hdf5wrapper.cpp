#include "hdf5wrapper.h"

#include <iostream>

using namespace H5;
using namespace std;

HDF5Wrapper::HDF5Wrapper(const std::string filename) :
    HDF5Member(NULL, ""),
    m_filename(filename),
    m_state(state::OPEN)
{

}

HDF5Wrapper::~HDF5Wrapper()
{
    if (m_state == state::OPEN)
    {
        finalize();
    }
}


void HDF5Wrapper::initialize(const uint flag)
{
    Exception::dontPrint();

    try
    {
        m_file = new H5File(m_filename, flag);
        cout << "Opened HDF5 file " << m_filename << endl;

        _onFileOpen();
    }
    catch (const FileIException &exc)
    {
        (void) exc;

        m_file = new H5File(m_filename, H5F_ACC_TRUNC);
        cout << "Created new HDF5 file " << m_filename << endl;

    }

    m_group = new Group(m_file->openGroup("/"));

}

void HDF5Wrapper::finalize()
{
    delete m_file;
    m_state = state::CLOSED;
}

void HDF5Wrapper::_onFileOpen()
{

}

