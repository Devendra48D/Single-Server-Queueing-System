////////////////////////////////////////////////////////////////////////////
// Model MODEL4                   SIMLIB/C++
//
// Ukázkový model obslu¾ného støediska (2 zaøízení 1 fronta)
//

#include "simlib.h"
#include <stdio.h>

// deklarace globálních objektù
Facility  Box[2];                 // bohu¾el nelze inicializovat
Queue InQueue("Èekající zákazníci");
Histogram Tabulka("Tabulka",0,5,20);

class Zakaznik : public Process { // tøída zákazníkù
  double Prichod;                 // atribut ka¾dého zákazníka
  void Behavior() {               // --- popis chování zákazníka ---
    Prichod = Time;               // èas pøíchodu zákazníka
    if (!Box[0].Busy())      Seize(Box[0]); // obsazení zaøízení1
    else if (!Box[1].Busy()) Seize(Box[1]); // obsazení zaøízení2
    else { 
        Into(InQueue); Passivate(); // èekání ve frontì
	// zaøízení1 nebo zaøízení2 pøi Release automaticky obsadí
        // pozor - zmìna sémantiky ve verzi SIMLIB 2.17
    }
    Wait(10);                     // obsluha
    // pozor!! nevíme, které zaøízení jsme skuteènì obsadili
    if (Box[0].in==this) Release(Box[0]); // uvolnìní
    else                 Release(Box[1]);
    Tabulka(Time-Prichod);        // doba obsluhy a èekání
  }
  public: Zakaznik() { Activate(); } // konstruktor aktivuje
};

class Generator : public Event {  // generátor zákazníkù
  void Behavior() {               // --- popis chování generátoru ---
    new Zakaznik;                 // nový zákazník
    Activate(Time+Exponential(1e3/150));  // interval mezi pøíchody
  }
  public: Generator() { Activate(); } // konstruktor aktivuje
};

int main() {
  SetOutput("model4.out");
  Print(" MODEL4 - test SIMLIB\n");
  Init(0,1000);              // inicializace experimentu, èas bude 0..1000
  Box[0].SetQueue(InQueue);
  Box[0].SetName("Box[0]");
  Box[1].SetQueue(InQueue);
  Box[1].SetName("Box[1]");
  new Generator;             // generátor zákazníkù
  Run();                     // simulace
  Box[0].Output();           // tisk výsledkù
  Box[1].Output();
  InQueue.Output();
  Tabulka.Output();
  SIMLIB_statistics.Output();     // print run statistics
}

//
