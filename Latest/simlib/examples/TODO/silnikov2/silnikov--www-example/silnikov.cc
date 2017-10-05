////////////////////////////////////////////////////////////////////////////
// Model silnikov.cc               SIMLIB
//
// 1995
// Silnikov equation:
//
//      dx1/dt = x2
//      dx2/dt = x3 
//      dx3/dt = -a*x3 - x2 + b*x1*(1 - c*x1 - d*x1*x1)
//
// where a=0.4, b=0.65, c=0, and d=1 are parameters
// initial conditions of x1(0) = 0.1234, x2(0)=0.2, x3(0)=0.1
// output for Gnuplot
//
// SOURCE: Fishwick
//
// PerPet 1996, 2006

#include "simlib.h"
#include <iostream>
#include <cmath>
using namespace std;

const double StepPrn = 0.05; // step of output

class Silnikov {
 public:
  Parameter a,b,c,d;
  Integrator x1, x2, x3;         
  Silnikov(double _a, double _b, double _c, double _d) :
    a(_a), b(_b), c(_c), d(_d),
    x1(x2, 0.1234),           
    x2(x3, 0.2), 
    x3(-a*x3 - x2 + b*x1*(1 - c*x1 - d*x1*x1), 0.1) {}         
};

Silnikov e(0.4, 0.65, 0, 1);

// output sampling:
void Sample() {
  cout << Time << ' ' << e.x1.Value() << ' ' << e.x2.Value() << '\n';
}
Sampler S(Sample, StepPrn);

// experiment description:
int main() {  
  cout << "# Silnikov equation output \n";
  SetStep(1e-8,1e-3);   // set step size range
  Init(0,250);          // experiment initialization 
  Run();                // simulation
}

