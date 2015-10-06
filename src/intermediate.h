#pragma once

#include "member.h"

namespace H5Wrapper
{

struct Intermediate
{
    Intermediate(Member *member,
         const string key,
         const bool overWrite = true) :
        m_member(member),
        m_key(key),
        m_overWrite(overWrite)
    {}

    template<typename eT>
    Intermediate& operator=(const eT &data)
    {
        m_member->addData(m_key, data, m_overWrite);
        return *this;
    }

    template<typename eT>
    operator eT()
    {
        eT *elem = new eT;

        m_member->readData(m_key, *elem);

        return *elem;
    }

    Member *m_member;
    const string m_key;
    const bool m_overWrite;
};

}
