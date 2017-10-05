

////////////////////////////////////////////////////////////////////////////
// druzice.cc -- pou¾ití knihovny SIMLIB pro spojitou simulaci 3D
//
// (c) PerPet 1997

#include "simlib.h"
#include "simlib3D.h"

typedef Value3D Position, Speed, Force;   // pojmenování typù

const double gravity_constant = 6.67e-11; // gravitaèní konstanta

const double   m0 = 1000;           // hmotnost dru¾ice
const double   m1 = 5.983e24;       // hmotnost Zemì
const double   m2 = 7.374e22;       // hmotnost Mìsíce
const Position p0(36.0e6, 0, 0);    // poloha dru¾ice
const Position p2(384.405e6, 0, 0); // poloha Mìsíce
const Speed    v0(0, 4200, 1600);   // poèáteèní rychlost dru¾ice
const Speed    v2(0, 1022, 0);      // obì¾ná rychlost Mìsíce
const double   MAXTime=3.6e7;       // doba simulace

Constant3D Zero3D(0,0,0); // pomocný objekt

// abstrakce "hmotný bod"
struct MassPoint {
  double m;              // hmotnost
  Expression3D inforce;  // vstupní síla
  Integrator3D v;        // rychlost
  Integrator3D p;        // poloha
  MassPoint(const double mass, Position p0, Speed v0=Speed(0,0,0)) :
    m(mass), 
    inforce(Zero3D),
    v(inforce/m, v0), 
    p(v,p0) {}
   void SetInput(Input3D i) { inforce.SetInput(i); }
};

// abstrakce "digitální svìt"
struct MyWorld {  
  enum { MAX=10 };
  MassPoint *m[MAX];
  unsigned n;
  MyWorld();
  Position Mcenter() {               // výpoèet tì¾i¹tì systému
    Position p = Value3D(0,0,0);
    double sum = 0;
    for(int i=0; i<n; i++) {
      p = p + m[i]->m * m[i]->p.Value();
      sum += m[i]->m;
    }
    return p/sum; 
  }
};

MyWorld *w; // svìt vznikne a¾ pozdìji :-)

// gravitaèní síla pusobící na hmotný bod p
class GravityForce : public aContiBlock3D {
  MassPoint *p;
  MyWorld *w;
 public:
  GravityForce(MassPoint *_p, MyWorld *_w) : p(_p), w(_w) {} 
  
  
  
  
  
  
  
  
  Force Value() {
    Force f(0,0,0);  // gravitaèní síla
    for(int i=0; i < w->n; i++) {
      MassPoint *m = w->m[i];
      if (m == p) continue;
      Value3D distance = m->p.Value() - p->p.Value(); 
      double d = abs(distance);  // vzdálenost
      f = f + (distance * gravity_constant * p->m * m->m / (d*d*d)) ; 
    }
    return f; 
  }
};

// pohyblivé planety
typedef MassPoint Planet, Satelite;

MyWorld::MyWorld() {  // vytvoøíme digitální svìt
  n=0;
  m[n++] = new Planet(m1,Position(0,0,0),Speed(0,0,0));
  m[n++] = new Planet(m2,p2,v2);
  m[n++] = new Satelite(m0,p0,v0);
  for(int i=0; i<n; i++)         // zapneme silové pùsobení
    m[i]->SetInput(new GravityForce(m[i],this));
}

// sledování stavu modelu ...
void Sample();
Sampler s(Sample,3600);  // vzorkování stavu modelu
void Sample() {
  Position t  = w->Mcenter();  // tì¾i¹tì systému
  Position p  = (w->m[2])->p.Value() - t; // pozice mìsíce
  Position p1 = (w->m[1])->p.Value() - t; // pozice dru¾ice
  Print("%g ", Time);
  Print("%g %g %g ", p1.x()/1e6, p1.y()/1e6, p1.z()/1e6);
  Print("%g %g %g ", p.x()/1e6,  p.y()/1e6,  p.z()/1e6);
  Print("\n");
}

// popis experimentu ...
int main()
{
  SetOutput("druzice.dat"); 
  w = new MyWorld;       // vytvoøíme "digitální svìt"
  Print("# Model obìhu dru¾ice kolem soustavy Zemì-Mìsíc \n");
  Init(0, MAXTime);      // inicializace experimentu
  SetMethod("abm4");     // je vhodná vícekroková metoda
  SetAccuracy(1e-7);     // potøebujeme vysokou pøesnost
  Run();                 // simulace
  return 0;
}

