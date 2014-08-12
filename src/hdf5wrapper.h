#pragma once

#include <H5Cpp.h>

#include <sys/types.h>
#include <sstream>


using H5::H5File;
using std::string;

class HDF5Wrapper
{
public:

    HDF5Wrapper(const string filename);

    virtual ~HDF5Wrapper();

    void initialize()
    {
        initialize(H5F_ACC_RDWR);
    }

    void initialize(const uint flag);

    template<typename T>
    bool hasMember(const T &_key)
    {
        string key = _stringify(_key);

        return false;
    }

    template<typename T>
    void registerMember(const T &_key)
    {
        string key = _stringify(_key);



    }


private:

    const string m_filename;

    H5File *m_file;

    template<typename T>
    static string _stringify(const T &&nonString)
    {
        std::stringstream s;

        s << nonString;

        return s.str();
    }

    static string _stringify(const string &&string)
    {
        return string;
    }

};
