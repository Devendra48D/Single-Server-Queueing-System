////////////////////////////////////////////////////////////////////////////
// Model ball3
// Bouncing ball (C++/SIMLIB example)
// 1996
//

#include "simlib.h"

Constant g(9.81);              // gravity

struct Ball {
  Integrator v,y;               // ball state
  Ball(double initialposition) :
    ylim(this),                 // condition
    v(-g - v*0.1),             
    y(v, initialposition)  {}
  void Bang()  {                // bounce action
    static unsigned count=0;   
    Out();
    Print("\n# ***** Bang#%u ***** \n", ++count);
    v = -0.9 * v.Value();     // loss of energy...
    if(v.Value()<1e-6) Stop();
//    y = 0;                    // must be here because of unaccuracy !!!
    Out();
  }
  void Out() {
    Print("%g  %g  %g\n", T.Value(), y.Value(), v.Value());
    if(y.Value() < -0.001)
      Stop();
  }
  // state-event detector:
  class LimitY : ConditionDown {
    Ball *b;
    void Action() { b->Bang(); }  // STATE EVENT action: send message
   public:
    LimitY(Ball *ball) :
      ConditionDown(ball->y),     // condition: (y>=0)  TRUE --> FALSE
      b(ball) {}
  } ylim;
};

Ball b1(10);

void Sample() { b1.Out(); }     // output event
Sampler S(Sample,0.001);

int main() {                    // experiment
  SetOutput("ball2.dat");
  _Print("# Bouncing ball model \n\n");
  Init(0);
  SetStep(1e-10,0.5);
  SetAccuracy(1e-6,1e-6);
  Run();                        // simulation run
  return 0;
}

// end MICEK2.CPP
