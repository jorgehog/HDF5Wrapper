#include "root.h"

#include <iostream>

using namespace H5;
using namespace H5Wrapper;
using namespace std;

#include <hdf5.h>

Root::Root(const std::string filename, const uint flag) :
    Member(NULL, ""),
    m_filename(filename),
    m_state(state::CLOSED)
{
    _initialize(flag);
}

Root::~Root()
{
    if (m_state == state::OPEN)
    {
        _finalize();
    }
}


void Root::_initialize(const uint flag)
{
    Exception::dontPrint();

    try
    {
        m_file = new H5File(m_filename, flag);
        cout << "Opened HDF5 file " << m_filename << endl;

        m_group = new Group(m_file->openGroup("/"));

        _loadFromFile();

    }
    catch (const FileIException &exc)
    {
        (void) exc;

        m_file = new H5File(m_filename, H5F_ACC_TRUNC);

        cout << "Created new HDF5 file " << m_filename << endl;

        m_group = new Group(m_file->openGroup("/"));
    }
}

void Root::_finalize()
{
    Member::finalize();

    delete m_file;
    m_state = state::CLOSED;
}

