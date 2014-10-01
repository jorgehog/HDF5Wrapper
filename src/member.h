#pragma once

#include <H5Cpp.h>

#include <BADAss/badass.h>

#include <string>
#include <sys/types.h>
#include <sstream>
#include <map>
#include <set>

#ifndef HDF5_NO_ARMA
#include <armadillo>
using namespace arma;
#endif

#ifdef H5_USE_MPI
#include <boost/mpi.hpp>
namespace mpi = boost::mpi;
#ifndef H5_MPI_MASTER
#define H5_MPI_MASTER 0
#endif
#endif

using std::string;
using std::map;
using std::vector;
using std::set;
using std::pair;
using std::size_t;

using H5::H5File;
using H5::Group;
using H5::Attribute;
using H5::DataSet;
using H5::DataSpace;
using H5::CompType;

#include "ctopredtype.h"

namespace H5Wrapper
{

class Member
{
public:

    Member(const Member *parent, const string ID);

    virtual ~Member();

    void finalize();

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

    const Group *group() const
    {
        return m_group;
    }

    const string absoluteName() const
    {
        if (isRoot())
        {
            return "/";
        }

        return m_parent->absoluteName() + m_ID + "/";
    }

    void purge();

    template<typename kT>
    void removeData(const kT &_key)
    {
        string key = _stringify(_key);

        BADAssBool(m_group->attrExists(key) || hasSet(key), "Data doesn't exist.");

        if (m_group->attrExists(key))
        {
            m_group->removeAttr(key);
            m_allAttrKeys.erase(key);
        }

        if (hasSet(key))
        {
            m_group->unlink(key);
            m_allSetKeys.erase(key);
        }

    }

