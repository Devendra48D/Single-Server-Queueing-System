////////////////////////////////////////////////////////////////////////////
// Model rc.cc -- SIMLIB/C++ continuous model example
//
// 1997
// Manual conversion of circuit to differential equation
//

#include "simlib.h"

#if 0
Constant Uin(5);  // try constant input voltage
#else
const double PI = 3.141592653589793;
Expression Uin( 0.5 * Sin(T*2*PI)+5 );
#endif

struct SystemRC {
// RC circuit:
//           _____
//     o----|_____|---o----o
//             R      |
//   Uin             === C   Uout
//                    |
//     o--------------o----o
//
//    Uin  - input voltage
//    Uout - output voltage = Uc
//
// Parameters:
  Parameter   R; // serial resistance
  Parameter   C; // capacity
  Expression  Current;
  Integrator  Uout; // capacitor voltage
  SystemRC(Input Uin, double R0, double C0, double U0=0):
    R(R0), C(C0), Current( (Uin-Uout)/R ),
    Uout( Current/C, U0 )  {}
};

////////////////////////////////////////////////////////////////////////////
SystemRC rc1(Uin, 1000 /*Ohm*/, 0.001 /*Farad*/);   // instance of RC system
////////////////////////////////////////////////////////////////////////////

void Sample1() {  // output from model (form suitable for GNUplot)
  Print("%6.3f %g %g %g\n", T.Value(), 
               Uin.Value(), rc1.Uout.Value(), rc1.Current.Value() );
}
Sampler s(Sample1, 0.01);

int main() {                      // experiment control ...
  SetOutput("rc.dat");
  Print("# RC circuit simulation output\n");
  Init(0,8);                      // initialization
  SetStep(1e-6, 0.01);
  Run();                          // simulation run
}

//
