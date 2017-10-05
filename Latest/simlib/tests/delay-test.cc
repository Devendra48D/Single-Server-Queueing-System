// delay-test.cc 
// 
// this tests the basic functionality of class Delay in SIMLIB/C++
//

#include "simlib.h"
#include "delay.h"

// parameters
const double dt=10;
const double initval=0;

// test blocks
Delay       d( Sin(T), dt, initval ); // delay sin(time)
Expression  e( Sin(T-dt) );           // exact value
Integrator i(d); // important!!! (switch on continuous simulation) (for now)
Integrator j(e);

// some random events
struct RandomEvent : public Event {
    void Behavior() { 
        Activate( Time + Exponential(0.3) ); 
    }
};

void Sample() {
  double delayed = d.Value();
  double error = delayed - e.Value();
  if(Time <= dt+1e-6) { i.Set(0); j.Set(0); error=0; } // eliminate delay initval
  // difference of integrator outputs --- important, because it takes
  // all differences into account
  Print("%9f  % 9.6f\n", Time, delayed);
}
Sampler s(Sample, 0.1); // output of model

// simple test experiment
int main()
{
    SetOutput("delay-test.out");
    Init(0, 100);
    // without random events
    Run();
    Print("\n\n"); // output separator
    Init(0, 100);
    // with random events
    (new RandomEvent())->Activate();
    (new RandomEvent())->Activate(0.5);
    Run();
}
