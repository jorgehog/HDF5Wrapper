#include <sys/time.h>
#include <iostream>

#include <armadillo>

#include <hdf5wrapper.h>

using namespace H5Wrapper;
using namespace arma;
using namespace std;


int main()
{
    Root root("testfile.h5");

    srand(time(NULL));

    uint nMembers = 3;
    uint maxSize = 30;

    uint n = 0;

    uint nx;
    uint ny;

    const uint NMAX = 10000;
    uint nc = 0;

    mat md;
    umat mu;
    imat mi;

    cube C(3, 3, 3, fill::randu);
    vec v = linspace(0, 100, 101);

    root.addData(12, 1337);

    Member &bror = root.addMember("bror", true);

    bror.addData("CUBZOR", C);
    bror.addData("VECZOR", v);
    bror.addData("attr", 2);

    bror.addData("lols", "bror");

    bror.removeData("CUBZOR");
    bror.addData("CUBZOR", C);

    Member &henrik = bror.addMember("HenrikAS", true);

    icube henriksCube(10, 10, 10, fill::randn);

    henrik.addData("minKube", henriksCube);

    henrik.addData("Etternavn", "Sveinsson");

    vector<string> lols = {"hei", "din", "penis"};

    root.addData("hihihi", lols);

    int *a = new int[2];
    a[0] = 1;
    a[1] = 2;

    uint b[3][2] = {{2, 3}, {2, 3}, {2, 3}};

    bror.addData("a", a, {2});
    bror.addData("b", b, {3, 2});

    return 0;

    while (n < nMembers && nc < NMAX)
    {
        nx = 1 + (rand() % maxSize);
        ny = 1 + (rand() % maxSize);

        md.reshape(nx, ny);
        md.randu();

        mu = conv_to<umat>::from(randi(nx, ny, distr_param(0, 100)));

        mi = randi(nx, ny, distr_param(-100, 100));

        uint key = nx*(maxSize + 1) + ny;

        switch (n%3) {
        case 0:
            if (root.addData(key, md))
            {
                n++;
            }
            break;
        case 1:
            if (root.addData(key, mu))
            {
                n++;
            }
            break;
        case 2:
            if (root.addData(key, mi))
            {
                n++;
            }
            break;
        default:
            break;
        }
        if (root.addData(key, md))
        {
            n++;
        }

        nc++;

        cout << n << " / " << nMembers << endl;
    }

    return 0;
}

