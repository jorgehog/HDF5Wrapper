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
using std::pair;

using H5::H5File;
using H5::Group;
using H5::DataSet;
using H5::DataSpace;



#include <iostream>
using std::cout;
using std::endl;

#include "ctopredtype.h"

class HDF5Member
{
public:

    HDF5Member(const HDF5Member *parent, const string ID) :
        m_parent(parent),
        m_ID(ID),
        m_file(isTopMember() ? NULL : parent->file())
    {

        if (!isTopMember())
        {
            m_group = new Group(m_file->createGroup(getFullGroup()));
        }

    }

    virtual ~HDF5Member()
    {
        for (auto &member : m_members)
        {
            delete member.second;
        }

        m_members.clear();

        delete m_group;
    }

    bool isTopMember() const
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
        if (isTopMember())
        {
            return "/";
        }

        return m_parent->getFullGroup() + m_ID + "/";
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

        return m_group->attrExists(key);
    }

    template<typename kT>
    HDF5Member *registerMember(const kT &_key)
    {
        string key = _stringify(_key);

        BADAssBool(!hasMember(key), "Key already exists.");

        HDF5Member newMember = new HDF5Member(this, key);

        m_members[key] = newMember;

        return newMember;
    }

    template<typename kT>
    void removeMember(const kT &_key)
    {
        string key = _stringify(_key);

        BADAssBool(hasMember(key), "Member not found: " + key);

        delete m_members[key];

        m_members.erase(key);
    }

    template<typename kT>
    const HDF5Member *operator [](const kT &_key) const
    {
        string key = _stringify(_key);

        BADAssBool(hasMember(key), "Member not found: " + key);


        return (*m_members.find(key)).second;
    }

    template<typename kT>
    HDF5Member *operator[](const kT &_key)
    {
        string key = _stringify(_key);

        BADAssBool(hasMember(key), "Member not found: " + key);


        return m_members[key];
    }

    //Pointer is the main storage function. All others simply channel this one.
    template<typename kT, typename eT>
    typename std::enable_if<std::is_pointer<eT>::value, bool>::type
    setData(const kT &setname, const eT &data, const uint rank, hsize_t dims[])
    {
        cout << "Storing pointer type " << typeid(eT).name() << " of size " << sizeof(data) << endl;

        try
        {
            DataSet dataset(m_file->createDataSet(getFullGroup() + _stringify(setname), CToPredType<eT>::type(), DataSpace(rank, dims)));
            dataset.write(data, CToPredType<eT>::type());
            return true;
        }
        catch (const H5::FileIException &)
        {
            return false;
        }

    }


    //Static object.
    template<typename kT, typename eT>
    typename std::enable_if<!std::is_pointer<eT>::value, bool>::type
    setData(const kT &setname, const eT &data, const uint rank, hsize_t dims[])
    {
        cout << "Storing " << typeid(eT).name() << " of size " << sizeof(data) << endl;
        return setData(setname, &data, rank, dims);
    }

    //Armadillo objects can be disabled.
#ifndef HDF5_NO_ARMA
    template<typename kT, typename eT>
    bool setData(const kT &setname, const Col<eT> &data)
    {
        cout << "Storing arma colvec of type " << typeid(eT).name() << " of size " << sizeof(data) << endl;

        hsize_t dims[1] = {data.n_elem};
        return setData(setname, data.memptr(), 1, dims);
    }

    template<typename kT, typename eT>
    bool setData(const kT &setname, const Row<eT> &data)
    {
        cout << "Storing arma rowvec of type " << typeid(eT).name() << " of size " << sizeof(data) << endl;

        hsize_t dims[1] = {data.n_elem};
        return setData(setname, data.memptr(), 1, dims);
    }

    template<typename kT, typename eT>
    bool setData(const kT &setname, const Mat<eT> &data)
    {
        cout << "Storing arma matrix of type " << typeid(eT).name() << " of size " << sizeof(data) << endl;

        hsize_t dims[2] = {data.n_rows, data.n_cols};
        return setData(setname, data.memptr(), 2, dims);
    }

    template<typename kT, typename eT>
    bool setData(const kT &setname, const Cube<eT> &data)
    {
        cout << "Storing arma cube of type " << typeid(eT).name() << " of size " << sizeof(data) << endl;

        hsize_t dims[3] = {data.n_slices, data.n_rows, data.n_cols};
        return setData(setname, data.memptr(), 3, dims);
    }

#endif

private:

    const HDF5Member *m_parent;

    const string m_ID;


    map<string, HDF5Member*> m_members;

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

protected:

    H5File *m_file;
    Group *m_group;

};




















