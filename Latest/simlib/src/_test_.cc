
//! \file  _test_.cc  Simple test model.

#define SIMLIB_DEBUG

#include "simlib.h"

struct Test {
  Lim limitation;
  Integrator v;
  Status y;
  Test(Input F):
    limitation( v, -1, 1),
    v( Sin(T)*F, 0.5 ),
    y( limitation*F, 1) {}
};

class TestProcess : public Process {
 public:
  void Behavior() {
    Wait(1);
    Wait(1);
    WaitUntil(Time>3);
    WaitUntil(true);
    Wait(5);
  }
};

// objects ...
//Problem!!! Constant F = 1.0; // no copy ctr available
Constant F(1.0);
Test t(F);          //
TestProcess globalprocess;

// sampling ...
void Sample() {
  Print("%f  %f  %f\n", T.Value(), t.y.Value(), t.v.Value());
}
Sampler S(Sample, 1);  // periodic event


int main() {                      // popis experimentu ...
  Print("Model _TEST_ --- test model for SIMLIB/C++ \n");
  Init(0,5);                      //
  SetStep(0.1,1.0);               //
  SetAccuracy(1e-5,0.01);         //
  (new TestProcess)->Activate();
  globalprocess.Activate();
//DebugOFF();
  Run();                          // simulation
  Print("# end of test t=%g\n", Time);
//DebugON();
  return 0;
}

// end
