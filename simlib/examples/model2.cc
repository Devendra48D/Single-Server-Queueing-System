////////////////////////////////////////////////////////////////////////////
// Model MODEL2                   SIMLIB/C++
//
// Uk�zkov� model jednoduch�ho obslu�n�ho st�ediska
//
//! \file  model2.cc  Simple model of queueing system (M/D/1).

#include "simlib.h"

// deklarace glob�ln�ch objekt�
Facility  Box("Linka");
Histogram Tabulka("Tabulka",0,25,20);

class Zakaznik : public Process { // t��da z�kazn�k�
  double Prichod;                 // atribut ka�d�ho z�kazn�ka
  void Behavior() {               // --- popis chov�n� z�kazn�ka ---
    Prichod = Time;               // �as p��chodu z�kazn�ka
    Seize(Box);                   // obsazen� za��zen� Box
    Wait(10);                     // obsluha
    Release(Box);                 // uvoln�n�
    Tabulka(Time-Prichod);        // doba obsluhy a �ek�n�
  }
};

class Generator : public Event {  // gener�tor z�kazn�k�
  void Behavior() {               // --- popis chov�n� gener�toru ---
    (new Zakaznik)->Activate();   // nov� z�kazn�k, aktivace v �ase Time
    Activate(Time+Exponential(1e3/150));  // interval mezi p��chody
  }
};

int main() {                 // popis experimentu s modelem
  //DebugON();
  SetOutput("model2.out");
  Print(" MODEL2 - p��klad SIMLIB/C++\n");
  Init(0,1000);              // inicializace experimentu, �as bude 0..1000
  (new Generator)->Activate(); // gener�tor z�kazn�k�, jeho aktivace
  Run();                     // simulace
  Box.Output();              // tisk v�sledk�
  Tabulka.Output();
  SIMLIB_statistics.Output();     // print run statistics
}

//
