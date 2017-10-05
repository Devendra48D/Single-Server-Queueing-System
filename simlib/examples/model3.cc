////////////////////////////////////////////////////////////////////////////
// Model MODEL3                   SIMLIB/C++
//
// Uk�zkov� model obslu�n�ho st�ediska s 5 m�sty
//

#include "simlib.h"

// deklarace glob�ln�ch objekt�
Store Box("Linka",5);
Histogram Tabulka("Tabulka",0,5,20);

class Zakaznik : public Process { // t��da z�kazn�k�
  double Prichod;                 // atribut ka�d�ho z�kazn�ka
  int kolik;
  void Behavior() {               // --- popis chov�n� z�kazn�ka ---
    Prichod = Time;               // �as p��chodu z�kazn�ka
    kolik = 1 + int(3*Random());  // cel� ��slo 1 .. 3
    Enter(Box,kolik);             // obsazen� za��zen�
    Wait(10);                     // obsluha
    Leave(Box,kolik);             // uvoln�n�
    Tabulka(Time-Prichod);        // doba obsluhy a �ek�n�
  }
  public: Zakaznik() { Activate(); } // konstruktor aktivuje
}; // Zakaznik

class Generator : public Event {  // gener�tor z�kazn�k�
  void Behavior() {               // --- popis chov�n� gener�toru ---
    new Zakaznik;                 // nov� z�kazn�k
    Activate(Time+Exponential(1e3/150));  // interval mezi p��chody
  }
  public: Generator() { Activate(); } // konstruktor aktivuje
};

int main() {
  SetOutput("model3.out");
  Print(" MODEL3 - test SIMLIB\n");
  Init(0,1000);              // inicializace experimentu, �as bude 0..1000
  new Generator;             // gener�tor z�kazn�k�
  Run();                     // simulace
  Box.Output();              // tisk v�sledk�
  Tabulka.Output();
  SIMLIB_statistics.Output();     // print run statistics
}

//
