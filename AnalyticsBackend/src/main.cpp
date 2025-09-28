#include "FrontendBase.hpp"
#include <BackendMain.hpp>
#include <BackendTypes.hpp>
#include <iostream>
#include <BackendBase.hpp>

#ifdef ATTRACOR_GENERATE
    #define CALC_METHOD() Backend::generateAttractor(a,h,fixedCT,init)
#endif



int main(){
    BackendBase back;
    back.send_custom_messge("asdasdasd");

    std::cout << "start"<< std::endl;
    CALC_METHOD();

    std::cout << "end"<< std::endl;

    return 0;
}
