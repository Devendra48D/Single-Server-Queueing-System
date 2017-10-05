/////////////////////////////////////////////////////////////////////////////
//! \file opt-hooke.cc Optimization algorithm - Hooke-Jeves
//
// Copyright (c) 2000-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

// EXPERIMENTAL
// Hooke-Jeves optimization method

#include "simlib.h"
#include "internal.h"
#include "optimize.h"

#define debug 1                 // print values

//## repair
#include <math.h>

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

//////////////////////////////////////////////////////////////////////////////
// optimization method
//
// look for a better minimum, change one coord at a time
static double hooke_step(double *delta, opt_function_t f, ParameterVector & p,
                         double min0)
{
    int n = p.size();
    double fmin = min0;
    for (int i = 0; i < n; i++) {
        if (delta[i] == 0.0)
            continue;           // ignore zero delta
        double old = p[i];
        p[i] = old + delta[i];
        double ftmp = (p[i] == old) ? fmin : f(p);
        if (ftmp < fmin)
            fmin = ftmp;
        else {
            delta[i] = -delta[i];       // opposite direction
            p[i] = old + delta[i];
            ftmp = (p[i] == old) ? fmin : f(p);
            if (ftmp < fmin)
                fmin = ftmp;
            else
                p[i] = old;     // no change
        }
    }
    return fmin;
}

//////////////////////////////////////////////////////////////////////////////

double Optimize_hooke(opt_function_t f, ParameterVector & parameter,
                      double rho, double epsilon, int itermax)
{
// assert(rho>0.01 && rho <1.0);
// assert(epsilon>1e-12 && epsilon < rho);  // 1e-12 > 100*DBL_EPS
// assert(itermax>0);
    int n = parameter.size();   // number of parameters
// assert(n>0);
    double *delta = new double[n];
    ParameterVector oldx(parameter);
    ParameterVector newx(parameter);
    for (int i = 0; i < n; i++)
        delta[i] = fabs(parameter[i].Range() / 10);     // initial step 10==MPARAMETER-???
    int iteration = 0;
    double steplength = rho;    // 1.0 ???
    double newf = f(newx);
#if debug
    newx.PrintValues();
    Print("%g\n", newf);
#endif
    double oldf = newf;
    while (iteration < itermax && steplength > epsilon) {
        iteration++;
        newx = oldx;
        newf = hooke_step(delta, f, newx, oldf);
        // if we made some improvements, continue that direction
        while (newf < oldf) {
#if debug
            newx.PrintValues();
            Print("%g\n", newf);
#endif
            for (int i = 0; i < n; i++) {
                double dxi = newx[i] - oldx[i]; // actual difference
                // arrange the sign of delta[]
                delta[i] = (dxi <= 0.0) ? -fabs(delta[i]) : fabs(delta[i]);
                // move further in this direction
                oldx[i] = newx[i];
                newx[i] = newx[i] + dxi;
            }
            oldf = newf;
            newf = hooke_step(delta, f, newx, oldf);
            /* if the further (optimistic) move was bad.... */
            if (newf >= oldf)   // worse
                break;          ///////////////////// break
            /* !!!! make sure that the differences between the new and the old
             * points are due to actual displacements; beware of roundoff
             * errors that might cause newf < oldf
             */
            int i;
            for (i = 0; i < n; i++)
                if (fabs(newx[i] - oldx[i]) > (0.5 * fabs(delta[i])))
                    break;
            // if there is NOT actual change in at least one axis
            if (i == n)
                break;          ///////////////////// break
        }
        if (steplength >= epsilon && newf >= oldf) {
            steplength *= rho;
            for (int i = 0; i < n; i++)
                delta[i] *= rho;        // decrease delta
        }
    }
    delete[]delta;              //
    parameter = oldx;           // copy result
//    iterations = iteration;   // number of iterations
    return oldf;                // return last minimum value
}

}
// end
