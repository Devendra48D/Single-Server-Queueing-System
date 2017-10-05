////////////////////////////////////////////////////////////////////////////
// Model silnikov.cc               SIMLIB
// Silnikov equations:
//      dx1/dt = x2
//      dx2/dt = x3
//      dx3/dt = -a*x3 - x2 + b*x1*(1 - c*x1 - d*x1*x1)
// where a=0.4, b=0.65, c=0, and d=1 are parameters
// initial conditions of x1(0) = 0.1234, x2(0)=0.2, x3(0)=0.1
// Output for GNU-Plot
//
// SOURCE: Fishwick
//
// PerPet 1996, 2006


#include "simlib.h"
#include <iostream>
#include <fstream>
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

ofstream f;
// output sampling:
void Sample() {
  f << Time << ' ' << e.x1.Value() << ' ' << e.x2.Value() << '\n';
}
Sampler S(Sample, StepPrn);

// experiment description:
int main(int argc, char*argv[]) {
  if(argc!=3) {
      cerr << "Usage:  " << argv[0] << " method stepsize\n";
      cerr << "(Available methods: abm4 euler fw rke rkf3 rkf5 rkf8)\n";
      return 1;
  }
  double stepsize = strtod(argv[2],0);
  if(stepsize>StepPrn) stepsize=StepPrn;
  if(stepsize<1e-7) stepsize=1e-7;
  const char* method = argv[1];
  string filename = "silnikov-";
  filename += method;
  filename += ".dat";
  f.open(filename.c_str());
  if(!f)
      return 2;
  Init(0,350);               // experiment initialization
  SetStep(1e-10,stepsize);   // set step size range
  SetMethod(method);
  f << "# Silnikov equation, " << method << ", stepsize=" << stepsize << "\n";
  cerr << "method: " << method << ", stepsize=" << stepsize << "\n";
  Run();                     // simulation
}

