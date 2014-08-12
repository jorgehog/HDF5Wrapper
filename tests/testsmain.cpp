#include <iostream>
#include <hdf5wrapper.h>

using namespace std;


int main()
{

    HDF5Wrapper writer("testfile.h5");

    writer.initialize();

    cout << "Hello World!" << endl;
    return 0;
}

