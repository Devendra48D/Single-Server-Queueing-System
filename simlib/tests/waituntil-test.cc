////////////////////////////////////////////////////////////////////////////
// waituntil-test.cc
//
// needs SIMLIB/debug with debug prints
//
//#define SIMLIB_DEBUG 1
#include "simlib.h"

int b = 0;      // global variable for WaitUntil tests

#ifdef SIMLIB_DEBUG
    void WU_print();
#else
#   define WU_print()
#endif

struct ZakaznikA : public Process {
    int n;
  void Behavior() {
    Wait(Random()*10);
    WaitUntil(b>1); Print("A%d: %g b=%d (>1)\n", n, Time, b);
  }
  ZakaznikA(int x) : n(x) { Print("new A%d: %g \n", n, Time); }
  ~ZakaznikA() { Print("delete A%d: %g \n", n, Time); }
};

struct ZakaznikB : public Process {
    int n;
  void Behavior() {
    WaitUntil(b<2); Print("B%d: %g b=%d (<2)\n", n, Time, b);
    Wait(Random()*10);
    WaitUntil(b>1); Print("B%d: %g b=%d (>1)\n", n, Time, b);
    WaitUntil(b>2); Print("B%d: %g b=%d (>2)\n", n, Time, b);
  }
  ZakaznikB(int x) : n(x) { Print("new B%d: %g \n", n, Time); }
  ~ZakaznikB() { Print("delete B%d: %g \n", n, Time); }
};

class Generator : public Event {
  static int num;
  void Behavior() {
    b = int(Random()*4);
    num++;
    (new ZakaznikA(num))->Activate();
    (new ZakaznikB(num))->Activate();
    WU_print();
    Activate(Time+Exponential(1e3/150));
  }
};
int Generator::num = 0;

int main() {                    // experiment description
    Print("\n===== BEGIN =====\n");
    for (int i = 1; i <= 3; i++) {
        Print("\n===== Init%d =====\n", i);
        Init(0, 100);           // initialize, time 0..100
        (new Generator)->Activate();
        Print("\n===== Run%d =====\n", i);
        Run();                  // simulation
        WU_print();
    }
    Print("\n===== END =====\n");
    return 0;
}

//
