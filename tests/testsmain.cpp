#include <sys/time.h>
#include <iostream>

#include <armadillo>

#include <hdf5wrapper.h>


using namespace H5Wrapper;
using namespace arma;
using namespace std;

struct _randomvalues
{
    static double double_value;
    static int int_value;
};

namespace _rand
{

template<typename T>
typename std::enable_if<std::is_integral<T>::value, T>::type
value()
{
    return static_cast<T>(_randomvalues::int_value);
}

template<typename T>
typename std::enable_if<std::is_floating_point<T>::value, T>::type
value()
{
    return static_cast<T>(_randomvalues::double_value);
}

template<typename T>
typename std::enable_if<!std::is_arithmetic<T>::value, T>::type
value()
{
    return T();
}

}

double _randomvalues::double_value;
int _randomvalues::int_value;

template<typename T>
struct _defvalue
{
    static const T value()
    {
        return 2 + _rand::value<T>();
    }

    static const vector<T> vectorValues()
    {
        return {_rand::value<T>(), _rand::value<T>(), _rand::value<T>()};
    }

    static T* pointerVectorValues(const uint NX)
    {

        T* values = new T[NX];
        for (uint i = 0; i < NX; ++i)
        {
            values[i] = i + _rand::value<T>();
        }

        return values;

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

    static string* pointerVectorValues(const uint NX)
    {
        stringstream s;

        string* values = new string[NX];
        for (uint i = 0; i < NX; ++i)
        {
            s.str(string());

            s << i << "lala";
            values[i] = s.str();
        }

        return values;
    }
};

template<typename T>
void saveMe(Member &member, string id)
{
    member.addData(id, _defvalue<T>::value(), true);
}

template<typename T>
void readMe(Member &member, string id)
{
    T value;

    member.readData(id, &value);

    if (value != _defvalue<T>::value())
    {
        cerr << value << " != " << _defvalue<T>::value() << endl;
    }

}


template<typename T>
void saveVectorMe(Member &member, string id)
{
    id += "vec";

    member.addData(id, _defvalue<T>::vectorValues(), true);
}

template<typename T>
void readVectorMe(Member &member, string id)
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
}

template<typename T>
void saveStaticMe(Member &member, string id)
{
    id += "static";

    constexpr uint NX = 3;
    constexpr uint NY = 2;

    T value[NX][NY];

    for (uint i = 0; i < NX; ++i)
    {
        for (uint j = 0; j < NY; ++j)
        {
            value[i][j] = _defvalue<T>::value();
        }
    }

    member.addData(id, value, {NX, NY}, true);
}

template<typename T>
void readStaticMe(Member &member, string id)
{
    id += "static";

    constexpr uint NX = 3;
    constexpr uint NY = 2;

    T value[NX][NY];

    member.readData(id, &value);

    for (uint i = 0; i < NX; ++i)
    {
        for (uint j = 0; j < NY; ++j)
        {
            if (value[i][j] != _defvalue<T>::value())
            {
                cerr << id << " " << i << " " << _defvalue<T>::value() << " != " << value[i][j] << endl;
            }
        }
    }
}

template<typename T>
void savePointerMe(Member &member, string id)
{
    id += "pointer";

    constexpr uint NX = 4;

    T* values = _defvalue<T>::pointerVectorValues(NX);

    member.addData(id, values, {NX}, true);

    delete [] values;
}

template<typename T>
void readPointerMe(Member &member, string id)
{
    id += "pointer";

    constexpr uint NX = 4;

    T value[NX];
    T* defvalues = _defvalue<T>::pointerVectorValues(NX);

    member.readData(id, &value);

    for (uint i = 0; i < NX; ++i)
    {
        if (value[i] != defvalues[i])
        {
            cerr << id << " " << i << " " << defvalues[i] << " != " << value[i] << endl;
        }
    }

    delete [] defvalues;
}

template<typename T>
typename std::enable_if<is_supported_elem_type<T>::value, void>::type
saveArmaMe(Member &member, string id)
{
    string vecid = id + "armavec";
    string recid = id + "armarec";
    string matid = id + "armamat";
    string cubid = id + "armacub";

    constexpr uint NX = 4;
    constexpr uint NY = 3;
    constexpr uint NZ = 2;

    Col<T>  v(NX);
    Row<T>  r(NX);
    Mat<T>  m(NX, NY);
    Cube<T> c(NX, NY, NZ);

    v.fill(_defvalue<T>::value());
    r.fill(_defvalue<T>::value());
    m.fill(_defvalue<T>::value());
    c.fill(_defvalue<T>::value());

    member.addData(vecid, v, true);
    member.addData(recid, r, true);
    member.addData(matid, m, true);
    member.addData(cubid, c, true);
}

template<typename T>
typename std::enable_if<!is_supported_elem_type<T>::value, void>::type
saveArmaMe(Member &member, string id)
{
    (void) member;
    (void) id;
}

