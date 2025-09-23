#pragma once

#include "cuda_runtime.h"
//#include "device_launch_parameters.h"
#include <math.h>




#define SYSTEM_CUSTOM
#define CUSTOM_PARAMS

#define ATTRACOR_GENERATE

#ifdef SYSTEM_CUSTOM

    __host__ __device__ inline void calcDiscreteModel(double* x, const double* a, double h) {
        double h1 = 0.5 * h + a[0];
        double h2 = 0.5 * h - a[0];
        x[0] = h1 * (-x[1] - x[2]) + x[0];
        x[1] = h1 * (x[0] + a[1] * x[1]) + x[1];
        x[2] = h1 * (a[2] + x[2] * (x[0] - a[3])) + x[2];
        double temp = -h2 * (x[0] - a[3]) + 1.0;
        x[2] = (h2 * a[2] + x[2]) / temp;
        temp = -h2 * a[1] + 1.0;
        x[1] = (h2 * x[0] + x[1]) / temp;
        x[0] = h2 * (-x[1] - x[2]) + x[0];
    }

    #ifdef CUSTOM_PARAMS
        const double a[4]{ 0, 0.2, 0.2, 5.7};
        const double init[3]{ 0.01, 0 , 0 };
        const double ranges[4]{ -0, 0.6, 0, 0.6 };
        const double h = 0.01;
        const int indicesOfMutVars[2]{ 1, 2 };
        const int writableVar = 0;
        const double maxValue = 10000;
        const double eps_bif = 0.001;
        const double eps_lle = 1e-6;
        const int preScaller = 1;
        const double fixedCT = 100;
        const double fixedResolution = 50;
        const double modelingTime = 500;
        const double fixedNT = 0.4;
    #endif

    #define SIZE_X 3
    #define SIZE_A 4
    #define CALC_DISCRETE_MODEL(X, a, h) calcDiscreteModel(X, a, h)
#endif
