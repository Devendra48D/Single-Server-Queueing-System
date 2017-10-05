////////////////////////////////////////////////////////////////////////////
// CTEST4.CPP
//
// zmÿna integraün° metody 
//

#include "simlib.h"
#include <math.h>      // sin()

const double ENDTIME = 63;
double ACCURACY = 0.001;

class Test : public aContiBlock { 
  Integrator x,y;
  Expression output;
 public:
  Test():
    x( -y, 1 ), 
    y(  x ),
    output( Sin(T) - y )    {}                     
  double Value() {return output.Value();}
};

Test t;

void Sample() {
    Print("%g %g %g\n", (double)Time, t.Value(), StepSize );
}
Sampler S(Sample,0.1);

int main(int argc, char *argv[])
{
  SetOutput("ctest4.dat");
  _Print("# Test \n");
  if(argc>1)
  {
    double d = atof(argv[1]);
    if(d==0) return 3;
    ACCURACY = d;
  }

  Print("# SetAccuracy(%g) \n", ACCURACY);

//DebugON();

//  _Print("# implicitn° metoda (RKE) \n");
  Init(0,ENDTIME);                // inicializace experimentu
  SetAccuracy(ACCURACY);          // rel chyba
//  SetMethod(SIMLIB_ABM4Step); 
//  SetMethod(SIMLIB_RK45Step); 
//  SetMethod(SIMLIB_EulerStep); 
//  SetMethod(SIMLIB_RK23Step); 
//  SetMethod(SIMLIB_FWStep); 
  Run();                          // simulace

return 0;

  _Print("\n# Eulerova metoda \n");

  Init(0,ENDTIME);                // inicializace experimentu
  SetAccuracy(ACCURACY);       
  SetMethod(SIMLIB_EulerStep);
  Run();                          // simulace

  return 0;
}
