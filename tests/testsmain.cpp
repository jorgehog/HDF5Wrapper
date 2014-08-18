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

    root.initialize();

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
    vec v(10, fill::randn);

    Member &member = root.addMember("firstMember");

    member.addData("CUBZOR", C);
    member.addData("VECZOR", v);
    member.addData("attr", 2);

    member.addData("lols", "heisann");

    member.removeData("CUBZOR");
    member.addData("CUBZOR", C);

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

