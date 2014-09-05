#pragma once

#include "member.h"

#include <H5Cpp.h>

using std::string;

namespace H5Wrapper
{

class Root : public Member
{
public:

    Root(const string filename, const uint flag = H5F_ACC_RDWR);

    virtual ~Root();


    enum class state
    {
        OPEN,
        CLOSED
    };


private:

    const string m_filename;

    state m_state;

    void _finalize();
    void _initialize(const uint flag);
    void _onFileOpen();

};

}