template<typename T>
typename std::enable_if<is_supported_elem_type<T>::value, void>::type
readArmaMe(Member &member, string id)
{
    string vecid = id + "armavec";
    string recid = id + "armarec";
    string matid = id + "armamat";
    string cubid = id + "armacub";

    constexpr uint NX = 4;
    constexpr uint NY = 3;
    constexpr uint NZ = 2;

    Col<T>  v;
    Row<T>  r;
    Mat<T>  m;
    Cube<T> c;

    member.readData(vecid, v);
    member.readData(recid, r);
    member.readData(matid, m);
    member.readData(cubid, c);

    BADAssEqual(NX, r.n_cols);

    BADAssEqual(NX, v.n_rows);

    BADAssEqual(NX, m.n_rows);
    BADAssEqual(NY, m.n_cols);

    BADAssEqual(NX, c.n_rows);
    BADAssEqual(NY, c.n_cols);
    BADAssEqual(NZ, c.n_slices);

    for (uint i = 0; i < NX; ++i)
    {
        BADAssEqual(r(i), _defvalue<T>::value());
        BADAssEqual(v(i), _defvalue<T>::value());

        for (uint j = 0; j < NY; ++j)
        {
            BADAssEqual(m(i, j), _defvalue<T>::value());

            for (uint k = 0; k < NZ; ++k)
            {
                BADAssEqual(c(i, j, k), _defvalue<T>::value());
            }
        }
    }

}

template<typename T>
typename std::enable_if<!is_supported_elem_type<T>::value, void>::type
readArmaMe(Member &member, string id)
{
    (void) member;
    (void) id;
}


#define DUMPSCALARTYPE(type, root) saveMe<type>(root, #type)
#define READSCALARTYPE(type, root) readMe<type>(root, #type)

#define DUMPVECTORTYPE(type, root) saveVectorMe<type>(root, #type)
#define READVECTORTYPE(type, root) readVectorMe<type>(root, #type)

#define DUMPSTATICTYPE(type, root) saveStaticMe<type>(root, #type)
#define READSTATICTYPE(type, root) readStaticMe<type>(root, #type)

#define DUMPPOINTERTYPE(type, root) savePointerMe<type>(root, #type)
#define READPOINTERTYPE(type, root) readPointerMe<type>(root, #type)

#define DUMPARMATYPE(type, root) saveArmaMe<type>(root, #type)
#define READARMATYPE(type, root) readArmaMe<type>(root, #type)

#define suite(WHICH, root) \
    \
    DUMP##WHICH##TYPE(short, root); \
    DUMP##WHICH##TYPE(unsigned short, root); \
    DUMP##WHICH##TYPE(int, root); \
    DUMP##WHICH##TYPE(uint, root); \
    DUMP##WHICH##TYPE(long, root); \
    DUMP##WHICH##TYPE(unsigned long, root); \
    DUMP##WHICH##TYPE(long long, root); \
    DUMP##WHICH##TYPE(unsigned long long, root); \
    DUMP##WHICH##TYPE(float, root); \
    DUMP##WHICH##TYPE(double, root); \
    DUMP##WHICH##TYPE(long double, root); \
    DUMP##WHICH##TYPE(bool, root); \
    \
    root.flush(); \
    \
    READ##WHICH##TYPE(short, root); \
    READ##WHICH##TYPE(unsigned short, root); \
    READ##WHICH##TYPE(int, root); \
    READ##WHICH##TYPE(uint, root); \
    READ##WHICH##TYPE(long, root); \
    READ##WHICH##TYPE(unsigned long, root); \
    READ##WHICH##TYPE(long long, root); \
    READ##WHICH##TYPE(unsigned long long, root); \
    READ##WHICH##TYPE(float, root); \
    READ##WHICH##TYPE(double, root); \
    READ##WHICH##TYPE(long double, root); \
    READ##WHICH##TYPE(bool, root)

#define stringSuite(WHICH, root) \
    DUMP##WHICH##TYPE(string, root); \
    root.flush(); \
    READ##WHICH##TYPE(string, root)

int main()
{
    Root root("testfile.h5");

    srand(time(nullptr));

    _randomvalues::int_value = 10*as_scalar(randu(1,1));
    _randomvalues::double_value = as_scalar(randu(1,1));

    Member &scalarMember = root.addMember("scalar");
    suite(SCALAR, scalarMember);
    stringSuite(SCALAR, scalarMember);

    Member &vectorMember = root.addMember("vector");
    suite(VECTOR, vectorMember);
    stringSuite(VECTOR, vectorMember);

    Member &staticMember = root.addMember("static");
    suite(STATIC, staticMember);
//    stringSuite(POINTER, root);

    Member &pointerMember = root.addMember("pointer");
    suite(POINTER, pointerMember);
//    stringSuite(POINTER, root);

#ifndef HDF5_NO_ARMA
    Member &armaMember = root.addMember("arma");
    suite(ARMA, armaMember);
#endif

    Member &trash = root.addMember("Trash");

    int a[2][2] = {{1,2},{3,4}};
    trash.addData("trash", -1);
    trash.addData("trish", a, {2,2});
    trash.addMember("megatrash");

    BADAssEqual(1u, trash.attributes().size());
    BADAssEqual(1u, trash.datasets().size());
    BADAssEqual(1u, trash.members().size());

    trash.purge();

    BADAssEqual(0u, trash.attributes().size());
    BADAssEqual(0u, trash.datasets().size());
    BADAssEqual(0u, trash.members().size());

    root.removeMember(trash);

    BADAssBool(!root.hasMember("Trash"));

    return 0;
}
