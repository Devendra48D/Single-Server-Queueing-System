////////////////////////////////////////////////////////////////////////////
// Model: 2kmit.CPP
//

#include "simlib.h"


Constant g=9.81;

// popis syst‚mu kola:   y'' = ( F - D * y' - k * y ) / M


// struct Pruzina { double k,l0; input y1,y2 output force };

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
    return K*(l-L0);
  }
};


const double L10=1, L20=1;
const double K1=10, K2=15;
const double y00=5, y10=2, y20=0;

struct System2 {
  Mass m1;
  Mass m2;
  Spring  f1;
  Spring  f2;
//
  System2() :
    m1(f1-f2, 2.0, y10),
    m2(f2, 1.0, y20),
    f1(y00, m1.position, K1, L10),
    f2(m1.position, m2.position, K2, L20)
    {}
};

// objekty modelu ...

System2 s;

// sledov n¡ stavu modelu ...
void Sample() {
  Print("%g  %g  %g\n", T.Value(),
             s.m1.position.Value(), s.m2.position.Value());
}
Sampler S(Sample, 0.1);

int main() {                        // popis experimentu ...
  SetOutput("test.dat");
  _Print("# model 2 systemu \n");
  Init(0,50);                     // inicializace parametr… experimentu
  SetStep(1e-3,0.01);             // rozsah kroku integrace
  SetAccuracy(1e-5,0.001);        // max. povolen  chyba integrace
  Run();                          // simulace
  return 0;
}

// konec 
