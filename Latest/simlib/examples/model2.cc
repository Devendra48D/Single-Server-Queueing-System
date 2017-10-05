////////////////////////////////////////////////////////////////////////////
// Model MODEL2                   SIMLIB/C++
//
// Ukázkový model jednoduchého obslu¾ného støediska
//
//! \file  model2.cc  Simple model of queueing system (M/D/1).

#include "simlib.h"

// deklarace globálních objektù
Facility  Box("Linka");
Histogram Tabulka("Tabulka",0,25,20);

class Zakaznik : public Process { // tøída zákazníkù
  double Prichod;                 // atribut ka¾dého zákazníka
  void Behavior() {               // --- popis chování zákazníka ---
    Prichod = Time;               // èas pøíchodu zákazníka
    Seize(Box);                   // obsazení zaøízení Box
    Wait(10);                     // obsluha
    Release(Box);                 // uvolnìní
    Tabulka(Time-Prichod);        // doba obsluhy a èekání
  }
};

class Generator : public Event {  // generátor zákazníkù
  void Behavior() {               // --- popis chování generátoru ---
    (new Zakaznik)->Activate();   // nový zákazník, aktivace v èase Time
    Activate(Time+Exponential(1e3/150));  // interval mezi pøíchody
  }
};

int main() {                 // popis experimentu s modelem
  //DebugON();
  SetOutput("model2.out");
  Print(" MODEL2 - pøíklad SIMLIB/C++\n");
  Init(0,1000);              // inicializace experimentu, èas bude 0..1000
  (new Generator)->Activate(); // generátor zákazníkù, jeho aktivace
  Run();                     // simulace
  Box.Output();              // tisk výsledkù
  Tabulka.Output();
  SIMLIB_statistics.Output();     // print run statistics
}

//
