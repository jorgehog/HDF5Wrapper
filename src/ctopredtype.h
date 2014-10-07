#pragma once

#include <H5Cpp.h>
#include <string>

#include <BADAss/badass.h>


using H5::AtomType;
using H5::PredType;
using H5::StrType;


#define PREDCONVERT(CTYPE, HDF5TYPE) \
namespace H5Wrapper \
{ \
template<> \
struct CToPredType<CTYPE> \
{ \
    static const AtomType type() \
    { \
        return HDF5TYPE; \
    } \
}; \
}

#define TYPECONVERTSUITE(CTYPE, HDF5TYPE) \
    PREDCONVERT(CTYPE, HDF5TYPE) \
    PREDCONVERT(const CTYPE, HDF5TYPE) \
    PREDCONVERT(CTYPE*, HDF5TYPE) \
    PREDCONVERT(const CTYPE*, HDF5TYPE)

#define WEIRD_TEST(ID, type, B) if (ID == string(typeid(type).name()).back()) return B

namespace H5Wrapper
{

template<typename T>
struct CPredTypeRef
{
    static const T* ref(const T& _obj)
    {
        return &_obj;
    }

    static T* ptr(T* _obj)
    {
        return _obj;
    }
};

template<>
struct CPredTypeRef<string>
{
    static const string& ref(const string& _obj)
    {
        return _obj;
    }

    static string& ptr(string* _obj)
    {
        return *_obj;
    }
};

template<typename T>
struct CToPredType
{
   static const AtomType type()
   {
       using std::string;

       string s = typeid(T).name();
       char typeID = s.back();

       WEIRD_TEST(typeID, short,              PredType::NATIVE_SHORT);
       WEIRD_TEST(typeID, unsigned short,     PredType::NATIVE_USHORT);
       WEIRD_TEST(typeID, int,                PredType::NATIVE_INT);
       WEIRD_TEST(typeID, uint,               PredType::NATIVE_UINT);
       WEIRD_TEST(typeID, long,               PredType::NATIVE_LONG);
       WEIRD_TEST(typeID, unsigned long,      PredType::NATIVE_ULONG);
       WEIRD_TEST(typeID, long long,          PredType::NATIVE_LLONG);
       WEIRD_TEST(typeID, unsigned long long, PredType::NATIVE_ULLONG);
       WEIRD_TEST(typeID, float,              PredType::NATIVE_FLOAT);
       WEIRD_TEST(typeID, double,             PredType::NATIVE_DOUBLE);
       WEIRD_TEST(typeID, long double,        PredType::NATIVE_LDOUBLE);
       WEIRD_TEST(typeID, bool,               PredType::NATIVE_B8);

       cerr << "Unable to deduce type " << typeid(T).name() << endl;

       return PredType::NATIVE_CHAR;
   }
};

template<>
struct CToPredType<char*>
{
   static const AtomType type(const uint size)
   {
       return StrType(PredType::C_S1, size);
   }
};


}

TYPECONVERTSUITE(short,              PredType::NATIVE_SHORT)
TYPECONVERTSUITE(unsigned short,     PredType::NATIVE_USHORT)
TYPECONVERTSUITE(int,                PredType::NATIVE_INT)
TYPECONVERTSUITE(uint,               PredType::NATIVE_UINT)
TYPECONVERTSUITE(long,               PredType::NATIVE_LONG)
TYPECONVERTSUITE(unsigned long,      PredType::NATIVE_ULONG)
TYPECONVERTSUITE(long long,          PredType::NATIVE_LLONG)
TYPECONVERTSUITE(unsigned long long, PredType::NATIVE_ULLONG)
TYPECONVERTSUITE(float,              PredType::NATIVE_FLOAT)
TYPECONVERTSUITE(double,             PredType::NATIVE_DOUBLE)
TYPECONVERTSUITE(long double,        PredType::NATIVE_LDOUBLE)
TYPECONVERTSUITE(bool,               PredType::NATIVE_B8)
TYPECONVERTSUITE(std::string,        StrType(PredType::C_S1, 256))

//all H* types are redefinitions of C-types.

//TYPECONVERTSUITE(, PredType::NATIVE_OPAQUE);
//TYPECONVERTSUITE(, PredType::NATIVE_B16);
//TYPECONVERTSUITE(, PredType::NATIVE_B32);
//TYPECONVERTSUITE(, PredType::NATIVE_B64);
//TYPECONVERTSUITE(schar, PredType::NATIVE_SCHAR);
//TYPECONVERTSUITE(, PredType::NATIVE_UCHAR);

