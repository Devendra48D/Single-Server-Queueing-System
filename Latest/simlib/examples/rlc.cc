////////////////////////////////////////////////////////////////////////////
// Model rlc.cc -- SIMLIB/C++ continuous model example
//
// 2007
// RLC circuit with initial voltage on capacitor
//
// Manual conversion of circuit to differential equation
//

#include "simlib.h"
#include <cmath>

const double PI = 3.141592653589793;

struct SystemRLC { // LC circuit:
    //                  
    //    +-----o-----o-----o
    //    |     |     |    
    //   | |    )     |    
    //   | | R  ) L  === C   Uout
    //   | |    )     |    
    //    |     |     |    
    //    +-----o-----o-----o
    //
    // Uout - output voltage = Uc = Ul = Ur
    //
    // Parameters:
    Parameter   R;  // resistance
    Parameter   L;  // inductance
    Parameter   C;  // capacity
    Expression  Current1; 
    Integrator  Current2;
    Integrator  Uout;  // output voltage
    SystemRLC(double U0, double R0, double L0, double C0):
        R(R0), L(L0), C(C0), 
        Current1( -Uout/R ),
        Current2( Uout/L, 0 ),
        Uout( (Current1-Current2)/C, U0 )  {}
    double ResFrequency() { 
        return 1/(2*PI * std::sqrt(L.Value() * C.Value())); 
    }
    double ResPeriod() { 
        return 1/ResFrequency(); 
    }
};


////////////////////////////////////////////////////////////////////////////
SystemRLC rlc(1 /*Volt*/, 1e3 /*Ohm*/, 1e-6 /*Henry*/, 1e-9 /*Farad*/);
////////////////////////////////////////////////////////////////////////////

void Sample1() {  // output from model (form suitable for GNUplot)
    Print("%g %g %g\n", T.Value(), rlc.Uout.Value(), rlc.Current2.Value());
}
Sampler s(Sample1, 0.01*rlc.ResPeriod());

int main() {                            // experiment control ...
    SetOutput("rlc.dat");
    Print("# RLC circuit simulation output\n");
    Print("# Fres = %g\n", rlc.ResFrequency());
    Init(0, 30 * rlc.ResPeriod());      // initialization
    SetStep(1e-9, 0.1 * rlc.ResPeriod());
    Run();                              // simulation run
}

//
