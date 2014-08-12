#include <iostream>
#include <hdf5wrapper.h>

#include <armadillo>

using namespace std;


int main()
{

    HDF5Wrapper writer("testfile.h5");

    writer.initialize();

    uint nFields = 100;
    uint maxSize = 1000;

    uint n = 0;
    while (n < nFields)
    {
        cout << random() << endl;
        n++;
    }

    cout << "Hello World!" << endl;
    return 0;
}

