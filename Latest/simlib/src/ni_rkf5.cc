/////////////////////////////////////////////////////////////////////////////
// ni_rkf5.cc
//
// Copyright (c) 1996-1997 David Leska
// Copyright (c) 1998-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  numerical integration: Runge-Kutta-Fehlberg's method 5th order
//

////////////////////////////////////////////////////////////////////////////
//  interface
//
#include "simlib.h"
#include "internal.h"
#include "ni_rkf5.h"
#include <cmath>
#include <cstddef>


////////////////////////////////////////////////////////////////////////////
//  implementation
//
namespace simlib3 {

SIMLIB_IMPLEMENTATION;


////////////////////////////////////////////////////////////////////////////
//  Runge-Kutta-Fehlberg's method 5th order
//
/*  Formula:

    k1  = h*f(t,y);
    k2  = h*f(t+0.2*h,  y + 0.2*k1);
    k3  = h*f(t+0.3*h,  y + (3.0*k1+9.0*k2)/40.0);
    k4  = h*f(t+0.6*h,  y + 0.3*k1 - 0.9*k2 + 1.2*k3);
    k5  = h*f(t+h,      y - 11.0/54.0*k1 + 2.5*k2
                          - 70.0/27.0*k3 + 35.0/27.0*k4);
    k6  = h*f(t+0.875*h,y +  1631.0 /  55296.0 * k1
                          +   175.0 /    512.0 * k2
                          +   575.0 /  13824.0 * k3
                          + 44275.0 / 110592.0 * k4
                          +   253.0 /   4096.0 * k5);
    y  +=   37.0 /  378.0 * k1
         + 250.0 /  621.0 * k3
         + 125.0 /  594.0 * k4
         + 512.0 / 1771.0 * k6;
    err = fabs(  -277.0 /  64512.0 * k1
               + 6925.0 / 370944.0 * k3
               - 6925.0 / 202752.0 * k4
               -  277.0 /  14336.0 * k5
               +  277.0 /   7084.0 * k6);
*/

void RKF5::Integrate(void)
{
  const double safety = 0.9; // keeps the new step from growing too large
  const double max_ratio = 4.0; // ditto
  const double pshrnk = 0.25;   // coefficient for reducing step
  const double pgrow  = 0.20;   // coefficient for increasing step
  register size_t i;   // auxiliary variables for loops to go through list
  Iterator ip, end_it; // of integrators
  double ratio;     // ratio for next step computation
  double next_step; // recommended stepsize for next step
  size_t n;       // integrator with the greatest error

  Dprintf((" RKF5 integration step ")); // print debugging info
  Dprintf((" Time = %g, optimal step = %g", (double)Time, OptStep));

  end_it=LastIntegrator(); // end of container of integrators

  //--------------------------------------------------------------------------
  //  Step of method
  //--------------------------------------------------------------------------

begin_step:

  ///////////////////////////////////////////////////////// beginning of step

  SIMLIB_StepSize = max(SIMLIB_StepSize, SIMLIB_MinStep); // low step limit

  SIMLIB_ContractStepFlag = false;           // clear reduce step flag
  SIMLIB_ContractStep = 0.5*SIMLIB_StepSize; // implicitly reduce to half step

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A1[i] = SIMLIB_StepSize*(*ip)->GetOldDiff(); // compute coefficient
    (*ip)->SetState((*ip)->GetOldState() + 0.2*A1[i]); // state (y) for next sub-step
  }

  ////////////////////////////////////////////////////////////// 0.2 of step

  _SetTime(Time,SIMLIB_StepStartTime + 0.2*SIMLIB_StepSize); // substep's time
  SIMLIB_DeltaTime = double(Time) - SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model (y'=f(t,y))      (1)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A2[i] = SIMLIB_StepSize*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() + (3.0*A1[i] + 9.0*A2[i]) / 40.0);
  }

  ////////////////////////////////////////////////////////////// 0.3 of step

