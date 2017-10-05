////////////////////////////////////////////////////////////////////////////
// CTEST8.CPP
//

#include "simlib.h"
#include <math.h>      // sin()

const double ENDTIME = 3000;
double ACCURACY = 0.001;

class Test : public aContiBlock { 
  Integrator x,y;
 public:
  Test(): x(-y, 1 ), y( x ) {}
  double Value() {return y.Value();}
} t;

void Sample() {
    Print("%g %g %g\n", (double)Time, sin(Time)-t.Value(), StepSize );
}
Sampler S(Sample,1);

int main(int argc, char *argv[])
{
  if(argc>1)
  {
    double d = atof(argv[1]);
    if(d==0) { Print("chybny parametr (rel.presnost)\n"); return 3; }
    ACCURACY = d;
  }

  SetOutput("ctest8.dat");
  _Print("# Test \n");
  Print("# SetAccuracy(%g) \n", ACCURACY);
  Print("# implicitn¡ metoda (RKE) \n");
  Init(0,ENDTIME);                // inicializace experimentu
  SetAccuracy(ACCURACY);       
  Run();                          // simulace

  return 0;
}
