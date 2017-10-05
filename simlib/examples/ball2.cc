////////////////////////////////////////////////////////////////////////////
// Model ball2.cc               SIMLIB/C++
//
// Bouncing ball (combined simulation, v2)
//

#include "simlib.h"

const unsigned MaxBang = 10;    // total number of take-offs

// model description:
Constant g(9.81);               // gravity acceleration

struct Ball {                   // ball model
    unsigned count;             // number of take-offs
    class LimitY: public ConditionDown { // state-event detector
        Ball *b;
        void Action() {         // state-event action 
            b->Bang();
        } 
      public:
        LimitY(Ball * ball): 
            ConditionDown(ball->y), // condition (y>=0) change TRUE-->FALSE
            b(ball) { }
    };
    LimitY ylim;                // detector object
    Integrator v, y;            // ball status

    Ball(double initialposition): 
        count(0), 
        ylim(this),             // detector
        v(-g - v * 0.1),        // physics
        y(v, initialposition) { }

    void Bang() {               // state event code - take-off
        Out();                  // output od status before
        Print("\n# Odraz %u\n", ++count);
        v = -0.9 * v.Value();   // loss of energy
        y = 0;                  // this is important for accurate simulation
        if (count >= MaxBang)
            Stop();             // end of simulation run
        else
            Out();              // output od status after
    }
    void Out() {
        Print("%f %9.4g %9.4f\n", T.Value(), y.Value(), v.Value());
    }
};

Ball b1(10);                    // ball object

void Sample() { b1.Out(); }     // periodic status sampling
Sampler S(Sample, 0.05);

int main() {                    // experiment control
    // DebugON();
    SetOutput("ball2.dat");
    Print("# BALL2 - bouncing ball model output\n");
    Print("# Time    y     v \n");
    Init(0);                    // initial time 0
    SetStep(1e-10, 0.5);        // minstep defines accuracy of detection
    SetAccuracy(1e-5, 0.001);   // accuracy of numerical method
    Run();                      // simulation
    SIMLIB_statistics.Output(); // print run statistics
}

//
