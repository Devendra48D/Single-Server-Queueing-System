/////////////////////////////////////////////////////////////////////////////
// ni_euler.cc
//
// Copyright (c) 1991-2004 Petr Peringer
// Copyright (c) 1996-1997 David Leska
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  numerical integration: Euler's method
//

////////////////////////////////////////////////////////////////////////////
//  interface
//
#include "simlib.h"
#include "internal.h"
#include "ni_euler.h"
#include <cmath>
#include <cstddef>


////////////////////////////////////////////////////////////////////////////
//  implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;


////////////////////////////////////////////////////////////////////////////
//  Euler's method
//
/*   Formula:

     y'(t)     = f(t, y);
     y(t+h/2)  = y(t) + h/2 * y'(t);
     y'(t+h/2) = f(t+h/2, y(t+h/2));
     y(t+h)    = y(t+h/2) + h/2 * y'(t+h/2);
     y'(t+h)   = f(t+h, y(t+h));
     err       = h * |y'(t) - y'(t+h/2)|;
*/

void EULER::Integrate(void)
{
  const double err_coef = 0.02; // limits an error range
  static double dthlf;   // half step
  register size_t i;   // auxiliary variables for loops to go through list
  Iterator ip, end_it; // of integrators
  static bool DoubleStepFlag; // flag - allow increasing (doubling) the step

  Dprintf((" Euler integration step ")); // print debugging info
  Dprintf((" Time = %g, optimal step = %g", (double)Time, OptStep));

  end_it=LastIntegrator(); // end of container of integrators

  //--------------------------------------------------------------------------
  //  Step of method
  //--------------------------------------------------------------------------

begin_step: // beginning of step

  SIMLIB_StepSize = max(SIMLIB_StepSize, SIMLIB_MinStep); // low step limit

  dthlf = 0.5*SIMLIB_StepSize;     // half step

  SIMLIB_ContractStepFlag = false; // clear reduce step flag
  SIMLIB_ContractStep = 0.5*dthlf; // implicitly reduce to half

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A[i]   = (*ip)->GetOldDiff();
    (*ip)->SetState((*ip)->GetOldState() + dthlf*(*ip)->GetDiff());   // state y(t+h/2)
  }

  ////////////////////////////////////////////////////////////// 1/2 of step

  _SetTime(Time, SIMLIB_StepStartTime+dthlf);
  SIMLIB_DeltaTime = double(Time)-SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // compute new state of model                  (1)

  if(StateCond()) { // check on changes of state conditions in 1/2 of step
    goto begin_step;
  }

  bool wasContractStepFlag = SIMLIB_ContractStepFlag; // remember value
  SIMLIB_ContractStepFlag = false; // not reduce step
  SIMLIB_ContractStep = dthlf;     // implicitly reduce to half of step

  StoreState(di, si, xi); // store values in 1/2 of step

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    // difference of differentiations for error estimation
    A[i] -= (*ip)->GetDiff();
    (*ip)->SetState(si[i] + dthlf*(*ip)->GetDiff());
  }

  //////////////////////////////////////////////////////////// end of step

  _SetTime(Time, SIMLIB_StepStartTime + SIMLIB_StepSize);
  SIMLIB_DeltaTime = SIMLIB_StepSize;

  SIMLIB_Dynamic();  // compute new state of model                  (2)

  //--------------------------------------------------------------------------
  //  Check on accuracy of numerical integration, estimate error
  //--------------------------------------------------------------------------

  DoubleStepFlag = true; // allow doubling the step
  SIMLIB_ERRNO = 0; // OK
  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    double eerr; // estimated error
    double terr; // greatest allowed error

    eerr = fabs(SIMLIB_StepSize*A[i]); // error estimation
    terr = SIMLIB_AbsoluteError + fabs(SIMLIB_RelativeError*si[i]);

    if(eerr < err_coef*terr) // allowed tolerantion is fulfiled with provision
      continue;

    if(eerr > terr) {        // allowed tolerantion is overfulfiled
      if(SIMLIB_StepSize > SIMLIB_MinStep) {  // reducing step is possible
        SIMLIB_OptStep = 0.5*SIMLIB_StepSize; // halve optimal step
        if(SIMLIB_OptStep < SIMLIB_MinStep) { // limit of optimal step
          SIMLIB_OptStep = SIMLIB_MinStep;
        }
        SIMLIB_StepSize = SIMLIB_OptStep;
        IsEndStepEvent = false;
        goto begin_step; // compute again with smaller step
      }
      // reducing step is unpossible
      SIMLIB_ERRNO++; // requested accuracy cannot be achieved
      _Print("\n Integrator[%lu] ",(unsigned long)i);
      if(SIMLIB_ConditionFlag) // event has been within the step
        break;
    }

    DoubleStepFlag = false;  // disable increasing SIMLIB_OptStep,
                             // accuracy is sufficient, but not well
  } // for

  if(SIMLIB_ERRNO) {
    SIMLIB_warning(AccuracyError);
  }

  //--------------------------------------------------------------------------
  //  Computation is continuing, compute y(t+h)
  //--------------------------------------------------------------------------

  if(wasContractStepFlag) {
    // step reducing has been requested in half step and it is unpossible
    RestoreState(dthlf, di, si, xi); // restore halfstep state
  } else { // go to half step and complete the computation

    GoToState(di, si, xi);

    SIMLIB_StepStartTime += dthlf;
    SIMLIB_DeltaTime = double(Time) - SIMLIB_StepStartTime;

    //-----------------------------------------------------------------------
    //  Analyse system at the end of the step
    //-----------------------------------------------------------------------

    if(StateCond()) { // check on changes of state conditions at end of step
      goto begin_step;
    }
  }

  //--------------------------------------------------------------------------
  //  Results of step have been accepted, take fresh step
  //--------------------------------------------------------------------------

  // increase step, if accuracy was good
  // step increasing is allowed
  // && method is not used to start multi-step method
  if(DoubleStepFlag && !IsStartMode()) {
    SIMLIB_OptStep += SIMLIB_OptStep; // step doubling
  }
  SIMLIB_OptStep = min(SIMLIB_OptStep,SIMLIB_MaxStep); // limit step size

} // EULER::Integrate


}

