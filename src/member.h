#pragma once

#include <H5Cpp.h>

#include <BADAss/badass.h>

#include <string>
#include <sys/types.h>
#include <sstream>
#include <map>

#ifndef HDF5_NO_ARMA
#include <armadillo>
using namespace arma;
#endif

using std::string;
using std::map;
using std::vector;
using std::pair;

using H5::H5File;
using H5::Group;
using H5::Attribute;
using H5::DataSet;
using H5::DataSpace;



#include <iostream>
using std::cout;
using std::endl;

#include "ctopredtype.h"

namespace H5Wrapper
{

class Member
{
public:

    Member(const Member *parent, const string ID) :
        m_parent(parent),
        m_ID(ID),
        m_file(isRoot() ? NULL : parent->file())
    {

        if (!isRoot())
        {
            try
            {
                m_group = new Group(m_file->createGroup(getFullGroup()));
            }
            catch(const H5::FileIException &)
            {
                m_group = new Group(m_file->openGroup(getFullGroup()));
            }
        }

    }

    virtual ~Member()
    {
        for (auto &member : m_members)
        {
            delete member.second;
        }

        _dumpKeysToGroup();

        m_members.clear();
        m_allSetKeys.clear();
        m_allAttrKeys.clear();

        delete m_group;
    }

    bool isRoot() const
    {
        return m_parent == NULL;
    }

    const string &ID() const
    {
        return m_ID;
    }

    H5File *file() const
    {
        return m_file;
    }

    const string getFullGroup() const
    {
        if (isRoot())
        {
            return "/";
        }

        return m_parent->getFullGroup() + m_ID + "/";
    }

    void clear()
    {

    }

    template<typename kT>
    bool hasMember(const kT &_key) const
    {
        string key = _stringify(_key);

        return m_members.find(key) != m_members.end();
    }

    template<typename kT>
    bool hasSet(const kT &_key) const
    {
        string key = _stringify(_key);

        try
        {
            m_group->openDataSet(key);
            return true;
        }
        catch (const H5::GroupIException &)
        {
            return false;
        }

    }

    template<typename kT>
    Member *addMember(const kT &_key) {
        string key = _stringify(_key);

        BADAssBool(!hasMember(key), "Key already exists.");

        Member *newMember = new Member(this, key);

        m_members[key] = newMember;

        return newMember;
    }

    template<typename kT>
    void removeMember(const kT &_key)
    {
        string key = _stringify(_key);

        BADAssBool(hasMember(key), "Member not found: " + key);

        Member *member = m_members[key];

        member->clear();

        delete member;

        m_members.erase(key);

    }

    template<typename kT>
    const Member *operator [](const kT &_key) const
    {
        string key = _stringify(_key);

        BADAssBool(hasMember(key), "Member not found: " + key);


        return (*m_members.find(key)).second;
    }

    template<typename kT>
    Member *operator[](const kT &_key)
    {
        string key = _stringify(_key);

        BADAssBool(hasMember(key), "Member not found: " + key);


        return m_members[key];
    }

    //Pointer is the main storage function. All others simply channel this one.
    template<typename kT, typename eT>
    typename std::enable_if<std::is_pointer<eT>::value, bool>::type
    addData(const kT &_setname, const eT &data, const uint rank, hsize_t dims[])
    {
        if (_notStorable(data, rank, dims))
        {
            return false;
        }

        cout << "Storing pointer type " << typeid(eT).name() << " of size " << sizeof(data) << endl;

        string setname = _stringify(_setname);

        try
        {
            DataSet dataset(m_file->createDataSet(getFullGroup() + setname, CToPredType<eT>::type(), DataSpace(rank, dims)));
            dataset.write(data, CToPredType<eT>::type());
            m_allSetKeys.push_back(setname);
            return true;
        }
        catch (const H5::FileIException &)
        {
            return false;
        }

    }

    //Static object.
    template<typename kT, typename eT>
    typename std::enable_if<!std::is_pointer<eT>::value && !std::is_integral<eT>::value, bool>::type
    addData(const kT &setname, const eT &data, const uint rank, hsize_t dims[])
    {
        cout << "Storing " << typeid(eT).name() << " of size " << sizeof(data) << endl;
        return addData(setname, &data, rank, dims);
    }

    //Std::string set
    template<typename kT>
    bool
    addData(const kT &_setname, const string *data, const uint rank, hsize_t dims[])
    {
        if (_notStorable(data, rank, dims))
        {
            return false;
        }

        cout << "Storing pointer string type " << *data  << endl;

        const uint L = dims[0];



        const char** newData = new const char*[L];

        for (uint i = 0; i < dims[0]; ++i)
        {
            newData[i] = (*(data + i)).c_str();
        }

        string setname = _stringify(_setname);

        return addData(_setname, newData, rank, dims);
        try
        {
            DataSet dataset(m_file->createDataSet(getFullGroup() + setname, CToPredType<char*>::type(), DataSpace(rank, dims)));
            dataset.write(newData, CToPredType<char*>::type());
            m_allSetKeys.push_back(setname);
            delete newData;
            return true;
        }
        catch (const H5::FileIException &)
        {
            delete newData;
            return false;
        }

    }

