////////////////////////////////////////////////////////////////////////////
// Model micek                   SIMLIB/C++
//
// Sk�kaj�c� m��ek (kombinovan� model, varianta 1)
//

#include "simlib.h"

const double g = 9.81;          // gravita�n� zrychlen�

class Micek : ConditionDown {   // popis modelu m��ku
  Integrator v,y;               // stav
  unsigned count;               // ��ta� dopad�
  void Action()  {              // je dopad
      Print("# Odraz %u:\n", ++count);
      Out();                    // tiskneme stav p�i dopadu
      v = -0.8 * v.Value();     // ztr�ta energie...
      y = 0;            // je nutn� pro detekci dal��ho dopadu !!!
      if(count>=20)             // povol�me max. 20 odraz�
        Stop();                 // konec simulace
  }
public:
  Micek(double initialposition) :
    ConditionDown(y),           // podm�nka dopadu: (y>=0) z TRUE na FALSE
    v(-g),                      // y' = INTG( - m * g )
    y(v, initialposition),      // y  = INTG( y' )
    count(0) {}                 // po�et dopad�
  void Out() {
    Print("%-9.3f  % -9.3g  % -9.3g\n",
          T.Value(), y.Value(), v.Value());
  }
};

Micek m1(1.0);                  // model syst�mu

void Sample() { m1.Out(); }     // v�stup stavu m��ku
Sampler S(Sample,0.01);

int main() {                    // popis experimentu
  SetOutput("micek.dat");
  Print("# MICEK --- model sk�kaj�c�ho m��ku\n");
  Print("# Time y v \n");
  Init(0);                      // inicializace parametr� experimentu
  SetStep(1e-10,0.5);           // rozsah kroku integrace
  SetAccuracy(1e-5,0.001);      // max. povolen� chyba integrace
  Run();                        // simulace
  SIMLIB_statistics.Output();   // print run statistics
}

//
