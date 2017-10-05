// Model struna.cc
// Problém: kmitání struny (viz SKRIPTA-1992 strana 42)
// re¹ení parciální dif. rovnice, metoda pøímek
// (c) Petr Peringer, 1997, 2006

#include "simlib.h"

// parametry simulce:
const double Print_Step = 0.002;  // perioda vzorkování
const double Simul_Time = 0.20;   // doba simulace
const int N = 40;                 // poèet bodù

// model
class Struna {
  const unsigned N; // poèet úsekù
  const double l;   // délka
  const double a;   // koeficient  (pnutí ve strunì ?)
  const double b;   // poèáteèní výchylka uprostøed struny
  class Point : public aContiBlock {
    Integrator v;   // y'
    Integrator y;   // y
   public:
    Point() : v(), y(v) {}
    double Value() { return y.Value(); }
    void Set(Input i, double y0) {
      v.SetInput(i-v*8.0); // tlumení
      y.Init(y0);
    }
  };
  Point *y;  // pole obsahujici uzlove body
 public:
  Struna(unsigned _N, double _l, double _a, double _b) :
    N(_N), l(_l), a(_a), b(_b)  {
    // konstrukce modelu:
    y = new Point[N+1];
    const double dx = l/N;                          // delka useku
    const double adx2 = a/(dx*dx);
    double xi = dx;
    for(int i=1; i<N; i++) {
      y[i].Set( adx2 * (y[i+1] - 2*y[i] + y[i-1]),  // nastaveni vstupu
                b * (-4/(l*l) * xi*xi + 4/l * xi)); // pocatecni podminka
      xi += dx;
    }
  }
  double GetX(unsigned i)  { return (i<=N) ? i*l/N : 0.0; }
  double GetY(unsigned i)  { return (i<=N) ? y[i].Value() : 0.0; }
  unsigned GetN()          { return N; }
};

Struna s(N, 1, 1e3, 0.01);

void Sample() {
  Print("#Time   x       y \n");
  for(int i=0; i<=s.GetN(); i++)
    Print("%-8g %-8g %g \n", Time, s.GetX(i), s.GetY(i) );
  Print("\n");
//  Print("\n");
}
Sampler S(Sample, Print_Step); 

int main() {
  SetOutput("struna.dat");      // output file
  _Print("# Struna \n");
  Init(0, Simul_Time);          // inicializace experimentu
  Run();                        // simulace
}
//
