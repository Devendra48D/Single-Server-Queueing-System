////////////////////////////////////////////////////////////////////////////
// Model rlc.cc -- SIMLIB/C++ continuous model example
//
// Manual conversion of circuit to differential equation
//

#include "simlib.h"
#include <cmath>

const double PI = 3.141592653589793;
const double Frequency = 1.000e6 /* Hz */;
const double Period = 1/Frequency /* s */;

const double EndT = 110*Period;

// Warning: single use alowed
double LastMaximum(double x) {          // LastPeak
    static double lm = 0;
    static double last = 0;
    static bool grows=false;
    if (x > last)
        grows=true;
    if (x <= last ) {		// go down
        if(grows) {
            lm = last;
            grows = false;
        }
    }
    last = x;
    if (x > lm) { // never less than current input
	lm = x;
    }
    return lm;
}

// Warning: single use alowed
double TotalMaximum(double x) {
    static double lm = 0;
    if (x > lm) {  // go up
	lm = x;
    }
    return lm;
}


double Fsweep(double x) {
    double pos=x/EndT;
    return (0.90+0.2*pos) * Frequency;
}
Function Freq(T,Fsweep);

Expression Uin( 1.0 * Sin( Freq * 2*PI*T) ); // input voltage

struct SystemRLC {
// RLC circuit:
//           _____
//     o----|_____|---o-----o----o
//             R      |     )
//   Uin             === C  ) L   Uout
//                    |     )
//     o--------------o-----o----o
//
//    Uin  - input voltage
//    Uout - output voltage = Uc
//
// Parameters:
  Parameter   R;  // serial resistance
  Parameter   L;  // inductance
  Parameter   C;  // capacity
  Expression  Current1;
  Integrator  Current2;
  Function    LastMax;
  Function    TotalMax;
  Integrator  Uout;  // output voltage
  SystemRLC(Input Uin, double R0, double L0, double C0):
    R(R0), L(L0), C(C0), 
    Current1( (Uin-Uout)/R ),
    Current2( Uout/L, 0 ),
    LastMax(Uout, LastMaximum),
    TotalMax(Uout, TotalMaximum),
    Uout( (Current1-Current2)/C, 0 )  {}
  double ResFrequency() { 
      return 1/(2*PI * std::sqrt(L.Value() * C.Value())); 
  }
};


////////////////////////////////////////////////////////////////////////////
SystemRLC rlc1(Uin, 1e4 /*Ohm*/, 5e-5 /*H*/, 0.5e-9 /*Farad*/);
////////////////////////////////////////////////////////////////////////////

void Sample1() {  // output from model (form suitable for GNUplot)
  Print("%g %g %g %g %g %g %g\n", T.Value(), 
            Uin.Value(), rlc1.Uout.Value(), rlc1.Current2.Value(), 
            Freq.Value(), rlc1.LastMax.Value(), rlc1.TotalMax.Value() );
}
Sampler s(Sample1, 0.01*Period);

int main() {                    // experiment control ...
    SetOutput("rlc.dat");
    _Print("# RLC circuit\n");
    Print("# Fres = %g\n", rlc1.ResFrequency());
    Init(0, EndT);             // initialization
    SetStep(1e-9, Period);
    Run();                      // simulation run
}

//
