////////////////////////////////////////////////////////////////////////////
// Model topeni                   SIMLIB/C++
//
// P��klad jednoduch�ho syst�mu s neline�rn�m prvkem - rel�
//

#include "simlib.h"

const double T1 = 10,
             T2 = 50,
             K1 = 50,
             K2 = 1;

const double XValue = 10;       // vstup
const double StepPrn = 0.1;     // krok tisku v�sledk�

// tepeln� soustava
class Soust : public aContiBlock {
  Integrator y2, y1;        
 public:
  Soust(Input inp, double T1, double T2, double K1, double K2) :
    y2( (K1*inp-y2)/T1 ),
    y1( (K2*y2-y1)/T2 )   {}
  double Value() { return y1.Value(); }  // v�stup 
};

// cel� syst�m
class Sys : public aContiBlock {
  Relay r;
  Soust s;
 public:
  Sys(Input inp, double T1, double T2, double K1, double K2) :
    r( inp - s, 0, 0, 1, 1, 0, 1 ),
    s( r, T1, T2, K1, K2 )  {}
  double Value() { return s.Value(); }  // v�stup 
  double relayValue() { return r.Value(); }
};

Sys   s(XValue, T1, T2, K1, K2);

// v�stup:
void Sample() { 
    Print("%-8.2f %g %g\n", T.Value(), s.Value(), s.relayValue()); 
}
Sampler S(Sample, StepPrn);

int main() {                    // popis experimentu
  SetOutput("topeni.dat");
  Print("# TOPENI --- model regulace oh��v�n�\n");
  Print("# Time   temp\n");
  Init(0,120);                  // inicializace experimentu
  SetAccuracy(1e-2);            // po�adovan� p�esnost
  SetStep(1e-6,StepPrn);        // rozsah kroku
  Run();                        // simulace
  SIMLIB_statistics.Output();   // print run statistics
}

//
