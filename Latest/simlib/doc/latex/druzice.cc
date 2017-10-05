

////////////////////////////////////////////////////////////////////////////
// druzice.cc -- pou�it� knihovny SIMLIB pro spojitou simulaci 3D
//
// (c) PerPet 1997

#include "simlib.h"
#include "simlib3D.h"

typedef Value3D Position, Speed, Force;   // pojmenov�n� typ�

const double gravity_constant = 6.67e-11; // gravita�n� konstanta

const double   m0 = 1000;           // hmotnost dru�ice
const double   m1 = 5.983e24;       // hmotnost Zem�
const double   m2 = 7.374e22;       // hmotnost M�s�ce
const Position p0(36.0e6, 0, 0);    // poloha dru�ice
const Position p2(384.405e6, 0, 0); // poloha M�s�ce
const Speed    v0(0, 4200, 1600);   // po��te�n� rychlost dru�ice
const Speed    v2(0, 1022, 0);      // ob�n� rychlost M�s�ce
const double   MAXTime=3.6e7;       // doba simulace

Constant3D Zero3D(0,0,0); // pomocn� objekt

// abstrakce "hmotn� bod"
struct MassPoint {
  double m;              // hmotnost
  Expression3D inforce;  // vstupn� s�la
  Integrator3D v;        // rychlost
  Integrator3D p;        // poloha
  MassPoint(const double mass, Position p0, Speed v0=Speed(0,0,0)) :
    m(mass), 
    inforce(Zero3D),
    v(inforce/m, v0), 
    p(v,p0) {}
   void SetInput(Input3D i) { inforce.SetInput(i); }
};

// abstrakce "digit�ln� sv�t"
struct MyWorld {  
  enum { MAX=10 };
  MassPoint *m[MAX];
  unsigned n;
  MyWorld();
  Position Mcenter() {               // v�po�et t�i�t� syst�mu
    Position p = Value3D(0,0,0);
    double sum = 0;
    for(int i=0; i<n; i++) {
      p = p + m[i]->m * m[i]->p.Value();
      sum += m[i]->m;
    }
    return p/sum; 
  }
};

MyWorld *w; // sv�t vznikne a� pozd�ji :-)

// gravita�n� s�la pusob�c� na hmotn� bod p
class GravityForce : public aContiBlock3D {
  MassPoint *p;
  MyWorld *w;
 public:
  GravityForce(MassPoint *_p, MyWorld *_w) : p(_p), w(_w) {} 
  
  
  
  
  
  
  
  
  Force Value() {
    Force f(0,0,0);  // gravita�n� s�la
    for(int i=0; i < w->n; i++) {
      MassPoint *m = w->m[i];
      if (m == p) continue;
      Value3D distance = m->p.Value() - p->p.Value(); 
      double d = abs(distance);  // vzd�lenost
      f = f + (distance * gravity_constant * p->m * m->m / (d*d*d)) ; 
    }
    return f; 
  }
};

// pohybliv� planety
typedef MassPoint Planet, Satelite;

MyWorld::MyWorld() {  // vytvo��me digit�ln� sv�t
  n=0;
  m[n++] = new Planet(m1,Position(0,0,0),Speed(0,0,0));
  m[n++] = new Planet(m2,p2,v2);
  m[n++] = new Satelite(m0,p0,v0);
  for(int i=0; i<n; i++)         // zapneme silov� p�soben�
    m[i]->SetInput(new GravityForce(m[i],this));
}

// sledov�n� stavu modelu ...
void Sample();
Sampler s(Sample,3600);  // vzorkov�n� stavu modelu
void Sample() {
  Position t  = w->Mcenter();  // t�i�t� syst�mu
  Position p  = (w->m[2])->p.Value() - t; // pozice m�s�ce
  Position p1 = (w->m[1])->p.Value() - t; // pozice dru�ice
  Print("%g ", Time);
  Print("%g %g %g ", p1.x()/1e6, p1.y()/1e6, p1.z()/1e6);
  Print("%g %g %g ", p.x()/1e6,  p.y()/1e6,  p.z()/1e6);
  Print("\n");
}

// popis experimentu ...
int main()
{
  SetOutput("druzice.dat"); 
  w = new MyWorld;       // vytvo��me "digit�ln� sv�t"
  Print("# Model ob�hu dru�ice kolem soustavy Zem�-M�s�c \n");
  Init(0, MAXTime);      // inicializace experimentu
  SetMethod("abm4");     // je vhodn� v�cekrokov� metoda
  SetAccuracy(1e-7);     // pot�ebujeme vysokou p�esnost
  Run();                 // simulace
  return 0;
}

