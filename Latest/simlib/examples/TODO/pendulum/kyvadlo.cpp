////////////////////////////////////////////////////////////////////////////
// Model: Kyvadlo.CPP
//
// buzene kyvadlo
// ZDROJ: jedna habilitacni prace
//

#include "simlib.h"

const double g = 9.81;
double L = 9.81;
double M = 1;
double K = 0.5;
double O = 2.0/3.0;
double A = 1.08*L*L*M;

#if 1

// popis syst‚mu s parametry:
struct Kyvadlo {
  Parameter l, o, A, m, k;
  Integrator x, x1;
  Kyvadlo(double L, double M, double K, double O, double a):
    l(L), m(M), k(K), o(O), A(a),
    x1( - g/l * Sin(x) - k/m * x1 + A/(m*l*l) * Cos(o*T) ),
    x( x1 ) {}
};

Kyvadlo k(L,M,K,O,A);

#else

// zjednoduseny popis:
struct Kyvadlo {
  Parameter o;
  Integrator x, x1;
  Kyvadlo(double O):
    o(O),
    x1( - Sin(x) - x1/2 + 1.15 * Cos(o*T) ),
    x( x1 ) {}
};

Kyvadlo k(O);

#endif


// sledov n¡ stavu modelu ...
void Sample() { 
  Print("%g %g %g\n", Time, k.x.Value(), k.x1.Value() ); 
}
Sampler S(Sample, 0.1);

// popis experimentu ...
int main() {
  SetOutput("kyvadlo.dat");
  Init(0,100);                  
  SetStep(1e-6, 5e-3);
  SetAccuracy(1e-6);
  Run();                       
  return 0;
}


