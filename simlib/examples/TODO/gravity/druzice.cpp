////////////////////////////////////////////////////////////////////////////
// DRUZICE.CPP - pou§it¡ knihovny SIMLIB pro spojitou simulaci
// Pý¡klad Ÿ.24 z Ÿ sti spojitìch pý¡klad…
// upraveno

#include "simlib.h"
#include <graphics.h>
#include <math.h>

const double STREDX = 320;
const double STREDY = 240;
const double ZVET   = 2.e-5;

struct Vector {
  double x;
  double y;
  Vector() : x(0), y(0) {}
  Vector(double x0, double y0) : x(x0), y(y0) {}
  Vector operator += (Vector v) { x+=v.x; y+=v.y; return *this; }
  Vector operator -= (Vector v) { x-=v.x; y-=v.y; return *this; }
  Vector operator *= (double k) { x*=k; y*=k; return *this; }
  Vector operator /= (double k) { x/=k; y/=k; return *this; }
};
inline double Abs(Vector v) { return sqrt(v.x*v.x + v.y*v.y); }
inline Vector operator+ (Vector v1, Vector v2) {
  Vector v = v1;
  return v += v2;
}
inline Vector operator- (Vector v1, Vector v2) {
  Vector v = v1;
  return v -= v2;
}
inline Vector operator- (Vector v1) {
  Vector v;
  return v -= v1;
}
inline Vector operator* (double k, Vector v1) {
  Vector v = v1;
  return v *= k;
}
inline Vector operator* (Vector v1, double k) {
  Vector v = v1;
  return v *= k;
}
inline Vector operator/ (Vector v1, double k) {
  Vector v = v1;
  return v /= k;
}

typedef Vector Position;
typedef Vector Speed;
typedef Vector Force;

const double gravity_constant = 6.67e-11;    //gravitaŸn¡ konstanta

const double   m1 = 3.958e23;  //hmotnost 1. tØlesa
const double   m2 = 5.97e24;   //hmotnost 2. tØlesa
const Position p1(3.84e6, 0);  //poloha 1. tØlesa
const Position p2(0, 0);       //poloha 2. tØlesa
const Position p0(1.92e6, 0);  //poŸ teŸn¡ poloha dru§ice
const Speed    v0(0, 1.9e4);   //poŸ teŸn¡ rychlost dru§ice
const double   m0 = 1000;      //hmotnost dru§ice

////////////////////////////////////////////////////////////////////////////
struct MassPoint {
  double mass;
 private:  
  Position p;
 public:
  virtual Position GetPosition() { return p; }
  MassPoint(const double m, Position p0) :  mass(m),  p(p0)  {}
};

Force GravityForce(MassPoint &p1, MassPoint &p2) { // vìpoŸet gravitaŸn¡ s¡ly
  Force F;
  Vector r = p2.GetPosition() - p1.GetPosition();
  double distance = Abs(r);
  double d3 = distance * distance * distance; 
  F = r * gravity_constant*p1.mass*p2.mass/d3;
  return F;
}

////////////////////////////////////////////////////////////////////////////
// neodpov¡d  pýesnØ realitØ - nepohybliv‚ planety
//
struct Planet : public MassPoint {
  const double radius;
  Planet(const double m, Position p, const double r) :
           MassPoint(m, p), radius(r) {}
};

// class World = kontejner pro vsechny planety/hmotu !!!!!!
// nalnit
Planet   planet1(m1,p1,1.0);
Planet   planet2(m2,p2,2.0);

////////////////////////////////////////////////////////////////////////////
struct Satelite : public MassPoint {
 private:
//  double mass;
  Integrator x, y;    // position
  Integrator vx, vy;  // velocity

// doýeçit!!!!!!!!!!!!
  class xAcceleration : public aContiBlock { //blok vìpoŸtu zrychlen¡
    MassPoint &p;
    double Value() {
       Force F = GravityForce(p,planet1) + GravityForce(p,planet2);
       return F.x/p.mass;
    }
   public:    xAcceleration(MassPoint &mp): p(mp) {}
  } ax;
  class yAcceleration : public aContiBlock { //blok vìpoŸtu zrychlen¡
    MassPoint &p;
    double Value() {
       Force F = GravityForce(p,planet1) + GravityForce(p,planet2);
       return F.y/p.mass;
    }
   public:    yAcceleration(MassPoint &mp): p(mp) {}
  } ay;

 public:
  Position GetPosition() { return Position(x.Value(), y.Value()); }
  Speed    GetSpeed() { return Speed(vx.Value(), vy.Value()); }
  Satelite(double m, Position p0, Speed v0=Speed(0,0)) :
       MassPoint(m,p0),
       ax(*this),
       ay(*this),
       vx( ax, v0.x ),
       vy( ay, v0.y ),
       x(  vx, p0.x ),
       y(  vy, p0.y )
       {}
};

Satelite satelite(m0,p0,v0);

////////////////////////////////////////////////////////////////////////////
// grafika
//
void Plot(Position p, int what=0)
{
  int px = STREDX+(ZVET*p.x);
  int py = STREDY+(ZVET*-p.y);
  if(what)
  {
    putpixel(px,py, EGA_RED);
    putpixel(px,py+1, EGA_RED);
    putpixel(px,py-1, EGA_RED);
    putpixel(px+1,py, EGA_RED);
    putpixel(px-1,py, EGA_RED);
  }
  else
    putpixel(px,py, EGA_WHITE);
}

void InitGraphics() {
   int gdriver = VGA, gmode=VGAHI, errorcode;
   initgraph(&gdriver, &gmode, "");
   errorcode = graphresult();
   if (errorcode != grOk)
      Error("Graphics error: %s\n", grapherrormsg(errorcode));
   Plot(planet1.GetPosition(),1);      // nakreslen¡ planety1
   Plot(planet2.GetPosition(),2);      // nakreslen¡ planety2
}

void DoneGraphics() {
  closegraph();
}

////////////////////////////////////////////////////////////////////////////
// sledov n¡ stavu modelu ...
//
void Sample();
Sampler s(Sample,10);
void Sample() {
  Position p = satelite.GetPosition();
  Speed v = satelite.GetSpeed();
  double dt = 1e5/(Abs(v)+1); // ka§dìch x km zaznamenat polohu

  Vector r = p - planet2.GetPosition();
  double distance = Abs(r);
  if(distance<1e5) dt /= 10;

  Print("%g %g %g \n", Time, p.x/1e6, p.y/1e6);
  Plot(p);
  s.SetStep(dt);
}

////////////////////////////////////////////////////////////////////////////
// popis experimentu ...
//
int main()
{
  SetOutput("druzice.dat");
  _Print("# Model obØhu dru§ice kolem soustavy dvou tØles v C++/SIMLIB \n");
  Init(0, 3600*1e3);              // inicializace experimentu
  InitGraphics();
  SetAccuracy(1e-9);              // je nutn  vysok  pýesnost
  Run();                          // simulace
  DoneGraphics();
  return 0;
}

////////////////////////////////////////////////////////////////////////////
