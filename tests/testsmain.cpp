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

    uint nFields = 10000;
    uint maxSize = 100;

    uint n = 0;

    uint nx;
    uint ny;

    while (n < nFields)
    {
        nx = 1 + (rand() % maxSize);
        ny = 1 + (rand() % maxSize);

        mat m(nx, ny, fill::randu);

        uint key = nx*(maxSize + 1) + ny;

        if (!writer.hasMember(key))
        {
            writer.registerMember(key)
        }

        n++;
    }

    cout << "Hello World!" << endl;
    return 0;
}

