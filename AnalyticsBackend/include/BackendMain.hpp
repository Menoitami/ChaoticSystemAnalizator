#pragma once

#include <vector>

namespace Backend
{
    void generateAttractor(const double* a, double h, int iterations,const double* initialState);

    void saveAttractor(std::vector<std::array<double, 3>> points);

}
