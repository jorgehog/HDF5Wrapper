#include <sys/time.h>
#include <iostream>

#include <armadillo>

#include <hdf5wrapper.h>

using namespace arma;
using namespace std;


int main()
{

    HDF5Wrapper writer("testfile.h5");

    writer.initialize();

    srand(time(NULL));

    uint nMembers = 200;
    uint maxSize = 100;

    uint n = 0;

    uint nx;
    uint ny;

    const uint NMAX = 1000000;
    uint nc = 0;

    mat md;
    umat mu;
    imat mi;

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
            if (writer.setData(key, md))
            {
                n++;
            }
            break;
        case 1:
            if (writer.setData(key, mu))
            {
                n++;
            }
            break;
        case 2:
            if (writer.setData(key, mi))
            {
                n++;
            }
            break;
        default:
            break;
        }
        if (writer.setData(key, md))
        {
            n++;
        }

        nc++;

        cout << n << " / " << nMembers << endl;
    }

    return 0;
}

