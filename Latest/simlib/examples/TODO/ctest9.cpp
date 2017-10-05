////////////////////////////////////////////////////////////////////////////
// CTEST9.CPP
//

#include "simlib.h"
#include <math.h>

const double ENDTIME = 100;

class Test : public aContiBlock {
  Integrator x,y;
  Expression z;          // = identity block
 public:
  Test():
    x( 100*x*Cos(T), 1 ),
    y( -100*y*Cos(T), 1 ),
    z( x*y )  {}
  double Value() { return z.Value(); }
};

Test t;

void Sample() {
    Print("%g %.9g %g\n", T.Value(), t.Value(), StepSize );
}
Sampler S(Sample,0.1);

main(int argc, char *argv[])
{
  Print("# Test \n");
  double x = 0.001;
  if(argc>1)
  {
    x = atof(argv[1]);
    if(x==0) return 3;
  }

  if(x>0) {
    Print("\n# SetStep(%g) \n", x);
    Print("# implicitn¡ metoda (RKE) \n");
    Init(0,ENDTIME);                // inicializace experimentu
    SetAccuracy(1);                 // prakticky nekontroluje
    SetStep(x,x);
    Run();                          // simulace
  }
  else
  {
    x = -x;
    Print("\n# SetAccuracy(%g) \n", x);
    Print("# implicitn¡ metoda (RKE) \n");
    Init(0,ENDTIME);                // inicializace experimentu
    SetAccuracy(x);                 
    SetStep(1e-6,0.1);
    Run();                          // simulace
  }
  return 0;
}