    //single std::string
    template<typename kT>
    bool
    addData(const kT &_attrname, const string &data)
    {
        cout << "Storing attribute string " << endl;

        string attrname = _stringify(_attrname);

        try
        {
            Attribute attr(m_group->createAttribute(attrname, CToPredType<char*>::type(), H5S_SCALAR));
            attr.write(CToPredType<char*>::type(), data.c_str());
            m_allAttrKeys.push_back(attrname);
            return true;
        }
        catch(const H5::AttributeIException &)
        {
            return false;
        }
    }

    //integral types
    template<typename kT, typename eT>
    typename std::enable_if<std::is_integral<eT>::value, bool>::type
    addData(const kT &_attrname, const eT data)
    {
        cout << "Storing attribute " << typeid(eT).name() << " of size " << sizeof(data) << endl;

        string attrname = _stringify(_attrname);

        try
        {
            Attribute attr(m_group->createAttribute(attrname, CToPredType<eT>::type(), H5S_SCALAR));
            attr.write(CToPredType<eT>::type(), &data);
            m_allAttrKeys.push_back(attrname);
            return true;
        }
        catch(const H5::AttributeIException &)
        {
            return false;
        }
    }

    //std vectors
    template<typename kT, typename eT>
    bool addData(const kT &setname, const vector<eT> &data)
    {
        cout << "stong std vector of type " << typeid(eT).name() << " of size " << sizeof(eT) << endl;

        hsize_t dims[1] = {data.size()};
        return addData(setname, data.front(), 1, dims);
    }

    //Armadillo objects can be disabled.
#ifndef HDF5_NO_ARMA
    template<typename kT, typename eT>
    bool addData(const kT &setname, const Col<eT> &data)
    {
        cout << "Storing arma colvec of type " << typeid(eT).name() << " of size " << sizeof(data) << endl;

        hsize_t dims[1] = {data.n_elem};
        return addData(setname, data.memptr(), 1, dims);
    }

    template<typename kT, typename eT>
    bool addData(const kT &setname, const Row<eT> &data)
    {
        cout << "Storing arma rowvec of type " << typeid(eT).name() << " of size " << sizeof(data) << endl;

        hsize_t dims[1] = {data.n_elem};
        return addData(setname, data.memptr(), 1, dims);
    }

    template<typename kT, typename eT>
    bool addData(const kT &setname, const Mat<eT> &data)
    {
        cout << "Storing arma matrix of type " << typeid(eT).name() << " of size " << sizeof(data) << endl;

        hsize_t dims[2] = {data.n_rows, data.n_cols};
        return addData(setname, data.memptr(), 2, dims);
    }

    template<typename kT, typename eT>
    bool addData(const kT &setname, const Cube<eT> &data)
    {
        cout << "Storing arma cube of type " << typeid(eT).name() << " of size " << sizeof(data) << endl;

        hsize_t dims[3] = {data.n_slices, data.n_rows, data.n_cols};
        return addData(setname, data.memptr(), 3, dims);
    }

#endif

private:

    const Member *m_parent;

    const string m_ID;

    map<string, Member*> m_members;

    vector<string> m_allSetKeys;
    vector<string> m_allAttrKeys;

    template<typename T>
    static string _stringify(const T &nonString)
    {
        std::stringstream s;

        s << nonString;

        return s.str();
    }

    static string _stringify(const string &string)
    {
        return string;
    }

    void _dumpKeysToGroup()
    {
        addData("SetKeys", m_allSetKeys);
        addData("AttrKeys", m_allAttrKeys);
        addData("GroupKeys", _memberKeys());
    }

    vector<string> _memberKeys() const
    {
        vector<string> memberKeys(m_members.size());

        uint i = 0;
        for (const auto &member : m_members)
        {
            memberKeys.at(i) = member.first;
        }

        return memberKeys;
    }

    bool _notStorable(const void *buffer, uint rank, hsize_t dims[]) const
    {
        //Zero rank, null buffer, or either dimension zero is not storable.

        bool notStorable = (buffer == NULL) || (rank == 0);

        for (uint i = 0; i < rank; ++i)
        {
            notStorable = notStorable || (dims[i] == 0);
        }

        if (notStorable)
        {
            cout << "not storable." << endl;
        }

        return notStorable;
    }

protected:

    H5File *m_file;
    Group *m_group;

};

}