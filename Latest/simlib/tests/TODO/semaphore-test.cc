////////////////////////////////////////////////////////////////////////////
// Model TestSemaphore.CPP 
//
// Example of queuing system using semaphore (the same as examples/model2.cc)

#include "simlib.h"

Semaphore Box2("Linka2");
Queue Q;
Histogram Tabulka("Tabulka",0,25,20);

class Zakaznik2 : public Process { // tøída zákazníkù
  double Prichod;                 // atribut ka¾dého zákazníka
  void Behavior() {               // --- popis chování zákazníka ---
    Prichod = Time;               // èas pøíchodu zákazníka
    Box2.P();                     // obsazení zaøízení Box
    Wait(10);                     // obsluha
    Box2.V();                     // uvolnìní
    Tabulka(Time-Prichod);        // doba obsluhy a èekání
  }
};

class Generator : public Event {  // generátor zákazníkù
  void Behavior() {               // --- popis chování generátoru ---
    (new Zakaznik2)->Activate();  // nový zákazník, aktivace
    Activate(Time+Exponential(1e3/150));  // interval mezi pøíchody
  }
};

int main() {                 // popis experimentu s modelem
  Print("Semaphore test\n");
  Init(0,1000);              // inicializace experimentu, èas bude 0..1000
  (new Generator)->Activate(); // generátor zákazníkù, jeho aktivace
  Run();                     // simulace
  Box2.Output();
  Tabulka.Output();
}

// end
