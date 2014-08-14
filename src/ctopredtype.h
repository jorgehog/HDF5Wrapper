#pragma once

#include <H5Cpp.h>
#include <string.h>


#include <iostream>
using std::cout;
using std::endl;


using H5::PredType;

template<class T>
struct CToPredType
{
   static const PredType type()
   {
//       static_assert(false, "Unsupported data type.");
       cout << "getting type fail.." << endl;
       return PredType::NATIVE_B8;
   }
};

template<>
struct CToPredType<int*>
{
   static const PredType type()
   {
       cout << "getting type int" << endl;
       return PredType::NATIVE_INT;
   }
};

template<>
struct CToPredType<const int*>
{
   static const PredType type()
   {
       cout << "getting type int" << endl;
       return PredType::NATIVE_INT;
   }
};

template<>
struct CToPredType<uint*>
{
   static const PredType type()
   {
       cout << "getting type uint" << endl;
       return PredType::NATIVE_UINT;
   }
};

template<>
struct CToPredType<const uint*>
{
   static const PredType type()
   {
       cout << "getting type uint" << endl;
       return PredType::NATIVE_UINT;
   }
};

template<>
struct CToPredType<double*>
{
   static const PredType type()
   {
       cout << "getting type double" << endl;
       return PredType::NATIVE_DOUBLE;
   }
};

template<>
struct CToPredType<const double*>
{
   static const PredType type()
   {
       cout << "getting type double" << endl;
       return PredType::NATIVE_DOUBLE;
   }
};
