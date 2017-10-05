////////////////////////////////////////////////////////////////////////////
// Model kolo.cc                 SIMLIB/C++
//
// kmitání kola (verze 1)
//
// popis systému kola:   
//   y'' = ( F - D * y' - k * y ) / M
// kde:
//   y   výchylka kola   
//   D   koeficient tlumení
//   k   tuhost pru¾iny
//   F   síla pùsobící na kolo
//

#include "simlib.h"

struct Kolo {                   // popis systému kola
  Integrator v, y;
  Kolo(Input F, double M, double D, double k):
    v( (F - D*v - k*y) / M ),   // rychlost   
    y( v ) {}                   // výchylka
};

// objekty modelu ...
//problem C++ | Constant F = 100;               // síla pùsobící na kolo k
Constant F(100);                // síla pùsobící na kolo k
Kolo k(F, 10, 500, 5e4);        // model systému

// sledování stavu modelu ...
void Sample() { 
  Print("%6.3f %.4g %.4g\n", T.Value(), k.y.Value(), k.v.Value()); 
}
Sampler S(Sample, 0.001);

int main() {                    // popis experimentu ...
  SetOutput("kolo.dat");
  Print("# KOLO - model tlumení kola\n");
  Print("# Time   y   v \n");
  Init(0, 0.5);                 // inicializace parametrù experimentu
  SetStep(1e-3, 0.1);           // rozsah kroku integrace
  SetAccuracy(1e-5, 0.001);     // max. povolená chyba integrace
  Run();                        // simulace
  Print("# konec \n");
  SIMLIB_statistics.Output(); // print run statistics
}

//
