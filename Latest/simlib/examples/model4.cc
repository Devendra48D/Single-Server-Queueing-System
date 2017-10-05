////////////////////////////////////////////////////////////////////////////
// Model MODEL4                   SIMLIB/C++
//
// Uk�zkov� model obslu�n�ho st�ediska (2 za��zen� 1 fronta)
//

#include "simlib.h"
#include <stdio.h>

// deklarace glob�ln�ch objekt�
Facility  Box[2];                 // bohu�el nelze inicializovat
Queue InQueue("�ekaj�c� z�kazn�ci");
Histogram Tabulka("Tabulka",0,5,20);

class Zakaznik : public Process { // t��da z�kazn�k�
  double Prichod;                 // atribut ka�d�ho z�kazn�ka
  void Behavior() {               // --- popis chov�n� z�kazn�ka ---
    Prichod = Time;               // �as p��chodu z�kazn�ka
    if (!Box[0].Busy())      Seize(Box[0]); // obsazen� za��zen�1
    else if (!Box[1].Busy()) Seize(Box[1]); // obsazen� za��zen�2
    else { 
        Into(InQueue); Passivate(); // �ek�n� ve front�
	// za��zen�1 nebo za��zen�2 p�i Release automaticky obsad�
        // pozor - zm�na s�mantiky ve verzi SIMLIB 2.17
    }
    Wait(10);                     // obsluha
    // pozor!! nev�me, kter� za��zen� jsme skute�n� obsadili
    if (Box[0].in==this) Release(Box[0]); // uvoln�n�
    else                 Release(Box[1]);
    Tabulka(Time-Prichod);        // doba obsluhy a �ek�n�
  }
  public: Zakaznik() { Activate(); } // konstruktor aktivuje
};

class Generator : public Event {  // gener�tor z�kazn�k�
  void Behavior() {               // --- popis chov�n� gener�toru ---
    new Zakaznik;                 // nov� z�kazn�k
    Activate(Time+Exponential(1e3/150));  // interval mezi p��chody
  }
  public: Generator() { Activate(); } // konstruktor aktivuje
};

int main() {
  SetOutput("model4.out");
  Print(" MODEL4 - test SIMLIB\n");
  Init(0,1000);              // inicializace experimentu, �as bude 0..1000
  Box[0].SetQueue(InQueue);
  Box[0].SetName("Box[0]");
  Box[1].SetQueue(InQueue);
  Box[1].SetName("Box[1]");
  new Generator;             // gener�tor z�kazn�k�
  Run();                     // simulace
  Box[0].Output();           // tisk v�sledk�
  Box[1].Output();
  InQueue.Output();
  Tabulka.Output();
  SIMLIB_statistics.Output();     // print run statistics
}

//
