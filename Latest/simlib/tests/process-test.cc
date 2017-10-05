////////////////////////////////////////////////////////////////////////////
// process-test.cc
//
#include "simlib.h"

int b = 0;

struct ZakaznikA : public Process {
  void Behavior() {               // --- behavior description ---
    Print("A: Start at time %g\n", Time);
    for(int i=1; i<1000; i++) {
        Wait(Random()*10);
        Print("A: %d %g \n", i, Time);
    }
    Print("A: End at time %g\n", Time);
  }
  ZakaznikA()  { Print("new A: %g \n",  Time); }
  ~ZakaznikA() { Print("delete A: %g \n",  Time); }
};

struct ZakaznikB : public Process {
    int n;
  void Behavior() {               // --- behavior description ---
    Print("B: Start at time %g\n", Time);
    for(int i=1; i<1000; i++) {
        Wait(Random()*10);
        Print("B%d: %g \n", n, Time);
    }
    Print("B: End at time %g\n", Time);
  }
  ZakaznikB(int x) : n(x) { Print("new B%d: %g \n", n, Time); }
  ~ZakaznikB() { Print("delete B%d: %g \n", n, Time); }
};

struct ZakaznikC : public Process {
    double n;
  void Behavior() {               // --- behavior description ---
    Print("C: Start at time %g\n", Time);
    for(int i=1; i<1000; i++) {
        Wait(Random()*10); 
        Print("C%g: %g \n", n, Time);
    }
    Print("C: End at time %g\n", Time);
  }
  ZakaznikC(int x) : n(x) { Print("new C%g: %g \n", n, Time); }
  ~ZakaznikC() { Print("delete C%g: %g \n", n, Time); }
};

class Generator : public Event {  // periodic event - generator
  int num;
  void Behavior() {               // --- behavior description ---
    num++;
    (new ZakaznikA)->Activate();   // create and activate at Time
    (new ZakaznikB(num))->Activate();
    (new ZakaznikC(num))->Activate();
    Activate(Time+Exponential(1e3/150));
  }
  public:
  Generator() : num(0) { Activate(); }
};

int main() {                    // experiments
    Print("\n===== BEGIN =====\n");
    for (int i = 1; i <= 3; i++) {
        Print("\n===== Init%d =====\n", i);
        Init(0, 100);           // initialize experiment for time 0..100
        (new Generator)->Activate();
        Print("\n===== Run%d =====\n", i);
        Run();                  // simulation
    }
    Print("\n===== END =====\n");
}

//