  _SetTime(Time,SIMLIB_StepStartTime + 0.3*SIMLIB_StepSize); //substep's time
  SIMLIB_DeltaTime = double(Time) - SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model                  (2)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A3[i] = SIMLIB_StepSize*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() + 0.3 * A1[i] - 0.9 * A2[i] + 1.2 * A3[i]);
  }

  ////////////////////////////////////////////////////////////// 0.6 of step

  _SetTime(Time, SIMLIB_StepStartTime+0.6*SIMLIB_StepSize);
  SIMLIB_DeltaTime = double(Time)-SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model                  (3)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A4[i] = SIMLIB_StepSize*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() - 11.0 / 54.0 * A1[i]
                                   +  2.5        * A2[i]
                                   - 70.0 / 27.0 * A3[i]
                                   + 35.0 / 27.0 * A4[i]);
  }

  ////////////////////////////////////////////////////////////// 1.0 of step

  _SetTime(Time, SIMLIB_StepStartTime+SIMLIB_StepSize);
  SIMLIB_DeltaTime = double(Time)-SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model                  (4)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A5[i] = SIMLIB_StepSize*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() +  1631.0 /  55296.0 * A1[i]
                                   +   175.0 /    512.0 * A2[i]
                                   +   575.0 /  13824.0 * A3[i]
                                   + 44275.0 / 110592.0 * A4[i]
                                   +   253.0 /   4096.0 * A5[i]);
  }

  ///////////////////////////////////////////////////////////// 0.875 of step

  _SetTime(Time, SIMLIB_StepStartTime+0.875*SIMLIB_StepSize);
  SIMLIB_DeltaTime = double(Time)-SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model                  (5)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A6[i] = SIMLIB_StepSize*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() +  37.0 /  378.0 * A1[i] // final state
                                   + 250.0 /  621.0 * A3[i]
                                   + 125.0 /  594.0 * A4[i]
                                   + 512.0 / 1771.0 * A6[i]);
  }

  ////////////////////////////////////////////////////////////// end of step

  _SetTime(Time, SIMLIB_StepStartTime+SIMLIB_StepSize); // go to end of step
  SIMLIB_DeltaTime = SIMLIB_StepSize;
  SIMLIB_Dynamic();

  //--------------------------------------------------------------------------
  //  Check on accuracy of numerical integration, estimate error
  //--------------------------------------------------------------------------

  SIMLIB_ERRNO = 0; // OK
  ratio = 32.0;     // 2^5 - ratio for stepsize computation - initial value
  n=0;              // integrator with greatest error
  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    double eerr; // estimated error
    double terr; // greatest allowed error

    eerr = fabs(  -277.0 /  64512.0 * A1[i] // estimation
                + 6925.0 / 370944.0 * A3[i]
                - 6925.0 / 202752.0 * A4[i]
                -  277.0 /  14336.0 * A5[i]
                +  277.0 /   7084.0 * A6[i]);
    terr = fabs(SIMLIB_AbsoluteError)
         + fabs(SIMLIB_RelativeError*(*ip)->GetState());
    if(terr < eerr*ratio) { // avoid arithmetic overflow
      ratio = terr/eerr;    // find the lowest ratio
      n=i;                  // remember the integrator
    }
  } // for

  Dprintf(("R: %g",ratio));

  if(ratio < 1.0) { // error is too large, reduce stepsize
    ratio = pow(ratio,pshrnk); // coefficient for reduce
    Dprintf(("Down: %g",ratio));
    if(SIMLIB_StepSize > SIMLIB_MinStep) {  // reducing step is possible
      SIMLIB_OptStep = max(safety*ratio*SIMLIB_StepSize, SIMLIB_MinStep);
      SIMLIB_StepSize = SIMLIB_OptStep;
      IsEndStepEvent = false; // no event will be at the end of the step
      goto begin_step;        // compute again with smaller step
    }
    // reducing step is unpossible
    SIMLIB_ERRNO++;          // requested accuracy cannot be achieved
    _Print("\n Integrator[%lu] ",(unsigned long)n);
    SIMLIB_warning(AccuracyError);
    next_step = SIMLIB_StepSize;
  } else { // allowed tolerantion is fulfiled
    if(!IsStartMode()) { // method is not used for start multi-step method
      ratio = min(pow(ratio,pgrow),max_ratio); // coefficient for increase
      Dprintf(("Up: %g",ratio));
      next_step = min(safety*ratio*SIMLIB_StepSize, SIMLIB_MaxStep);
    } else {
      next_step = SIMLIB_StepSize;
    }
  }

  //--------------------------------------------------------------------------
  //  Analyse system at the end of the step
  //--------------------------------------------------------------------------

  if(StateCond()) { // check on changes of state conditions at end of step
    goto begin_step;
  }

  //--------------------------------------------------------------------------
  //  Results of step have been accepted, take fresh step
  //--------------------------------------------------------------------------

  // increase step, if accuracy is good
  SIMLIB_OptStep = next_step;

} // RKF5::Integrate


}
// end of ni_rkf5.cc

