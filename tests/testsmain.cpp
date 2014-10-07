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
    member.addData(id, value, true);
}

template<typename T>
T readMe(Member &member, string id)
{
    T value;

    member.readData(id, &value);

    if (value != _defvalue<T>::value())
    {
        cerr << value << " != " << _defvalue<T>::value() << endl;
    }

    return value;

}


template<typename T>
void saveVectorMe(Member &member, string id, vector<T> value)
{
    id += "vec";

    member.addData(id, value, true);
}

template<typename T>
vector<T> readVectorMe(Member &member, string id)
{
    id += "vec";

    vector<T> value;

    member.readData(id, value);

    vector<T> defs = _defvalue<T>::vectorValues();

    for (uint i = 0; i < defs.size(); ++i)
    {
        if (defs.at(i) != value.at(i))
        {
            cerr << id << " " << i << " " << defs.at(i) << " != " << value.at(i) << endl;
        }
    }

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

    root.flush();

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

    root.flush();

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
    READVECTORTYPE(string, root);

    //TEST: static write + read;
    //TEST: Armadillo write + read
    //TEST: member add
    //TEST: all types remove.
    //TEST: member purge.

    return 0;
}
