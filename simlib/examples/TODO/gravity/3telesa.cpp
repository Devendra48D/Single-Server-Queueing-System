////////////////////////////////////////////////////////////////////////////
// Model 3telesa.cpp
//
// podivna rovnice !!!!!
//

#include "simlib.h"
#include <graphics.h>
#include <math.h>

const double Mi  = 0.012277471;
const double MiC = 1 - Mi;

// parametry zobrazen°:
  const double STREDX = 320;
  const double STREDY = 240;
  const double ZVET   = 100;

class Rovnice {
  Input d1,d2;
 public:
  Integrator x1,x;
  Integrator y1,y;
  Rovnice() :
    d1( Sqrt((x+Mi)*(x+Mi) + y*y) ),
    d2( Sqrt((x-MiC)*(x-MiC) +y*y) ),
    x1 ( x
       + 2*y1
       - MiC / (d1*d1*d1) * (x+Mi)
       - Mi / (d2*d2*d2) * (x-MiC),
       0 // poü. podm°nka
       ),
    y1(  y
       -2*x1
       -MiC*y / (d1*d1*d1)
       -Mi*y / (d2*d2*d2),
//       -2.00158510637908255224053786224 // poü. podm°nka
       -2.0317326295573368357302057924 // poü. podm°nka
     ),
    x(x1,0.994),
    y(y1)

    {}

} r;

////////////////////////////////////////////////////////////////////////////
// grafika
//
void Plot(double x, double y, int what=0)
{
  int px = STREDX + (ZVET * x);
  int py = STREDY + (ZVET * -y);
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
}

void DoneGraphics() {
  closegraph();
}

////////////////////////////////////////////////////////////////////////////
// sledov†n° stavu modelu ...
//
void Sample();
Sampler s(Sample,0.01);
void Sample() {
  double x = r.x.Value();
  double y = r.y.Value();
  Print("%g %g %g \n", Time, x, y);
  Plot(x,y);
}

/*
         ya = x;  yb = y;  DISP GRAPH #1, ya, yb;
         ya = x1; yb = y1; DISP GRAPH #2, ya, yb;
         ya = x;  yb = x1; DISP GRAPH #3, ya, yb;
         ya = y;  yb = y1; DISP GRAPH #4, ya, yb);

*/


////////////////////////////////////////////////////////////////////////////
// popis experimentu ...
//
main()
{
  SetOutput("3telesa.dat");
  _Print("# Model obÿhu drußice kolem soustavy dvou tÿles v C++/SIMLIB \n");
  Init(0, 300);                   // inicializace experimentu
  InitGraphics();
  SetStep(1e-12,0.01);
  SetAccuracy(1e-14);             // je nutn† vysok† p˝esnost
  Run();                          // simulace
  DoneGraphics();
  return 0;
}

////////////////////////////////////////////////////////////////////////////

/*
Reseni systemu diferencialnich rovnic
  System tri teles
*************************************
- - - - - - - - - promànnÇ, konstanty, procedury a funkce  - - - - - - - - -
USES DIFROV;

REAL x, y[1 TO 2], y1[1 TO 2], y2[1 TO 2];
REAL Mi := 0.012277471;
REAL MiC := 1 - Mi;
REAL D1, D2;
- - - - - - - - - - - - - -  poá†teán° hodnoty   - - - - - - - - - - - - - -
x = 0;
y[1] = 0.994;
y[2] = 0;
y1[1] = 0;
y1[2] = -2.0317326295573368357302057924;
! y1[2] = -2.00158510637908255224053786224;

DifRovPres(1E-12, 1E-12);

DifRov2S(y2[1]=y[1]+2*y1[2]-MiC*1/(((y[1]+Mi)^2+y[2]^2)*sqrt((y[1]+Mi)^2+y[2]^2))*(y[1]+Mi)-Mi*1/(((y[1]-MiC)^2+y[2]^2)*sqrt((y[1]-MiC)^2+y[2]^2))*(y[1]-MiC);
         y2[2]=y[2]-2*y1[1]-MiC*y[2]/(((y[1]+Mi)^2+y[2]^2)*sqrt((y[1]+Mi)^2+y[2]^2))-Mi*y[2]/(((y[1]-MiC)^2+y[2]^2)*sqrt((y[1]-MiC)^2+y[2]^2)),
         y2[], y1[], y[], x, 1E-3, x < 300,
         DISP TAB #1, x;
         ya = y[1]; yb = y[2]; DISP GRAPH #1, ya, yb;
         ya = y1[1]; yb = y1[2]; DISP GRAPH #2, ya, yb;
         ya = y[1]; yb = y1[1]; DISP GRAPH #3, ya, yb;
         ya = y[2]; yb = y1[2]; DISP GRAPH #4, ya, yb);
- - - - - - - - - - - - - - - - -  model   - - - - - - - - - - - - - - - - -
STOP
- - - - - - - - - - - - - - - - - - disp - - - - - - - - - - - - - - - - - -
S1:G1;"ya, yb
S2{S2,X20,Y10}:T1,G1;
S4:G1,G2,G3,G4;"reseni dif. rov. popisujici pohyb tri teles
G1{A-}:({-2}<ya<{2},{-2}<yb<{2}){C1,M1,L,A-};
G2:({-2}<ya<{2},{-2}<yb<{2}){C1,M1,L,D};
G3:({-2}<ya<{2},{-2}<yb<{2}){C1,M1,L};
G4:({-2}<ya<{2},{-2}<yb<{2}){C1,M1,L};
T1:x{D -6};
M5:32;
M6:32;
Z5:32;
Z6:32;
*/
