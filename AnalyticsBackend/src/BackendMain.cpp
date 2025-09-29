#include "../include/BackendMain.hpp"
#include <iostream>
#include <vector>
#include <array>
#include <fstream>
#include <system.cuh>

namespace Backend{

    void generateAttractor( const double* a, double h, int iterations, const double* initialState) {
        std::vector<std::array<double, 3>> points;
        double x[3] = { initialState[0], initialState[1], initialState[2] };
        for (int i = 0; i < iterations; ++i) {
            points.push_back({ x[0], x[1], x[2] });
            calcDiscreteModel(x, a, h);
        }
        saveAttractor(points);
    }

    void saveAttractor(std::vector<std::array<double, 3> > points)
    {
        std::string filename = OUTPUT_PATH;
        filename += "Attractor.csv";
        std::ofstream file(filename);

        if (!file.is_open()) {
            std::cerr << "Error opening file: " << filename << std::endl;
            return;
        }

        for (const auto& point : points) {
            file << point[0] << "," << point[1] << "," << point[2] << "\n";
        }

        file.close();
        std::cout << "Attractor saved to " << filename << std::endl;
    }

}

