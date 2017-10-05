////////////////////////////////////////////////////////////////////////////
// Test of simple event          SIMLIB/C++

#include "simlib.h"

class Test4 : public Event {
  void Behavior() {
      Print("T=%g Event behavior\n", Time);
// corrected      Cancel(); // there is memory leak without this
  }
 public:
  Test4() {
      Print("T=%g Event created\n", Time);
  }
  ~Test4() {
      Print("T=%g Event destroyed\n", Time);
  }
};

int main() {
  //DebugON();
  Print("Test4:\n");
  Init(0);
  Print("Start time = %g\n", Time);
  (new Test4)->Activate();   // Event creation and activation
  Run();                     // simulation
  Print("End time = %g\n", Time);

  Init(10);
  Print("Start time = %g\n", Time);
  (new Test4)->Activate();   // Event creation and activation
  Run();                     // simulation
  Print("End time = %g\n", Time);
}

