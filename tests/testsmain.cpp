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

    uint nMembers = 10000;
    uint maxSize = 100;

    uint n = 0;

    uint nx;
    uint ny;

    double a[3][3] = {{1,2,3}, {1,2,3}, {1,2,3}};
    cube B(3,3,3, fill::randn);
    vec A(100, fill::randu);

    while (n < nMembers)
    {
        nx = 1 + (rand() % maxSize);
        ny = 1 + (rand() % maxSize);

        mat m(nx, ny, fill::randu);

        uint key = nx*(maxSize + 1) + ny;


        if (!writer.hasMember(key))
        {
            writer.registerMember(key, A);
            n++;
        }

    }

    return 0;
}

