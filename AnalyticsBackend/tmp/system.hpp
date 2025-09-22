#pragma once
#define SYSTEM_CUSTOM

#include "cuda_runtime.h"
//#include "device_launch_parameters.h"
#include <math.h>

#ifdef SYSTEM_CUSTOM
__host__ __device__ inline void calcDiscreteModel(double* X, const double* a, double h) {
        float h1 = h * a[0];
        float h2 = h * (1 - a[0]);
            
        X[0] = X[0] + h1 * (-X[1]);
        X[1] = X[1] + h1 * (a[1] * X[0] + sin(X[1]));
            
        float z = X[1];
            
        X[1] = z + h2 * (a[1] * X[0] + sin(X[1]));
        X[0] = X[0] + h2 * (-X[1]);
    }
    #define SIZE_X 2
    #define SIZE_A 2
    #define CALC_DISCRETE_MODEL(X, a, h) calcDiscreteModel(X, a, h)
#endif
