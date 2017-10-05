////////////////////////////////////////////////////////////////////////////
// Test of SIMLIB/C++

#include "simlib.h"

unsigned long N = 1000;
unsigned long number = 0;

class Test1 : public Process {
  void Behavior() {
    float data = 1;
    for(unsigned long i=0; i<N; i++) {
        Wait(1);  // schedule, save context, jump to dispatcher, 
                  // change time, restore context, jump back
        number++;
    }
    if(data != 1)
        _Print("Error in class Process implementation\n");
  }
};

int main() {
  //DebugON();
  Print("Test1: ");
//  SetOutput("test1.out");
  Init(0, N);              // Initialization, Time 0..1000
  (new Test1)->Activate();   // Process creation and activation
  Run();                     // simulation
  Print("End time = %g ", Time);
  Print("  number = %ld\n", number);
}

