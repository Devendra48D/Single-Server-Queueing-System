////////////////////////////////////////////////////////////////////////////
// Model: Rovnice1.CPP
//

#include "simlib.h"
#include "iostream.h"
#include "fstream.h"

//                            x'' + x * x' + x^2 = 0
struct Rovnice1 {
  Integrator x,x1;
  Rovnice1(double px=0, double px1=0):
    x1( -x*x1 - x*x, px  ),
    x(  x1,          px1 ) { }
};

// model ...
Rovnice1 *r;

// sledov n¡ stavu modelu ...
void Sample() { 
  cout << Time << ' ' << r->x.Value() << ' ' << r->x1.Value() << "\n"; 
}
Sampler S(Sample, 0.05);

// popis experimentu ...
main() {
  cout << "# Rovnice1 \n";
  r = new Rovnice1(0.1,0.1);
  Init(0,100);                    // inicializace parametr… experimentu
  SetAccuracy(1e-6);
  Run();                          // simulace
  delete r;
  cout << "# konec \n";
  return 0;
}


