////////////////////////////////////////////////////////////////////////////
// Model: vanderpol.cc
// (c) PerPet, 1997-2007

#include "simlib.h"
#include <iostream>
#include <fstream>
using namespace std;

// Equation:   y'' =  e * ( 1 - x^2) * x' - x
//             e > 0  (e=1)

struct VanDerPol {
  Parameter e;
  Integrator x, x1;
  VanDerPol(double _e):
    e(_e),
    x1( e * (1.0 - x*x) * x1 - x ),
    x( x1, 1e-3 ) { }
  void Sete(double _e) { e=_e; }
  void PrintParameters() { 
     cout << "# x0 = " << x.Value()  << "\n"; 
     cout << "# y0 = " << x1.Value() << "\n"; 
  }
};

// object:
VanDerPol r(1);

// output sampling
void Sample() { 
  cout << Time << ' ' << r.x.Value() << ' ' << r.x1.Value() << "\n"; 
}
Sampler S(Sample, 0.05);

// experiment control
main() {
    cout << "# Van der Pol equation using SIMLIB/C++ \n";
    ifstream f("vanderpol.in");	// Format: x0 y0
    if (!f) {
	cerr << "Can not open file vanderpo.in \n";
	return 0;
    }
    while (1) {
	double x, y;
	f >> x >> y;
	if (!f)
	    break;
	cerr << "x0 = " << x << ",  \t y0 = " << y << "\n";
	r.x.Init(x);		// set initial value
	r.x1.Init(y);
	cout << "\n";		// output separator
	r.PrintParameters();
	Init(0, 20);		// initialize simulator
	SetAccuracy(1e-6, 0.001);
	Run();			// simulation
    }
    cout << "# end \n";
}

