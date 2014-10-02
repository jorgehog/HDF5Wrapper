#include <sys/time.h>
#include <iostream>

#include <armadillo>

#include <hdf5wrapper.h>

using namespace H5Wrapper;
using namespace arma;
using namespace std;


template<typename T>
void saveMe(Member &member, string id, T _def)
{
    T value = _def;
    cout << "dumping " << id << " : " << value << endl;
    member.addData(id, value);
}


template<typename T>
T readMe(Member &member, string id)
{
    T value;

    cout << member.group()->attrExists(id) << endl;

    Attribute attr = member.group()->openAttribute(id);

    cout << "read " << id << " : ";

    attr.read(CToPredType<T>::type(), &value);

    cout << value << endl;

    return value;

}

template<>
string readMe<string>(Member &member, string id)
{
    cout << "STRING LOLS" << endl;
}

#define DUMPTYPE(type, root) saveMe<type>(root, #type, 2)
#define READTYPE(type, root) readMe<type>(root, #type)

int main()
{
    Root root("testfile.h5");

    srand(time(NULL));

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
    saveMe(root, "string", "testing12");


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

    return 0;
}
