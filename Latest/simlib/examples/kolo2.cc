////////////////////////////////////////////////////////////////////////////
// Model kolo2                SIMLIB/C++
//
// kmit�n� kola (verze 2 - n�kolik experiment�)
// popis syst�mu kola:  
//   y'' = ( F - D * y' - k * y ) / M
//

#include "simlib.h"
#include <stdlib.h>

struct Kolo {                   // popis syst�mu
  Parameter M, D, k;
  Integrator v, y;
  Kolo(Input F, double _M, double _D, double _k):
    M(_M), D(_D), k(_k),        // parametry syst�mu
    v( (F - D*v - k*y) / M ),
    y( v ) { }
    void SetM(double _M) { M=_M; }
    void SetD(double _D) { D=_D; }
    void Setk(double _k) { k=_k; }
    void PrintParameters() {
      Print("# hmotnost = %g kg ", M.Value());
      Print("  tlumeni = %g ", D.Value());
      Print("  tuhost = %g \n", k.Value());
    }
};

// objekty modelu ...
double _m=5, _d=500, _k=5e4;    // implicitn� hodnoty parametr�
//Problem!!! Constant F = 100;               // s�la p�sob�c� na kolo k
Constant F(100);                // s�la p�sob�c� na kolo k
Kolo k(F, _m, _d, _k);

// sledov�n� stavu modelu ...
void Sample() { 
  Print("%6.3f %.4g %.4g\n", T.Value(), k.y.Value(), k.v.Value()); 
}
Sampler S(Sample, 0.001);

// popis experimentu ...
int main(int argc, char *argv[]) {
  SetOutput("kolo2.dat");
  Print("# KOLO2 - model tlumen� kola (v�ce experiment�)\n");
  if(argc==4) {
     _m = atof(argv[1]);
     _d = atof(argv[2]);
     _k = atof(argv[3]);
  }
  for(double m=_m/2; m<=_m*5; m*=1.2) {
    Print("\n");                    // odd�l� v�stupy
    k.SetM(m);     // nastav� parametr M
    k.SetD(_d);    // nastav� parametr D
    k.Setk(_k);    // nastav� parametr k
    k.PrintParameters();
    Print("# Time  y  v \n");
    Init(0,0.3);                    // inicializace parametr� experimentu
    SetAccuracy(1e-6,0.001);        // max. povolen� chyba integrace
    Run();                          // simulace
    SIMLIB_statistics.Output();     // print run statistics
  }
  Print("# konec \n");
}

//
