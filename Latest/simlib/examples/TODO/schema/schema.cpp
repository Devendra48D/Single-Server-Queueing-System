////////////////////////////////////////////////////////////////////////////
// Model: Schema.CPP
//
// zajimavy analogovy model (ZDROJ: Prof. Serba)  --  verze 1
//

#include "simlib.h"

// popis syst‚mu:
struct System {
  Integrator u1, du1, u2, du2;
  System():
    du1(0.05*du1 - 0.1*u1, 0.9),       // subsystem1
    u1(0.1*du1),
    du2(0.05*du2 - 10.*u2, 0.1),       // subsystem2
    u2(10*du2) {}
  double x()  { return du1.Value() + du2.Value(); }
  double y()  { return u1.Value() + u2.Value(); }
};

System k;

// sledov n¡ stavu modelu ...
void Sample() { 
  Print("%g %g %g %g\n", Time, k.x(), k.y(), -k.y() );
}
Sampler S(Sample, 0.01);

// popis experimentu ...
int main() {
  SetOutput("schema.dat");
  Init(0,195);                  
  SetStep(1e-6, 0.01);
  SetAccuracy(1e-3);
  Run();                       
  return 0;
}


