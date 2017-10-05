////////////////////////////////////////////////////////////////////////////
// Test of SIMLIB/C++

#include "simlib.h"

const int N = 1000;

class Test1 : public Process {
  void Behavior() {
    float data = 1;
    for(int i=0; i<N; i++)
        Wait(1);
    if(data != 1)
        _Print("Error1 in class Process implementation\n");
  }
};

class Test2 : public Process {
  void Behavior() {
    float data = 22222;
    for(int i=0; i<N; i++)
        Wait(1);
    if(data != 22222)
        _Print("Error2 in class Process implementation\n");
  }
};

int main() {
  //DebugON();
  Print("Test2: ");
//  SetOutput("test2.out");
  Init(0, N+1);              // Initialization, Time 0..1000
  (new Test1)->Activate();   // Process1 creation and activation
  (new Test2)->Activate();   // Process2 creation and activation
  Run();                     // simulation
  Print("End time = %g ", Time);
  Print("       N = %d\n", N);
}

