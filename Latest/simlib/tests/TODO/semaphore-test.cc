////////////////////////////////////////////////////////////////////////////
// Model TestSemaphore.CPP 
//
// Example of queuing system using semaphore (the same as examples/model2.cc)

#include "simlib.h"

Semaphore Box2("Linka2");
Queue Q;
Histogram Tabulka("Tabulka",0,25,20);

class Zakaznik2 : public Process { // t��da z�kazn�k�
  double Prichod;                 // atribut ka�d�ho z�kazn�ka
  void Behavior() {               // --- popis chov�n� z�kazn�ka ---
    Prichod = Time;               // �as p��chodu z�kazn�ka
    Box2.P();                     // obsazen� za��zen� Box
    Wait(10);                     // obsluha
    Box2.V();                     // uvoln�n�
    Tabulka(Time-Prichod);        // doba obsluhy a �ek�n�
  }
};

class Generator : public Event {  // gener�tor z�kazn�k�
  void Behavior() {               // --- popis chov�n� gener�toru ---
    (new Zakaznik2)->Activate();  // nov� z�kazn�k, aktivace
    Activate(Time+Exponential(1e3/150));  // interval mezi p��chody
  }
};

int main() {                 // popis experimentu s modelem
  Print("Semaphore test\n");
  Init(0,1000);              // inicializace experimentu, �as bude 0..1000
  (new Generator)->Activate(); // gener�tor z�kazn�k�, jeho aktivace
  Run();                     // simulace
  Box2.Output();
  Tabulka.Output();
}

// end
