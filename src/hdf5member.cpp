#include "hdf5member.h"

HDF5Member::HDF5Member(const HDF5Member *parent, const string ID) :
    m_parent(parent),
    m_ID(ID)
{

}

HDF5Member::~HDF5Member()
{
    for (auto &member : m_members)
    {
        delete member.second;
    }

    m_members.clear();

}
