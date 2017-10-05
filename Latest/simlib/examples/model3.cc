////////////////////////////////////////////////////////////////////////////
// Model MODEL3                   SIMLIB/C++
//
// Ukázkový model obslu¾ného støediska s 5 místy
//

#include "simlib.h"

// deklarace globálních objektù
Store Box("Linka",5);
Histogram Tabulka("Tabulka",0,5,20);

class Zakaznik : public Process { // tøída zákazníkù
  double Prichod;                 // atribut ka¾dého zákazníka
  int kolik;
  void Behavior() {               // --- popis chování zákazníka ---
    Prichod = Time;               // èas pøíchodu zákazníka
    kolik = 1 + int(3*Random());  // celé èíslo 1 .. 3
    Enter(Box,kolik);             // obsazení zaøízení
    Wait(10);                     // obsluha
    Leave(Box,kolik);             // uvolnìní
    Tabulka(Time-Prichod);        // doba obsluhy a èekání
  }
  public: Zakaznik() { Activate(); } // konstruktor aktivuje
}; // Zakaznik

class Generator : public Event {  // generátor zákazníkù
  void Behavior() {               // --- popis chování generátoru ---
    new Zakaznik;                 // nový zákazník
    Activate(Time+Exponential(1e3/150));  // interval mezi pøíchody
  }
  public: Generator() { Activate(); } // konstruktor aktivuje
};

int main() {
  SetOutput("model3.out");
  Print(" MODEL3 - test SIMLIB\n");
  Init(0,1000);              // inicializace experimentu, èas bude 0..1000
  new Generator;             // generátor zákazníkù
  Run();                     // simulace
  Box.Output();              // tisk výsledkù
  Tabulka.Output();
  SIMLIB_statistics.Output();     // print run statistics
}

//
