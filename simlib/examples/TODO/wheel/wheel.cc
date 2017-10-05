////////////////////////////////////////////////////////////////////////////
// Model wheel.cc - SIMLIB continuous model example
//

#include "simlib.h"

Constant g(9.81);  // gravity acceleration

////////////////////////////////////////////////////////////////////////////
// Car and wheel motion
//
//           x'' = -g - F / M
//           F   =  D * (x'-y') + K * (x-y-l0) 
//           y'' = -g + F / m
//
// parameters:
//    l0  - length of spring
//    k   - spring stiffnes
//    D   - dissipation coefficient
//    m   - wheel mass
//    M   - car mass
//    x   - car position
//    y   - wheel position
//
const double r0 = 0.2; // wheel radius
const double l0 = 0.3; // spring length

class System : public aContiBlock {              // new continuous block
 public:
  Integrator y, y1;  // wheel
  Integrator x, x1;  // car
  Status F;          // force
  Lim l;
  System(double M, double m, double D, double K):  // parameters
    x1( -g - F/M ),              // car body
    x( x1, r0+l0 ),

    y1( -g + (F + l*1e6)/m ),    // wheel
    y( y1, r0 ),

    F( D*(x1-y1) + K*(x-y-l0) ), // spring & shock absorber

    l(r0-y,0,r0)                 // wheel move limit

    {}
  double Value() { return x.Value(); }  // output = position
};

////////////////////////////////////////////////////////////////////////////
System k1(1000, 20, 5000, 1e5);   // instance of wheel system

////////////////////////////////////////////////////////////////////////////
void Sample() {  // output from model (form suitable for GNUplot)
    Print("%g %g %g %g %g\n", Time, k1.x.Value(), k1.x1.Value(),
	                            k1.y.Value(), k1.y1.Value() );
}
Sampler S(Sample,0.01);

////////////////////////////////////////////////////////////////////////////
int main() {                      // experiment control ...
  SetOutput("wheel.dat");
  _Print("# WHEEL - Car and wheel motion model\n");
  Init(0,5);                      // initialization
  SetStep(1e-6,1);
  Run();                          // simulation run
}

//
