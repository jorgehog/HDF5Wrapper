#pragma once

#include "member.h"

#include <H5Cpp.h>

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


private:

    const string m_filename;

    state m_state;

    void _onFileOpen();

};

}
