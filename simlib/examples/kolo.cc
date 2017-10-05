////////////////////////////////////////////////////////////////////////////
// Model kolo.cc                 SIMLIB/C++
//
// kmit�n� kola (verze 1)
//
// popis syst�mu kola:   
//   y'' = ( F - D * y' - k * y ) / M
// kde:
//   y   v�chylka kola   
//   D   koeficient tlumen�
//   k   tuhost pru�iny
//   F   s�la p�sob�c� na kolo
//

#include "simlib.h"

struct Kolo {                   // popis syst�mu kola
  Integrator v, y;
  Kolo(Input F, double M, double D, double k):
    v( (F - D*v - k*y) / M ),   // rychlost   
    y( v ) {}                   // v�chylka
};

// objekty modelu ...
//problem C++ | Constant F = 100;               // s�la p�sob�c� na kolo k
Constant F(100);                // s�la p�sob�c� na kolo k
Kolo k(F, 10, 500, 5e4);        // model syst�mu

// sledov�n� stavu modelu ...
void Sample() { 
  Print("%6.3f %.4g %.4g\n", T.Value(), k.y.Value(), k.v.Value()); 
}
Sampler S(Sample, 0.001);

int main() {                    // popis experimentu ...
  SetOutput("kolo.dat");
  Print("# KOLO - model tlumen� kola\n");
  Print("# Time   y   v \n");
  Init(0, 0.5);                 // inicializace parametr� experimentu
  SetStep(1e-3, 0.1);           // rozsah kroku integrace
  SetAccuracy(1e-5, 0.001);     // max. povolen� chyba integrace
  Run();                        // simulace
  Print("# konec \n");
  SIMLIB_statistics.Output(); // print run statistics
}

//
