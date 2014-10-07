#include <sys/time.h>
#include <iostream>

#include <armadillo>

#include <hdf5wrapper.h>


using namespace H5Wrapper;
using namespace arma;
using namespace std;


template<typename T>
struct _defvalue
{
    static const T value()
    {
        return 2.0;
    }

    static const vector<T> vectorValues()
    {
        return {1, 2, 3};
    }
};

template<>
struct _defvalue<string>
{

    static const string value()
    {
        return string("heyhey");
    }

    static const vector<string> vectorValues()
    {
        return {"one", "two", "three"};
    }
};

template<typename T>
void saveMe(Member &member, string id, T value)
{
    cout << "dumping " << id << " : " << value << endl;
    member.addData(id, value, true);
}

template<typename T>
T readMe(Member &member, string id)
{
    T value;


    cout << "read " << id << " : ";

    member.readData(id, &value);

    cout << value << endl;

    return value;

}


template<typename T>
void saveVectorMe(Member &member, string id, vector<T> value)
{
    id += "vec";

    cout << "dumping " << id << " : ";
    cout << value.at(0) << " " << value.at(1) << " " << value.at(2) << " " << endl;

    member.addData(id, value, true);
}

template<typename T>
vector<T> readVectorMe(Member &member, string id)
{
    id += "vec";

    cout << "read " << id << " : ";

    vector<T> value;

    member.readData(id, value);

    cout << value.at(0) << "   " << value.at(1) << "   " << value.at(2) << endl;

    return value;

}

#define DUMPTYPE(type, root) saveMe<type>(root, #type, _defvalue<type>::value())
#define READTYPE(type, root) readMe<type>(root, #type)

#define DUMPVECTORTYPE(type, root) saveVectorMe<type>(root, #type, _defvalue<type>::vectorValues())
#define READVECTORTYPE(type, root) readVectorMe<type>(root, #type)

int main()
{
    Root root("testfile.h5");

    srand(time(NULL));

    //Scalars

    DUMPTYPE(short, root);
    DUMPTYPE(unsigned short, root);
    DUMPTYPE(int, root);
    DUMPTYPE(uint, root);
    DUMPTYPE(long, root);
    DUMPTYPE(unsigned long, root);
    DUMPTYPE(long long, root);
    DUMPTYPE(unsigned long long, root);
    DUMPTYPE(float, root);
    DUMPTYPE(double, root);
    DUMPTYPE(long double, root);
    DUMPTYPE(bool, root);
    DUMPTYPE(string, root);


    READTYPE(short, root);
    READTYPE(unsigned short, root);
    READTYPE(int, root);
    READTYPE(uint, root);
    READTYPE(long, root);
    READTYPE(unsigned long, root);
    READTYPE(long long, root);
    READTYPE(unsigned long long, root);
    READTYPE(float, root);
    READTYPE(double, root);
    READTYPE(long double, root);
    READTYPE(bool, root);
    READTYPE(string, root);

    //Vectors

    DUMPVECTORTYPE(short, root);
    DUMPVECTORTYPE(unsigned short, root);
    DUMPVECTORTYPE(int, root);
    DUMPVECTORTYPE(uint, root);
    DUMPVECTORTYPE(long, root);
    DUMPVECTORTYPE(unsigned long, root);
    DUMPVECTORTYPE(long long, root);
    DUMPVECTORTYPE(unsigned long long, root);
    DUMPVECTORTYPE(float, root);
    DUMPVECTORTYPE(double, root);
    DUMPVECTORTYPE(long double, root);
    DUMPVECTORTYPE(bool, root);
    DUMPVECTORTYPE(string, root);


    H5Fflush(root.group()->getId(), H5F_SCOPE_GLOBAL);

    READVECTORTYPE(short, root);
    READVECTORTYPE(unsigned short, root);
    READVECTORTYPE(int, root);
    READVECTORTYPE(uint, root);
    READVECTORTYPE(long, root);
    READVECTORTYPE(unsigned long, root);
    READVECTORTYPE(long long, root);
    READVECTORTYPE(unsigned long long, root);
    READVECTORTYPE(float, root);
    READVECTORTYPE(double, root);
    READVECTORTYPE(long double, root);
    READVECTORTYPE(bool, root);
    READVECTORTYPE(string, root); // THIS IS BROKEN ATM

    //ITERATE: ftp://www.hdfgroup.org/HDF5/examples/misc-examples/h5grpnam.cpp


    return 0;
}
