////////////////////////////////////////////////////////////////////////////
// Model micek                   SIMLIB/C++
//
// Skákající míèek (kombinovaný model, varianta 1)
//

#include "simlib.h"

const double g = 9.81;          // gravitaèní zrychlení

class Micek : ConditionDown {   // popis modelu míèku
  Integrator v,y;               // stav
  unsigned count;               // èítaè dopadù
  void Action()  {              // je dopad
      Print("# Odraz %u:\n", ++count);
      Out();                    // tiskneme stav pøi dopadu
      v = -0.8 * v.Value();     // ztráta energie...
      y = 0;            // je nutné pro detekci dal¹ího dopadu !!!
      if(count>=20)             // povolíme max. 20 odrazù
        Stop();                 // konec simulace
  }
public:
  Micek(double initialposition) :
    ConditionDown(y),           // podmínka dopadu: (y>=0) z TRUE na FALSE
    v(-g),                      // y' = INTG( - m * g )
    y(v, initialposition),      // y  = INTG( y' )
    count(0) {}                 // poèet dopadù
  void Out() {
    Print("%-9.3f  % -9.3g  % -9.3g\n",
          T.Value(), y.Value(), v.Value());
  }
};

Micek m1(1.0);                  // model systému

void Sample() { m1.Out(); }     // výstup stavu míèku
Sampler S(Sample,0.01);

int main() {                    // popis experimentu
  SetOutput("micek.dat");
  Print("# MICEK --- model skákajícího míèku\n");
  Print("# Time y v \n");
  Init(0);                      // inicializace parametrù experimentu
  SetStep(1e-10,0.5);           // rozsah kroku integrace
  SetAccuracy(1e-5,0.001);      // max. povolená chyba integrace
  Run();                        // simulace
  SIMLIB_statistics.Output();   // print run statistics
}

//
