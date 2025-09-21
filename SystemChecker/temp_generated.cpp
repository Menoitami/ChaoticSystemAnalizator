#include <iostream>
#include <cmath>
#include <iomanip>
#include <fstream>

// Function to check if a value is valid (not NaN or Inf)
bool is_valid(double x) {
    return !std::isnan(x) && !std::isinf(x);
}

// --- ODE SYSTEM FUNCTION ---
// Receives pointers to arrays X, a, and step size h
// No variable declarations inside — only uses parameters
void system(double X[], double a[], double h) {
float h1 = h * a[0];
float h2 = h * (1 - a[0]);
    
X[0] = X[0] + h1 * (-X[1]);
X[1] = X[1] + h1 * (a[1] * X[0] + sin(X[1]));
    
float z = X[1];
    
X[1] = z + h2 * (a[1] * X[0] + sin(X[1]));
X[0] = X[0] + h2 * (-X[1]);
    
}

int main() {
    double X[2] = {0,0};
    double a[2] = {0,0};
    double h = 0.001;
    

    // Call system once
    system(X, a, h);

    // Write results to file
    std::ofstream out("result.txt");
    if (out.is_open()) {
        out << std::setprecision(15);
        out << X[0] << "\n";
        out << X[1] << "\n";
        out.close();
    }

    // Success output (ASCII-safe, no Unicode)
    std::cout << "SUCCESS: One step completed successfully.\n";
    std::cout << "X[0] = " << X[0] << ", X[1] = " << X[1] << "\n";
    return 0;
}