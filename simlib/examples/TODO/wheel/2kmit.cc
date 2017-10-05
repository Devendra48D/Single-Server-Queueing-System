////////////////////////////////////////////////////////////////////////////
// Model: 2kmit.CPP
//
// 2 spring+mass systems on top
//
//                parameters      initial positions
//      _______                   y0
//         \
//         /
//         \      K1, L1
//         /
//         \
//         O      M1              y1
//         /
//         \      K2, L2
//         /
//         O      M2              y2
//

#include "simlib.h"


// parameters
const double M1=2,  M2=1;       // mass
const double K1=10, K2=15;      // spring coef.
const double L1=1,  L2=1;       // spring length if zero force
Constant g(9.81);               // block
// initial values
//const double F0=27; // initial force - step to zero
const double F0=17; // initial force - step to zero
const double y0=5, y1=y0-L1-F0/K1, y2=y1-L2-F0/K2; // initial position

// popis syst‚mu kola:   y'' = ( F - D * y' - k * y ) / M

struct Mass : public aContiBlock1 {
  Parameter mass;
  Integrator speed, position;
  Mass(Input force, double m, double pos0):
    aContiBlock1( force ),
    mass( m ),
    speed( -g + force/mass ),
    position( speed, pos0 ) {}
  double Value() { return position.Value(); }
};

struct Spring : aContiBlock2 {
  double K, L0;
  Spring(Input p1, Input p2, double k, double l0) :
    aContiBlock2(p1,p2),
    K(k),
    L0(l0) {}
  double Value() {
    double l = Input1Value() - Input2Value();
    if(l<=0) Error("Pruzina je stlacena na nulovou delku");
    return K*(l-L0); // Force
  }
};

struct System2 {
  Mass m1;
  Mass m2;
  Spring  f1;
  Spring  f2;
//
  System2() :
    m1(f1-f2, M1, y1),
    m2(f2, M2, y2),
    f1(y0, m1.position, K1, L1),
    f2(m1.position, m2.position, K2, L2)
    {}
};

// objekty modelu ...

System2 s;

// sledov n¡ stavu modelu ...
void Sample() {
  Print("%g  %g  %g  %g  %g\n", T.Value(),
             s.m1.position.Value(), s.m2.position.Value(),
             s.m1.speed.Value(), s.m2.speed.Value());
}
Sampler S(Sample, 0.02);

int main() {                        // popis experimentu ...
  SetOutput("2kmit.dat");
  _Print("# model \n");
  Init(0,50);                     // inicializace parametr… experimentu
  SetStep(1e-3,0.01);             // rozsah kroku integrace
  SetAccuracy(1e-6);
  Run();                          // simulace
  SIMLIB_statistics.Output();
}

//
