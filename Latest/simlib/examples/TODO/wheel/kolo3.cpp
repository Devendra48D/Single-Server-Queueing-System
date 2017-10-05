////////////////////////////////////////////////////////////////////////////
// Model: KOLO3.CPP
//

#include "simlib.h"

// popis syst‚mu kola:   y'' = ( F - D * y' - k * y ) / M
struct Kolo {
  Integrator v,y;
  Kolo(Input F, double M, double D, double k):
    v( (F - D*v - k*y) / M ),
    y( v ) {
      Print("# hmotnost = %g kg \n", M);
      Print("# tlumeni  = %g \n", D);
      Print("# tuhost   = %g \n", k);
    }
};

// objekty modelu ...
Constant F = 100;               // s¡la p…sob¡c¡ na kolo k

// sledov n¡ stavu modelu ...
Kolo *p=0; // uazatel na kolo
void Sample() { 
  Print("%g  %g  %g\n", T.Value(), p->y.Value(), p->v.Value()); 
}
Sampler S(Sample, 0.0025);

int main() {                        // popis experimentu ...
  SetOutput("kolo3.dat");
  _Print("# KOLO3 - model tlumen¡ kola v C++/SIMLIB \n");
  for(double m=5; m<=26; m+=5) {
    Print("\n");                    // oddØl¡ vìstupy
    Kolo k(F, m, 500, 5e4);         // vytvoý¡ model
    p = &k;
    Init(0,0.5);                    // inicializace parametr… experimentu
    SetStep(1e-3,0.01);             // rozsah kroku integrace
    SetAccuracy(1e-5,0.001);        // max. povolen  chyba integrace
    Run();                          // simulace
  }
  Print("# konec \n");
  return 0;
}

// konec 
