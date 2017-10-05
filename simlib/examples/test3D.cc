////////////////////////////////////////////////////////////////////////////
// Model test3D.cc
//
// Basic test of 3D modelling in SIMLIB/C++

#include "simlib.h"
#include "simlib3D.h"

Constant3D   gravity(0,0,-9.81);  // gravity acceleration vector
Value3D      inip(10,10,10);      // initial position vector
Value3D      iniv(1,1,1);         // initial speed vector

struct Model {
  //const double m = 10; // mass is not used
  Integrator3D v;  // rychlost
  Integrator3D p;  // zrychleni
  Model(Input3D a, Value3D initial_p, Value3D initial_v) :
    v(gravity, initial_v),
    p(v, initial_p) {}    
};

////////////////////////////////////////////////////////////////////////////
Model m(gravity, inip, iniv);
////////////////////////////////////////////////////////////////////////////

void sampling_function() {
    Print("%3g: ", Time);
    Print("pos=");
    m.p.Print();
    Print(", v=");
    m.v.Print();
    Print("\n");
}
Sampler s(sampling_function, 0.1);

// Experiment
int main() {
    SetOutput("test3D.out");
    Print("# test3D --- simulation output \n");
    Init(0, 5);
    SetStep(0.01, 0.1);
    Run();
    SIMLIB_statistics.Output(); // print run statistics
}

//
