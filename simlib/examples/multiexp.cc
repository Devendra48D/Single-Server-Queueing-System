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


class Zakaznik : public Process { // popis t��dy z�kazn�k�
  double Prichod;                 // atribut ka�d�ho z�kazn�ka
  void Behavior() {               // --- popis chov�n� z�kazn�ka ---
    Prichod = Time;               // �as p��chodu z�kazn�ka
    Seize(Box);                   // obsazen� za��zen� Box
    Wait(10);                     // obsluha
    Release(Box);                 // uvoln�n�
    Tabulka(Time-Prichod);        // doba obsluhy a �ek�n�
  }
 public:
  Zakaznik() { Activate(); }
};

class Generator : public Event {  // gener�tor z�kazn�k�
  double dt;
  void Behavior() {               // --- popis chov�n� gener�toru ---
    new Zakaznik;                    // nov� z�kazn�k, aktivace
    Activate(Time+Exponential(dt));  // interval mezi p��chody
  }
 public:
  Generator(double d) : dt(d) { Activate(); }
};

void Sample() {
   if(Time>0) Print(" %g", Tabulka.stat.MeanValue());
}
Sampler s(Sample,500);

int main() {
  SetOutput("multiexp.dat");    // p�esm�rov�n� v�stupu
  Print("# MULTIEXP --- test v�ce experiment� (�as=%g) \n", ENDTime);
  for(int i=1; i<=20; i++)  {
    Print("# Experiment#%d \n", i);
    Init(0,ENDTime);            // inicializace, �as bude 0..ENDTime
    Box.Clear();                // zde mus�me inicializovat v�echny objekty
    Tabulka.Clear();
    double interval = i;
    new Generator(interval);    // gener�tor z�kazn�k�, aktivuje se
    Print("%g ", interval);
    Run();                      // jeden simula�n� experiment
    Print(" %g\n", Tabulka.stat.MeanValue());
    SIMLIB_statistics.Output();     // print run statistics
  }
}

//
