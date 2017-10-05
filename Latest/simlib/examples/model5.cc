////////////////////////////////////////////////////////////////////////////
// Model MODEL5                   SIMLIB/C++
//
// Ukázkový model obslu¾ného støediska (2 zaøízení 1 fronta), jiná strategie
//

#include "simlib.h"

// deklarace globálních objektù
Facility  Box[2];                 // bohu¾el nelze inicializovat
Queue InQueue("Vstupní");
Histogram Tabulka("Tabulka",0,5,20);

class Zakaznik : public Process { // tøída zákazníkù
  double Prichod;                 // atribut ka¾dého zákazníka
  int BoxNum;
  void Behavior() {               // --- popis chování zákazníka ---
    Prichod = Time;               // èas pøíchodu zákazníka
    BoxNum = (Random()<0.5) ? 0 : 1;
    if (Box[BoxNum].Busy()) 
      {Into(InQueue); Passivate(); } // ##### ????? !!!!!!
    Seize(Box[BoxNum]);           // obsazení zaøízení
    Wait(10);                     // obsluha
    Release(Box[BoxNum]);         // uvolnìní
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
  SetOutput("model5.out");
  Print(" MODEL5 - test SIMLIB\n");
  Init(0,1000);              // inicializace experimentu, èas bude 0..1000
  Box[0].SetName("Box[0]");
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
