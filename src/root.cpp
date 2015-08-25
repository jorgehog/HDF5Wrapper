#include "root.h"

#include <iostream>

using namespace H5;
using namespace H5Wrapper;
using namespace std;

#include <hdf5.h>

Root::Root(const std::string filename, const uint flag) :
    Member(nullptr, ""),
    m_filename(filename),
    m_state(state::CLOSED)
{
    open(flag);
}

Root::~Root()
{
    close();
}


void Root::open(const uint flag)
{
    if (m_state == state::OPEN)
    {
        return;
    }

    Exception::dontPrint();

    try
    {
        m_file = new H5File(m_filename, flag);
//        cout << "Opened HDF5 file " << m_filename << endl;

        m_group = new Group(m_file->openGroup("/"));

        _loadFromFile();

    }
    catch (const FileIException &exc)
    {
        (void) exc;

        m_file = new H5File(m_filename, H5F_ACC_TRUNC);

//        cout << "Created new HDF5 file " << m_filename << endl;

        m_group = new Group(m_file->openGroup("/"));
    }

    m_state = state::OPEN;
}

void Root::close()
{
    if (m_state == state::OPEN)
    {
        flush();

        _clearall();

        m_file->close();
        delete m_file;

        m_state = state::CLOSED;
    }
}

