////////////////////////////////////////////////////////////////////////////
// Test of SIMLIB/C++

#include "simlib.h"

class Test3 : public Process {
  void Behavior() {
    int data = 1;
    if(data) {
        method1();
        data = ~data;
        Wait(1);
        data = ~data;
        method2();
    }
    if(data != 1)
        _Print("Error in class Process implementation\n");
  }
  void method1() {
      Wait(1);
  }
  void method2();
};

void Test3::method2() {
    Wait(1);
}

int main() {
  //DebugON();
  Print("Test3: ");
//  SetOutput("test3.out");
  Init(0, 10);             // Initialization, Time 0..1000
  (new Test3)->Activate();   // Process creation and activation
  Run();                     // simulation
  Print("End time = %g\n", Time);
}

