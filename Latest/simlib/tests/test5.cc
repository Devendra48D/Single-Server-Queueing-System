////////////////////////////////////////////////////////////////////////////
// Test of Cancel()             SIMLIB/C++

#include "simlib.h"

class TestEvent : public Event {
  void Behavior() {
      Print("T=%g Event behavior\n", Time);
      Cancel();
  }
 public:
  TestEvent() {
      Print("T=%g Event created\n", Time);
  }
  ~TestEvent() {
      Print("T=%g Event destroyed\n", Time);
  }
};

class TestProcess : public Process {
  void Behavior() {
      Print("T=%g Process behavior\n", Time);
      Wait(1);
      Cancel();
      Wait(1);
  }
 public:
  TestProcess() {
      Print("T=%g Process created\n", Time);
  }
  ~TestProcess() {
      Print("T=%g Process destroyed\n", Time);
  }
};

int main() {
  //DebugON();
  Print("Test5 (Cancel):\n");
  Init(0);
  Print("Start time = %g\n", Time);
  (new TestEvent)->Activate();   // Event creation and activation
  (new TestProcess)->Activate(); // Process creation and activation
  Run();                     // simulation 1
  Print("End time = %g\n", Time);

  Init(10);
  Print("Start time = %g\n", Time);
  (new TestEvent)->Activate();   // Event creation and activation
  (new TestProcess)->Activate(); // Process creation and activation
  Run();                     // simulation 2
  Print("End time = %g\n", Time);
}

