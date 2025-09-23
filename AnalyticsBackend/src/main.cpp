#include<BackendMain.hpp>
#include<BackendTypes.hpp>
#include <iostream>

#ifdef ATTRACOR_GENERATE
    #define CALC_METHOD() Backend::generateAttractor(a,h,fixedCT,init)
#endif





int main(){
    std::cout << "start";
    CALC_METHOD();
    std::cout << "end";

    return 0;
}
