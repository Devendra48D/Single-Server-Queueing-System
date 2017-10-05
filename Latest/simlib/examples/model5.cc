////////////////////////////////////////////////////////////////////////////
// Model MODEL5                   SIMLIB/C++
//
// Uk�zkov� model obslu�n�ho st�ediska (2 za��zen� 1 fronta), jin� strategie
//

#include "simlib.h"

// deklarace glob�ln�ch objekt�
Facility  Box[2];                 // bohu�el nelze inicializovat
Queue InQueue("Vstupn�");
Histogram Tabulka("Tabulka",0,5,20);

class Zakaznik : public Process { // t��da z�kazn�k�
  double Prichod;                 // atribut ka�d�ho z�kazn�ka
  int BoxNum;
  void Behavior() {               // --- popis chov�n� z�kazn�ka ---
    Prichod = Time;               // �as p��chodu z�kazn�ka
    BoxNum = (Random()<0.5) ? 0 : 1;
    if (Box[BoxNum].Busy()) 
      {Into(InQueue); Passivate(); } // ##### ????? !!!!!!
    Seize(Box[BoxNum]);           // obsazen� za��zen�
    Wait(10);                     // obsluha
    Release(Box[BoxNum]);         // uvoln�n�
    for( Queue::iterator p = InQueue.begin();
         p != InQueue.end();
         ++p ) {
      Zakaznik *z = (Zakaznik*)(*p);
      if( z->BoxNum == BoxNum ) {
        z->Out();
        z->Activate();
        break;
      }
    } // for 
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
  SetOutput("model5.out");
  Print(" MODEL5 - test SIMLIB\n");
  Init(0,1000);              // inicializace experimentu, �as bude 0..1000
  Box[0].SetName("Box[0]");
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
