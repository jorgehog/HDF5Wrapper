#pragma once

#include <H5Cpp.h>
#include <string>

#include <BADAss/badass.h>


#include <iostream>
using std::cout;
using std::endl;



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
        cout << "getting type " << #CTYPE << endl; \
        return HDF5TYPE; \
    } \
}; \
}

#define TYPECONVERTSUITE(CTYPE, HDF5TYPE) \
    PREDCONVERT(CTYPE, HDF5TYPE) \
    PREDCONVERT(const CTYPE, HDF5TYPE) \
    PREDCONVERT(CTYPE*, HDF5TYPE) \
    PREDCONVERT(const CTYPE*, HDF5TYPE)

namespace H5Wrapper
{

template<class T>
struct CToPredType
{
   static const AtomType type()
   {
       BADAssBreak("Unsupported type.", [] ()
       {
            std::cerr << "Tried to store datatype " << typeid(T).name() << std::endl;
       });

       return PredType::NATIVE_B8;
   }
};



}

PREDCONVERT(const char**,            PredType::NATIVE_B64)
TYPECONVERTSUITE(char,               PredType::NATIVE_CHAR)
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


//all H* types are redefinitions of C-types.

//TYPECONVERTSUITE(, PredType::NATIVE_OPAQUE);
//TYPECONVERTSUITE(, PredType::NATIVE_B16);
//TYPECONVERTSUITE(, PredType::NATIVE_B32);
//TYPECONVERTSUITE(, PredType::NATIVE_B64);
//TYPECONVERTSUITE(schar, PredType::NATIVE_SCHAR);
//TYPECONVERTSUITE(, PredType::NATIVE_UCHAR);