    template<typename kT>
    bool hasMember(const kT &_key) const
    {
        string key = _stringify(_key);

        try
        {
            m_file->openGroup(absoluteName() + key);
            return true;
        }

        catch(const H5::FileIException &)
        {
            return false;
        }
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
    Member &addMember(const kT &_key, bool overWrite = false)
    {
        string key = _stringify(_key);

        if (overWrite)
        {
            if (hasMember(key))
            {
                removeMember(key);
            }
        }

        else
        {
//            BADAssBool(hasMember(key), "Key already exists. Did you mean to overwrite?");
        }

        Member *newMember = new Member(this, key);

        m_members[key] = newMember;

        return *newMember;
    }

    template<typename kT>
    void removeMember(const kT &_key)
    {

        string key = _stringify(_key);

        BADAssBool(hasMember(key), "Member not found: " + key);


        Member *member;

        if (m_members.find(key) == m_members.end())
        {
            member = new Member(this, key);
        }
        else
        {
            member = m_members[key];
        }

        member->purge();

        delete member;

        m_members.erase(key);
    }

    void removeMember(Member *member)
    {
        removeMember(member->ID());
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
    addData(const kT &_setname, const eT &data, const vector<size_t> &dims, bool overWrite = false)
    {
        if (_notStorable(data, dims))
        {
            return false;
        }

        string setname = _stringify(_setname);
        const uint rank = dims.size();

        hsize_t _dims[rank];
        for (uint i = 0; i < rank; ++i)
        {
            _dims[i] = dims.at(i);
        }

        try
        {
            DataSet dataset(m_file->createDataSet(absoluteName() + setname, CToPredType<eT>::type(), DataSpace(rank, _dims)));
            dataset.write(data, CToPredType<eT>::type());
            m_allSetKeys.insert(setname);
            return true;
        }
        catch (const H5::FileIException &)
        {
            if (overWrite)
            {
                removeData(_setname);
                return addData(_setname, data, dims, false);
            }

            m_allSetKeys.insert(setname);
            return false;
        }

    }

    //Static object.
    template<typename kT, typename eT>
    typename std::enable_if<!std::is_pointer<eT>::value && !std::is_integral<eT>::value, bool>::type
    addData(const kT &setname, const eT &data, const vector<size_t> &dims, bool overWrite = false)
    {
        return addData(setname, &data, dims, overWrite);
    }

    //single std::string
    template<typename kT>
    bool
    addData(const kT &_attrname, const string &data, bool overWrite = false)
    {
        string attrname = _stringify(_attrname);

        try
        {
            Attribute attr(m_group->createAttribute(attrname, CToPredType<char*>::type(data.size()), H5S_SCALAR));
            attr.write(CToPredType<char*>::type(data.size()), data.c_str());
            m_allAttrKeys.insert(attrname);
            return true;
        }
        catch(const H5::AttributeIException &)
        {
            if (overWrite)
            {
                removeData(_attrname);
                return addData(_attrname, data, false);
            }

            m_allAttrKeys.insert(attrname);
            return false;
        }
    }

    //vector of std::strings
    template<typename kT>
    bool
    addData(const kT &_setname, const string *data, const vector<size_t> &dims, bool overWrite = false)
    {
        if (_notStorable(data, dims))
        {
            return false;
        }

        string setname = _stringify(_setname);

        uint L = dims.at(0);
        string fullString("");

        string s;
        uint size = 0;
        for (uint k = 0; k < L; ++k)
        {
            s = *(data + k);
            size += s.size();
        }

        CompType stringVecType(size*sizeof(char*));

        uint offset = 0;
        uint i = 0;

        for (uint k = 0; k < L; ++k)
        {
            s = *(data + k);

            fullString += s;

            stringVecType.insertMember(_stringify(i), offset, CToPredType<char*>::type(s.size()));

            i++;;

            offset += s.size();
        }

        hsize_t newDims[] = {1};

        try
        {
            DataSet dataset(m_file->createDataSet(absoluteName() + setname, stringVecType, DataSpace(1, newDims)));
            dataset.write(fullString.c_str(), stringVecType);
            m_allSetKeys.insert(setname);
            return true;
        }
        catch (const H5::FileIException &)
        {
            if (overWrite)
            {
                removeData(_setname);
                return addData(_setname, data, dims, false);
            }

            m_allSetKeys.insert(setname);
            return false;
        }

    }


    //integral types
    template<typename kT, typename eT>
    typename std::enable_if<std::is_arithmetic<eT>::value, bool>::type
    addData(const kT &_attrname, const eT &data, bool overWrite = false)
    {
        string attrname = _stringify(_attrname);

        try
        {
            Attribute attr(m_group->createAttribute(attrname, CToPredType<eT>::type(), H5S_SCALAR));
            attr.write(CToPredType<eT>::type(), &data);
            m_allAttrKeys.insert(attrname);
            return true;
        }
        catch(const H5::AttributeIException &)
        {
            if (overWrite)
            {
                removeData(_attrname);
                return addData(_attrname, data, false);
            }

            m_allAttrKeys.insert(attrname);
            return false;
        }
    }

    //std vectors
    template<typename kT, typename eT>
    bool addData(const kT &setname, const vector<eT> &data, bool overWrite = false)
    {
        return addData(setname, data.front(), {data.size()}, overWrite);
    }

    //std sets
    template<typename kT, typename eT>
    bool addData(const kT &setname, const set<eT> &data, bool overWrite = false)
    {
        vector<eT> vectorOfSet(data.size());

        uint i = 0;
        for (const eT &element : data)
        {
            vectorOfSet.at(i) = element;
            i++;
        }

        return addData(setname, vectorOfSet.front(), {data.size()}, overWrite);
    }


    //Armadillo objects can be disabled.
#ifndef HDF5_NO_ARMA
    template<typename kT, typename eT>
    bool addData(const kT &setname, const Col<eT> &data, bool overWrite = false)
    {
        return addData(setname, data.memptr(), {data.n_elem}, overWrite);
    }

    template<typename kT, typename eT>
    bool addData(const kT &setname, const Row<eT> &data, bool overWrite = false)
    {
        return addData(setname, data.memptr(), {data.n_elem}, overWrite);
    }

    template<typename kT, typename eT>
    bool addData(const kT &setname, const Mat<eT> &data, bool overWrite = false)
    {
        return addData(setname, data.memptr(),{data.n_cols, data.n_rows}, overWrite);
    }

    template<typename kT, typename eT>
    bool addData(const kT &setname, const Cube<eT> &data, bool overWrite = false)
    {
        return addData(setname, data.memptr(), {data.n_slices, data.n_rows, data.n_cols}, overWrite);
    }

#endif

private:

    const Member *m_parent;

    const string m_ID;

    map<string, Member*> m_members;

    set<string> m_allSetKeys;
    set<string> m_allAttrKeys;

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

//    void _dumpKeysToGroup()
//    {
//        addData("SetKeys", m_allSetKeys);
//        addData("AttrKeys", m_allAttrKeys);
//        addData("GroupKeys", _memberKeys());
//    }

    vector<string> _memberKeys() const;

    bool _notStorable(const void *buffer, const vector<size_t> &dims) const;

protected:

    H5File *m_file;
    Group *m_group;

//    static const mpi::environment env;
//    static const mpi::communicator world;

    void _loadFromFile()
    {
        //        if (hasSet("AttrKeys"))
        //        {
        //            DataSet AttrKeys = m_file->openDataSet(absoluteName() + "AttrKeys");
        //            CompType StringVecType = AttrKeys.getCompType();

        //            uint rank = 1;
        //            hsize_t dims[] = {1};

        //            char* inputBuffer;

        //            AttrKeys.read(inputBuffer, StringVecType);

        //            cout << inputBuffer << endl;



        //        }
    }
};

}
