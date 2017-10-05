////////////////////////////////////////////////////////////////////////////
// Model ctest2.cc               SIMLIB/C++
//
// Filtr s p�enosovou funkc�:
//
//      F(p) = 1 / (p^3 + 3.25p^2 + 16.5p + 17.5)
//
// Model odezvy na jednotkov� skok
//

#include "simlib.h"

#define OUTPUT_FILE "ctest.dat"

const double a1 = 1.0,          // koeficienty p�enosu filtru
             a2 = 3.25,
             a3 = 16.5,
             a4 = 17.5;

const double InpValue = 1.0;    // vstup filtru -- jednotkov� skok
const double StepPrn  = 0.05;   // krok tisku v�sledk�

class Filter : aContiBlock {
  Integrator y2, y1, y;         // pou�it� integr�tory
 public:
  Filter(Input inp, double a1, double a2, double a3, double a4) :
    // popis propojen� blok�:
    y2((inp-a2*y2-a3*y1-a4*y)/a1),   // y''= int(y''')
    y1(y2),                          // y'= int(y'')
    y(y1) {}                         // y= int(y')
  double Value() { return y.Value(); }  // v�stup bloku
};

Filter F(InpValue, a1, a2, a3, a4);

void Sample() {
  Print("%6.2f  %.4g \n", T.Value(), F.Value());
}
Sampler S(Sample, StepPrn);

int main() {                    // popis experimentu
  SetOutput(OUTPUT_FILE);
  Print("# CTEST -- model odezvy p�enosov�ho filtru na jednotkov� skok \n");
  Print("# Time  y\n");
  Init(0,7);                    // inicializace experimentu
  SetAccuracy(1e-3);            // po�adovan� p�esnost
  Run();                        // simulace
  Print("# konec \n");
  SIMLIB_statistics.Output(); // print run statistics
}

//
