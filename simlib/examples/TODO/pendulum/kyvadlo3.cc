////////////////////////////////////////////////////////////////////////////
// Model: kyvadlo3.cc -- vysoká pøesnost výpoètu
// text encoding: iso-8859-2
// 
// Netlumené kyvadlo -- tyè
//
// (c) PerPet 1995, 2001
// TODO: ovìøit správnost

#include "simlib.h"
#include <iostream>
#include <fstream>
using namespace std;

const double g = 9.81;   // gravitaèní zrychlení
double Length = 1;            // délka

// popis systému:   x'' + omega^2 * sin(x) = 0,  omega^2 = g/L
struct Kyvadlo {
  Parameter L;
  Integrator x1;          // úhlová rychlost
  Integrator x;           // úhel vychýlení
  Kyvadlo(double len):
    L(len),
    x1( - g/L * Sin(x) ),
    x( x1 ) { }
  void PrintParameters() { 
     cout << "# L = " << L.Value();
     cout << ", x0 = " << x.Value() << ", y0 = " << x1.Value() << "\n"; 
  }
};

Kyvadlo k(Length);

// sledovani stavu modelu ...
void Sample() { 
  cout << Time << ' ' << k.x.Value() << ' ' << k.x1.Value() << "\n"; 
}
Sampler S(Sample, 0.1);

// popis experimentu ...
int main() {
  cout << "# Model: kyvadlo3 \n";
  ifstream input("kyvadlo3.in"); // soubor s popisem parametrù experimentù
  if(!input) { 
    cerr << "soubor poèáteèních hodnot 'kyvadlo3.in' neexistuje"; 
    return 0; 
  }
  input >> Length; 
  k.L = Length;
  cerr << "Model kyvadla \nL = " << Length << "\n";
  while(1) {
    double x, y, t;
    input >> x >> y >> t;
    if(!input) break;
    cerr << "x0 = " << x << ",  \t y0 = " << y << "\n";
    k.x.Init(x);                    // poèáteèní hodnoty
    k.x1.Init(y);                    
    cout << "\n";                   // oddìlí výstupy
    k.PrintParameters();
    Init(0,t);                      // inicializace experimentu
    SetAccuracy(1e-14);             // nutné pro vysokou pøesnost
    Run();                          // simulace
  }
  cout << "# konec \n";
}

