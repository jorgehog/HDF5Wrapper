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

    bool isRoot() const
    {
        return m_parent == nullptr;
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

    void flush();

    void purge();

    template<typename kT>
    void removeData(const kT &_key)
    {
        string key = _stringify(_key);

        BADAssBool(m_group->attrExists(key) || hasDataset(key), "Data doesn't exist.");

        if (hasAttribute(key))
        {
            m_group->removeAttr(key);
            m_attributes.erase(key);
        }

        if (hasDataset(key))
        {
            m_group->unlink(key);
            m_datasets.erase(key);
        }

    }

    template<typename kT>
    bool hasMember(const kT &_key) const
    {
        string key = _stringify(_key);

        return m_members.find(key) != m_members.end();
    }

    template<typename kT>
    bool hasDataset(const kT &_key) const
    {
        string key = _stringify(_key);

        return m_datasets.find(key) != m_datasets.end();
    }

    template<typename kT>
    bool hasAttribute(const kT &_key) const
    {
        string key = _stringify(_key);

        return m_attributes.find(key) != m_attributes.end();
    }

    template<typename kT>
    Member &addMember(const kT &_key)
    {
        string key = _stringify(_key);

        if (hasMember(key))
        {
            return *m_members[key];
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


        Member *member = m_members[key];

        member->purge();

        m_group->unlink(key);

        delete member;
        member = nullptr;

        m_members.erase(key);
    }

    void removeMember(Member *member)
    {
        removeMember(member->ID());
    }

    void removeMember(Member &member)
    {
        removeMember(&member);
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

    template<typename kT, typename eT>
    void
    readData(const kT &_setname, eT *data)
    {
        string setname = _stringify(_setname);

        if (group()->attrExists(setname))
        {
            _readAttr(setname, data);
        }

        else if (hasDataset(setname))
        {
            _readDataSet(setname, data);
        }
        else
        {
            BADAssBreak("No such data.");
        }

    }

    template<typename kT>
    vector<hsize_t> getDims(const kT &_setname)
    {
        string setname = _stringify(_setname);

        const DataSet &dataset = group()->openDataSet(setname);

        const DataSpace &dataspace = dataset.getSpace();

        hsize_t rank = dataspace.getSimpleExtentNdims();

        hsize_t dims[rank];

        dataspace.getSimpleExtentDims(dims);

        vector<hsize_t> _dims(rank);

        for (uint i = 0; i < rank; ++i)
        {
            _dims[i] = dims[i];
        }

        return _dims;

    }

    template<typename kT, typename eT>
    void
    readData(const kT &_setname, vector<eT> &data)
    {
        auto dims = getDims(_setname);

        uint size = dims[0];
        eT mem[size];
        data.resize(size);

        readData(_setname, &mem[0]);

        for (uint i = 0; i < size; ++i)
        {
            data.at(i) = mem[i];
        }
    }

    template<typename kT>
    void
    readData(const kT &_setname, vector<string> &data)
    {

        string setname = _stringify(_setname);

        const DataSet &dataset = group()->openDataSet(setname);
        CompType stringVecType = dataset.getCompType();

        uint totalSize = stringVecType.getSize();
        uint nStrings = stringVecType.getNmembers();

        data.resize(nStrings);

        char mem[totalSize];

        dataset.read(mem, stringVecType);

        string fullstring = mem;

        uint offset = 0;
        for (uint i = 0; i < nStrings; ++i)
        {
            H5::StrType stringType = stringVecType.getMemberStrType(i);

            uint subStringSize = stringType.getSize();

            string substr = fullstring.substr(offset, subStringSize);

            data.at(i) = substr;

            offset += subStringSize;
        }
    }


    template<typename eT>
    void
    _readAttr(const string setname, eT *data)
    {
        const Attribute &attr = group()->openAttribute(setname);
        attr.read(CToPredType<eT>::type(), CPredTypeRef<eT>::ptr(data));
    }

    template<typename eT>
    void
    _readDataSet(const string setname, eT *data)
    {
        const DataSet &dataset = group()->openDataSet(setname);

        dataset.read(CPredTypeRef<eT>::ptr(data), dataset.getCompType());

    }

    //Pointer is the main storage function. All others simply channel this one.
    template<typename kT, typename eT>
    typename std::enable_if<std::is_pointer<eT>::value, bool>::type
    addData(const kT &_setname, const eT &data, const vector<size_t> &dims, bool overWrite = true)
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

            m_datasets.insert(setname);
            return true;
        }
        catch (const H5::FileIException &)
        {
            if (overWrite)
            {
                removeData(_setname);
                return addData(_setname, data, dims, false);
            }

            m_datasets.insert(setname);
            return false;
        }

    }

    //Static object.
    template<typename kT, typename eT>
    typename std::enable_if<!std::is_pointer<eT>::value, bool>::type
    addData(const kT &setname, const eT &data, const vector<size_t> &dims, bool overWrite = true)
    {
        return addData(setname, &data, dims, overWrite);
    }

    //vector of std::strings
    template<typename kT>
    bool
    addData(const kT &_setname, const string *data, const vector<size_t> &dims, bool overWrite = true)
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
            m_datasets.insert(setname);
            return true;
        }
        catch (const H5::FileIException &)
        {
            if (overWrite)
            {
                removeData(_setname);
                return addData(_setname, data, dims, false);
            }

            m_datasets.insert(setname);
            return false;
        }

    }

    //attributes
    template<typename kT, typename eT>
    bool
    addData(const kT &_attrname, const eT &data, bool overWrite = true)
    {
        string attrname = _stringify(_attrname);

        try
        {
            Attribute attr(m_group->createAttribute(attrname, CToPredType<eT>::type(), H5S_SCALAR));
            attr.write(CToPredType<eT>::type(), CPredTypeRef<eT>::ref(data));
            m_attributes.insert(attrname);
            return true;
        }
        catch(const H5::AttributeIException &)
        {
            if (overWrite)
            {
                removeData(_attrname);
                return addData(_attrname, data, false);
            }

            m_attributes.insert(attrname);
            return false;
        }
    }

    //std vectors
    template<typename kT, typename eT>
    bool addData(const kT &setname, const vector<eT> &data, bool overWrite = true)
    {
        return addData(setname, data.front(), {data.size()}, overWrite);
    }

    //std sets
    template<typename kT, typename eT>
    bool addData(const kT &setname, const set<eT> &data, bool overWrite = true)
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
    bool addData(const kT &setname, const Col<eT> &data, bool overWrite = true)
    {
        return addData(setname, data.memptr(), {data.n_elem}, overWrite);
    }

    template<typename kT, typename eT>
    bool addData(const kT &setname, const Row<eT> &data, bool overWrite = true)
    {
        return addData(setname, data.memptr(), {data.n_elem}, overWrite);
    }

    template<typename kT, typename eT>
    bool addData(const kT &setname, const Mat<eT> &data, bool overWrite = true)
    {
        return addData(setname, data.memptr(),{data.n_cols, data.n_rows}, overWrite);
    }

    template<typename kT, typename eT>
    bool addData(const kT &setname, const Cube<eT> &data, bool overWrite = true)
    {
        return addData(setname, data.memptr(), {data.n_slices, data.n_rows, data.n_cols}, overWrite);
    }

    template<typename kT, typename eT>
    void
    readData(const kT &_setname, Col<eT> &data)
    {
       vector<hsize_t> dims = getDims(_setname);
       const uint &N = dims[0];

       eT* mem = new eT[N];

       readData(_setname, mem);

       data = Col<eT>(mem, N, false, true); //strict true to avoid memory leakage.
    }

    template<typename kT, typename eT>
    void
    readData(const kT &_setname, Row<eT> &data)
    {
        vector<hsize_t> dims = getDims(_setname);
        const uint &N = dims[0];

        eT* mem = new eT[N];

        readData(_setname, mem);

        data = Row<eT>(mem, N, false, true); //strict true to avoid memory leakage.
    }

    template<typename kT, typename eT>
    void
    readData(const kT &_setname, Mat<eT> &data)
    {
        vector<hsize_t> dims = getDims(_setname);

        const uint &NY = dims[0];
        const uint &NX = dims[1];

        eT* mem = new eT[NX*NY];

        readData(_setname, mem);

        data = Mat<eT>(mem, NX, NY, false, true); //strict true to avoid memory leakage.
    }

    template<typename kT, typename eT>
    void
    readData(const kT &_setname, Cube<eT> &data)
    {
        vector<hsize_t> dims = getDims(_setname);

        const uint &NZ = dims[0];
        const uint &NX = dims[1];
        const uint &NY = dims[2];

        eT* mem = new eT[NX*NY*NZ];

        readData(_setname, mem);

        data = Cube<eT>(mem, NX, NY, NZ, false, true); //strict true to avoid memory leakage.
    }

