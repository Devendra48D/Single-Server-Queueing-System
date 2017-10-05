////////////////////////////////////////////////////////////////////////////
// Model lorenz                  SIMLIB/C++
//
// Lorenz equation:
//
//      dx1/dt = sigma * (x2 - x1)
//      dx2/dt = (1 + lambda - x3) * x1 - x2
//      dx3/dt = x1 * x2 - b * x3
//
// Parameters:  sigma = 10
//              lambda = 24
//              b = 2
// Initial conditions:  xi(0) = 1.0
//
// SOURCE: SimPack
//
// Creates output for GNU-Plot
//
// Results strongly depend on required accuracy
//

#include "simlib.h"
#include <cstdlib>      // atof()

const double DEFAULT_T1 = 30.0;

// Model
struct Lorenz {
  Integrator x1, x2, x3;
  Lorenz(double sigma, double lambda, double b) :
    // blocks:
    x1(sigma*(x2 - x1), 1),           // dx1/dt = sigma * (x2 - x1)
    x2((1 + lambda - x3)*x1 - x2, 1), // dx2/dt = (1 + lambda - x3) * x1 - x2
    x3(x1*x2 - b*x3, 1) {}            // dx3/dt = x1 * x2 - b * x3
};

Lorenz L(10, 24, 2);            // Create instance of model

// Output sampling
void Sample() {
    Print("%6.2f %.5g %.5g %.5g\n", T.Value(), L.x1.Value(), L.x2.Value(), L.x3.Value());
}
Sampler S(Sample, 0.01);        // Output step 

// Experiment control
int main(int argc, char *argv[]) {
    double maxtime = DEFAULT_T1;
    if (argc > 1)
        maxtime = std::atof(argv[1]);   // Parameter of simulation
    if (maxtime < 1.0) {
        _Print("\nUsage:  %s  [maxtime>=1,default=%g] \n\n", argv[0], DEFAULT_T1);
        return 1;
    }
    SetOutput("lorenz.dat");    // Redirect output to file
    Print("# Lorenz equation output (maxtime=%g) \n", maxtime);
    Print("# Time x1 x2 \n");
    Init(0, maxtime);           // Initialize simulator
    SetAccuracy(1e-8);          // Required accuracy
    Run();                      // Simulate
    SIMLIB_statistics.Output(); // Print simulation run statistics
}

//
