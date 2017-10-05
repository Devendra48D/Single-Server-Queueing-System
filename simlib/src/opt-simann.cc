/////////////////////////////////////////////////////////////////////////////
//! \file  opt-simann.cc  Optimization algorithm - simulated annealing (simplified)
//
// Copyright (c) 2000-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

// EXPERIMENTAL
// simulated annealing optimization method

#include "simlib.h"
#include "internal.h"
#include "optimize.h"           // Param, ParameterVector
#include <cmath>                // exp()

#define debug 1                 // 0=NO, 1=OPTn, >1=ALL

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

//////////////////////////////////////////////////////////////////////////////
// optimization method
//

///////////////////////////////////////////////////////////////////////////////
// decision based on temperature
//
bool accept_bad(double eps)
{
    double p = exp(6.0 * (eps - 1.0));  // go down exponentialy
    double x = Random();        // <0,1)
    return (x <= p);
}

///////////////////////////////////////////////////////////////////////////////
// next random point
void move_to_next_point(ParameterVector & p, double eps)
{
    for (int i = 0; i < p.size(); i++) {
        double range = p[i].Range();
        double delta = range * (Random() - 0.5) * eps;
        p[i] = p[i] + delta;    // limited assignment
    }
}

///////////////////////////////////////////////////////////////////////////////
// simulated annealing method
//  - simplified version
//
double Optimize_simann(double (*f) (const ParameterVector & p),
                       ParameterVector & p, int MAXT)
{
    int bad_count = 0;
    double opt = 1e30;
    double xopt = opt;
    ParameterVector px(p);
    //bad_count = 0;              // statistic
    for (int temp = MAXT; temp > 0; temp--) {   // falling temperature
        // random step depends on temperature
        double eps = temp / double (MAXT);
        ParameterVector new_p = px;
        move_to_next_point(new_p, eps);
        // evaluate cost function
        double new_x = f(new_p);
#if debug>1                     // ALL points
        Print("%g %g %.12g\n", new_p["d"].Value(), new_p["k"].Value(), new_x);
#endif
        bool bad = false;
        if (new_x < xopt || (bad = accept_bad(eps))) {  // move to next point
            xopt = new_x;       // can be worse
            px = new_p;
            if (bad)
                ++bad_count;
#if debug==3
            Print("# %shil step accepted: ", bad ? "up" : "down");
            Print("%g %g %.12g\n", new_p["d"].Value(), new_p["k"].Value(),
                  new_x);
#endif
        }
        if (new_x < opt) {      // accept new temporary optimum
            opt = new_x;
            p = new_p;
#if debug==1                    // optima only
            Print("%g %g %.12g\n", p["d"].Value(), p["k"].Value(), opt);
#endif
        }
    }
#if debug
    Print("# %d accepted uphill steps\n", bad_count);
#endif
    return opt;                 // return optimal value and p
}

}
// end
