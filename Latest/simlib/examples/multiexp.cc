////////////////////////////////////////////////////////////////////////////
// Model multiexp                  SIMLIB/C++
//
// More experiments using various parameters
//

#include "simlib.h"

const double ENDTime = 2000;      // total time of simulation

// model objects:
Facility  Box("Linka");
Histogram Tabulka("Tabulka",0,25,20);


class Zakaznik : public Process { // popis tøídy zákazníkù
  double Prichod;                 // atribut ka¾dého zákazníka
  void Behavior() {               // --- popis chování zákazníka ---
    Prichod = Time;               // èas pøíchodu zákazníka
    Seize(Box);                   // obsazení zaøízení Box
    Wait(10);                     // obsluha
    Release(Box);                 // uvolnìní
    Tabulka(Time-Prichod);        // doba obsluhy a èekání
  }
 public:
  Zakaznik() { Activate(); }
};

class Generator : public Event {  // generátor zákazníkù
  double dt;
  void Behavior() {               // --- popis chování generátoru ---
    new Zakaznik;                    // nový zákazník, aktivace
    Activate(Time+Exponential(dt));  // interval mezi pøíchody
  }
 public:
  Generator(double d) : dt(d) { Activate(); }
};

void Sample() {
   if(Time>0) Print(" %g", Tabulka.stat.MeanValue());
}
Sampler s(Sample,500);

int main() {
  SetOutput("multiexp.dat");    // pøesmìrování výstupu
  Print("# MULTIEXP --- test více experimentù (èas=%g) \n", ENDTime);
  for(int i=1; i<=20; i++)  {
    Print("# Experiment#%d \n", i);
    Init(0,ENDTime);            // inicializace, èas bude 0..ENDTime
    Box.Clear();                // zde musíme inicializovat v¹echny objekty
    Tabulka.Clear();
    double interval = i;
    new Generator(interval);    // generátor zákazníkù, aktivuje se
    Print("%g ", interval);
    Run();                      // jeden simulaèní experiment
    Print(" %g\n", Tabulka.stat.MeanValue());
    SIMLIB_statistics.Output();     // print run statistics
  }
}

//
