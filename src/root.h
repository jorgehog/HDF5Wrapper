#pragma once

#include "member.h"

#include <H5Cpp.h>

#include <armadillo>
using namespace arma;

using namespace H5;
using std::string;


namespace H5Wrapper
{

class Root : public Member
{
public:

    Root(const string filename);

    virtual ~Root();

    void initialize(const uint flag);

    void initialize()
    {
        initialize(H5F_ACC_RDWR);
    }

    void finalize();

    enum class state
    {
        OPEN,
        CLOSED
    };

    void TMP()
    {

        int a = 2;

        if (!m_group->attrExists("n_sAtoms"))
        {
            Attribute nAtoms_a(m_group->createAttribute("n_sAtoms", PredType::NATIVE_INT, H5S_SCALAR));
            nAtoms_a.write(PredType::NATIVE_INT, &a);
        }


        try
        {

            imat A(3, 5, fill::ones);

            const int rank = 2;
            hsize_t dims[rank] = {A.n_rows, A.n_cols};

            DataSpace dataspace(rank, dims);

            // Create the dataset in group "MyGroup".  Same note as for the
            // dataspace above.
            DataSet dataset(m_file->createDataSet("/lolset1", PredType::NATIVE_INT, dataspace));

            // Write the data to the dataset using default memory space, file
            // space, and transfer properties.
            dataset.write(A.memptr(), PredType::NATIVE_INT);

        }
        catch (const H5::FileIException &exc)
        {
            cout << "dataset already exists." << endl;
        }

    }


private:

    const string m_filename;

    state m_state;

    void _onFileOpen();

};

}
