////////////////////////////////////////////////////////////////////////////
// Model bessel.cc               SIMLIB/C++
//
// Besselova diferenci�ln� rovnice:
//
//   y '' + (1/t) * y ' + (1 - 1/t*t) * y = 0
//
// s podm�nkami y(0.001) = 0.001, y '(0.001) = 0.49999
//

#include "simlib.h"

class Bessel { // Besselova diferenci�ln� rovnice
  Integrator yi,y;
 public:
  Bessel():
    yi( -(1/T)*yi+(1/(T*T)-1)*y , 0.49999), 
    y( yi , 0.001)  {}                     
  double Y() { return y.Value(); }
  double YI() { return yi.Value(); }
};

Bessel bes;

void Sample() {                 // vzorkovac� funkce
  Print("%-8g  %g  %g\n", T.Value(), bes.Y(), bes.YI());
}
Sampler S(Sample,0.2);          // vzorkovac� objekt

int main() {                    // popis experimentu
    SetOutput("bessel.dat");
    Print("# Besselova diferenci�ln� rovnice \n");
    Print("# Time    y     y'\n");
    SetStep(1e-6, 0.1);         // krok
    Init(0.001, 50);            // inicializace experimentu
    Run();                      // simulace
    SIMLIB_statistics.Output(); // print run statistics
}

//
