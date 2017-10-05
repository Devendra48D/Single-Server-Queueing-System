// delay-test.cc 
// 
// this tests the basic functionality of class Delay in SIMLIB/C++
//

#include "simlib.h"
#include "zdelay.h"

// parameters
const double dt=3.14159/10;
const double initval=0;

// test blocks
ZDelayTimer clk(dt, ZDelayTimer::Default);
Expression  e(Sin(T-dt));
ZDelay      d(Sin(T), clk, initval ); // delay sin(time)

// some random events
struct RandomEvent : Event {
  void Behavior() { Activate( Time + Exponential(0.5) ); }
  RandomEvent() { Activate( Time ); }
};

void Sample() {
  if(Time>10) return; // stop writing
  double delayed = d.Value();
  double v = e.Value();
  Print("%9.3f  %9.6f %9.3f \n", Time, delayed, v);
}
Sampler s(Sample, 0.01); // output of model

// simple test experiment
int main() {
  //SetOutput("zdelay-test.dat");
  Init( 0, 1000 );
  new RandomEvent(); 
  Run();
}
