/////////////////////////////////////////////////////////////////////////////
// ni_rkf3.cc
//
// Copyright (c) 1996-1997 David Leska
// Copyright (c) 1998-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  numerical integration: Runge-Kutta-Fehlberg's method 3rd order
//

////////////////////////////////////////////////////////////////////////////
//  interface
//
#include "simlib.h"
#include "internal.h"
#include "ni_rkf3.h"
#include <cmath>
#include <cstddef>


////////////////////////////////////////////////////////////////////////////
//  implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;


////////////////////////////////////////////////////////////////////////////
//  Runge-Kutta-Fehlberg's method 3rd order
//
/*   Formula:

     k1  = h * f(t, y);
     k2  = h * f(t + 0.5*h, y + 0.5*k1);
     k3  = h * f(t + 0.75*h, y + 0.75*k2);
     y  += (2.0*k1 + 3.0*k2 + 4.0*k3) / 9.0;
     t  += h;
     err = |-5.0*k1 + 6.0*k2 + 8.0*k3 - 9.0*h*f(t, y)| / 72.0;
*/

void RKF3::Integrate(void)
{
  const double safety = 0.9; // keeps the new step from growing too large
  const double max_ratio = 4.0;  // ditto
  const double pshrnk = 0.5;     // coefficient for reducing step
  const double pgrow  = 1.0/3.0; // coefficient for increasing step
  register size_t i;   // auxiliary variables for loops
  Iterator ip, end_it; // to go through list of integrators
  double ratio;     // ratio for next step computation
  double next_step; // recommended stepsize for next step
  size_t n;         // integrator with greatest error

  Dprintf((" RKF3 integration step ")); // print debugging info
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
    A1[i]  = SIMLIB_StepSize*(*ip)->GetOldDiff(); // compute coefficient
    (*ip)->SetState((*ip)->GetOldState() + 0.5*A1[i]); // state (y) for next sub-step
  }

  ////////////////////////////////////////////////////////////// 1/2 of step

  _SetTime(Time,SIMLIB_StepStartTime + 0.5*SIMLIB_StepSize); // substep's time
  SIMLIB_DeltaTime = double(Time) - SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model (y'=f(t,y))      (1)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A2[i]  = SIMLIB_StepSize*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() + 0.75*A2[i]);
  }

  ////////////////////////////////////////////////////////////// 3/4 of step

  _SetTime(Time,SIMLIB_StepStartTime + 0.75*SIMLIB_StepSize); //substep's time
  SIMLIB_DeltaTime = double(Time) - SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model                  (2)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A3[i]  = SIMLIB_StepSize*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState()
                 + (2.0*A1[i] + 3.0*A2[i] + 4.0*A3[i]) / 9.0);
  }

  ////////////////////////////////////////////////////////////// 1.0 of step

  _SetTime(Time, SIMLIB_StepStartTime+SIMLIB_StepSize); // goto end time point
  SIMLIB_DeltaTime = SIMLIB_StepSize;

  SIMLIB_Dynamic();  // evaluate new state of model                  (3)

  //--------------------------------------------------------------------------
  //  Check on accuracy of numerical integration, estimate error
  //--------------------------------------------------------------------------

  SIMLIB_ERRNO = 0; // OK
  ratio = 8.0;      // 2^3 - ratio for next step computation - initial value
  n=0;              // integrator with greatest error
  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    double eerr; // estimated error
    double terr; // greatest allowed error

    eerr = fabs(  -5.0*A1[i]  // estimation
                 + 6.0*A2[i]
                 + 8.0*A3[i]
                 - 9.0*SIMLIB_StepSize*(*ip)->GetDiff()
               ) / 72.0;
    terr = fabs(SIMLIB_AbsoluteError)
         + fabs(SIMLIB_RelativeError*(*ip)->GetState());
    if(terr < eerr*ratio) { // avoid arithmetic overflow
      ratio = terr/eerr;    // find the lowest ratio
      n=i;                  // remember the integrator
    }
  } // for

  Dprintf(("R: %g",ratio));

  if(ratio < 1.0) { // error is too large, reduce stepsize
    ratio = pow(ratio,pshrnk);              // coefficient for reduce
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

  // increase step, if accuracy was good
  SIMLIB_OptStep = next_step;

} // RKF3::Integrate

}
// end of ni_rkf3.cc

