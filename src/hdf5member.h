#pragma once


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


#include <iostream>
using std::cout;
using std::endl;

class HDF5Member
{
public:
    HDF5Member(const HDF5Member *parent, const string ID);

    virtual ~HDF5Member();


    bool isTopMember() const
    {
        return m_parent == NULL;
    }

    const string &ID() const
    {
        return m_ID;
    }

    template<typename T>
    bool hasMember(const T &_key) const
    {
        string key = _stringify(_key);

        return m_members.find(key) != m_members.end();
    }


    template<typename T1, typename T2>
    void registerMember(const T1 &_key, const T2 &data)
    {
        registerMember(_key);
        this->operator [](_key)->setData(data);
    }

    template<typename T>
    void registerMember(const T &_key)
    {
        string key = _stringify(_key);

        BADAssBool(!hasMember(key), "Key already exists.");

        m_members[key] = new HDF5Member(this, key);

    }

    template<typename T>
    void removeMember(const T &_key)
    {
        string key = _stringify(_key);

        BADAssBool(hasMember(key), "Member not found: " + key);


        delete m_members[key];

        m_members.erase(key);
    }

    template<typename T>
    const HDF5Member *operator [](const T &_key) const
    {
        string key = _stringify(_key);

        BADAssBool(hasMember(key), "Member not found: " + key);


        return (*m_members.find(key)).second;
    }

    template<typename T>
    HDF5Member *operator[](const T &_key)
    {
        string key = _stringify(_key);

        BADAssBool(hasMember(key), "Member not found: " + key);


        return m_members[key];
    }

    //Pointer is the main storage function. All others simply channel this one.
    template<typename T>
    typename std::enable_if<std::is_pointer<T>::value, void>::type
    setData(const T &data)
    {
        cout << "Storing pointer type " << typeid(T).name() << " of size " << sizeof(data) << endl;
    }


    //Static object.
    template<typename T>
    typename std::enable_if<!std::is_pointer<T>::value, void>::type
    setData(const T &data)
    {
        cout << "Storing " << typeid(T).name() << " of size " << sizeof(data) << endl;
        setData(&data);
    }

    //Armadillo objects can be disabled.
#ifndef HDF5_NO_ARMA
    template<typename T>
    void setData(const Col<T> &data)
    {
        cout << "Storing arma colvec of type " << typeid(T).name() << " of size " << sizeof(data) << endl;
        setData(data.memptr());
    }

    template<typename T>
    void setData(const Row<T> &data)
    {
        cout << "Storing arma rowvec of type " << typeid(T).name() << " of size " << sizeof(data) << endl;
        setData(data.memptr());
    }

    template<typename T>
    void setData(const Mat<T> &data)
    {
        cout << "Storing arma matrix of type " << typeid(T).name() << " of size " << sizeof(data) << endl;
        setData(data.memptr());
    }

    template<typename T>
    void setData(const Cube<T> &data)
    {
        cout << "Storing arma cube of type " << typeid(T).name() << " of size " << sizeof(data) << endl;
        setData(data.memptr());
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
};

