#include "member.h"

using namespace H5Wrapper;
using namespace std;

Member::Member(const Member *parent, const string ID) :
    m_parent(parent),
    m_ID(ID),
    m_file(isRoot() ? NULL : parent->file())
{

    if (!isRoot())
    {
        try
        {
            m_group = new Group(m_file->createGroup(absoluteName()));
        }
        catch(const H5::FileIException &)
        {
            m_group = new Group(m_file->openGroup(absoluteName()));

            _loadFromFile();
        }
    }

}

Member::~Member()
{
    _clearall();
}

void Member::purge()
{

    for (const string &key : m_attributes)
    {
        m_group->removeAttr(key);
    }

    for (const string &key : m_datasets)
    {
        m_group->unlink(key);
    }

    for (auto &member : m_members)
    {
        member.second->purge();
    }

    m_attributes.clear();
    m_datasets.clear();

}

bool Member::_notStorable(const void *buffer, const vector<size_t> &dims) const
{
    //Zero rank, null buffer, or either dimension zero is not storable.

    bool notStorable = (buffer == NULL) || (dims.size() == 0);

    for (const uint &dim : dims)
    {
        notStorable = notStorable || (dim == 0);
    }

    return notStorable;
}

void Member::_clearall()
{
    for (auto &member : m_members)
    {
        delete member.second;
    }

    m_members.clear();
    m_datasets.clear();
    m_attributes.clear();

    if (m_group != NULL)
    {
        delete m_group;
        m_group = NULL;
    }
}
