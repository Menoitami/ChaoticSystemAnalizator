#include "FrontendBase.hpp"
#include <BackendBase.hpp>
#include <BackendMain.hpp>
#include <BackendTypes.hpp>
#include <iostream>

#ifdef ATTRACOR_GENERATE
#define CALC_METHOD() Backend::generateAttractor(a, h, fixedCT, init)
#endif

int main()
{
    BackendBase *back = BackendBase::instance();

    std::cout << "start" << std::endl;
    CALC_METHOD();

    std::cout << "end" << std::endl;

    return 0;
}
