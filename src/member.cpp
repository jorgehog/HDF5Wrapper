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
    for (auto &member : m_members)
    {
        delete member.second;
    }

    m_members.clear();
    m_allSetKeys.clear();
    m_allAttrKeys.clear();

    delete m_group;
}

void Member::finalize()
{
    //        _dumpKeysToGroup();

    for (auto & member : m_members)
    {
        member.second->finalize();
    }
}

void Member::purge()
{

    for (const string &key : m_allAttrKeys)
    {
        m_group->removeAttr(key);
    }

    for (const string &key : m_allSetKeys)
    {
        m_group->unlink(key);
    }

    for (auto &member : m_members)
    {
        member.second->purge();
    }

    m_allAttrKeys.clear();
    m_allSetKeys.clear();

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

vector<string> Member::_memberKeys() const
{
    vector<string> memberKeys(m_members.size());

    uint i = 0;
    for (const auto &member : m_members)
    {
        memberKeys.at(i) = member.first;
    }

    return memberKeys;
}

//const mpi::environment Member::env;
//const mpi::communicator Member::world;