#endif

    void _iterate()
    {
        m_info.index++;
    }

    const int &iter_index() const
    {
        return m_info.index;
    }

    void _registerDataset(string name)
    {
        m_datasets.insert(name);
    }

    void _registerAttribute(string name)
    {
        m_attributes.insert(name);
    }

    const set<string> &datasets() const
    {
        return m_datasets;
    }

    const set<string> &attributes() const
    {
        return m_attributes;
    }

    const map<string, Member*> &members() const
    {
        return m_members;
    }

private:

    const Member *m_parent;

    const string m_ID;

    set<string> m_datasets;
    set<string> m_attributes;

    map<string, Member*> m_members;


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

    bool _notStorable(const void *buffer, const vector<size_t> &dims) const;



protected:

    H5File *m_file;
    Group *m_group;

    void _clearall();

    static herr_t _loadGroupAndDataset(hid_t loc_id, const char *name, void *opdata)
    {
        Member *member=(Member *)opdata;

        int obj_type = H5Gget_objtype_by_idx(loc_id, member->iter_index());

        if(obj_type == H5G_GROUP)
        {
            member->addMember(name);
        }
        else if (obj_type == H5G_DATASET)
        {
            member->_registerDataset(name);
        }
        else
        {
            cerr << "unsupported datatype " << name << " " << obj_type << endl;
        }

        member->_iterate();
        return 0;
    }

    static herr_t _loadAttr(hid_t location_id, const char *attr_name, const H5A_info_t *ainfo, void *op_dat)
    {
        (void) ainfo;
        (void) location_id;

        Member *member=(Member *)op_dat;

        member->_registerAttribute(attr_name);

        return 0;

    }

    struct iter_info
    {
        int index = 0;
    } m_info;

    void _loadFromFile()
    {
        m_file->iterateElems(absoluteName(), nullptr, Member::_loadGroupAndDataset, this);

        H5Aiterate(m_group->getId(), H5_INDEX_NAME, H5_ITER_NATIVE, nullptr, Member::_loadAttr, this);

//        cout << absoluteName() << endl;
//        cout << "datasets: ";
//        for (string s : datasets())
//        {
//            cout << s << " - ";
//        }
//        cout << endl;

//        cout << "Attributes: ";
//        for (string s : attributes())
//        {
//            cout << s << " - ";
//        }
//        cout << endl;

//        cout << "Members: ";
//        for (const auto & member : m_members)
//        {
//            cout << member.second->ID() << " - ";
//        }
//        cout << endl;

//        cout << "---------------------" << endl;

    }
};

}
