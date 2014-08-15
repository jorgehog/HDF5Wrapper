#include "root.h"

#include <iostream>

using namespace H5;
using namespace H5Wrapper;
using namespace std;


Root::Root(const std::string filename) :
    Member(NULL, ""),
    m_filename(filename),
    m_state(state::OPEN)
{

}

Root::~Root()
{
    if (m_state == state::OPEN)
    {
        finalize();
    }
}


void Root::initialize(const uint flag)
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

void Root::finalize()
{
    Member::finalize();

    delete m_file;
    m_state = state::CLOSED;
}

