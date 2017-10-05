/////////////////////////////////////////////////////////////////////////////
// ni_abm4.cc
//
// Copyright (c) 1996-1997 David Leska
// Copyright (c) 1998-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  numerical integration: Adams-Bashforth-Moulton's
//                         predictor-corrector 4th order
//

////////////////////////////////////////////////////////////////////////////
//  interface
//
#include "simlib.h"
#include "internal.h"
#include "ni_abm4.h"
#include <cmath>
#include <cstddef>


////////////////////////////////////////////////////////////////////////////
//  implementation
//
namespace simlib3 {

SIMLIB_IMPLEMENTATION;


////////////////////////////////////////////////////////////////////////////
//  Adams-Bashforth-Moulton's predictor-corrector 4th order
//
/*  Formula:

    for i = 0 to 2 loop
      z[i] = f(t,y)
      Self-start by rkf5
    end for
    z[3] = f(t,y)
    pred = y + (55*z[3] - 59*z[2] + 37*z[1] - 9*z[0]) * h/24
    for i = 0 to 2 loop
      z[i] = z[i+1]
    end for
    t += h
    z3 = f(t,pred)
    corr = y + (9*z3 + 19*z[2] - 5*z[1] + z[0]) * h/24
    err = 0.5 * |pred - corr|
*/

void ABM4::Integrate(void)
{
  const double err_lo = 0.75; // limits an error range
  const double err_hi = 1.00; // limits an error range
  const int max_dbl = 8; // avoid stepsize growing too quickly
  register size_t i;   // auxiliary variables
  Iterator ip, end_it; // for loops to go through container of integrators
  bool DoubleStepFlag; // allows doubling step
  // WARNING: following variables must be static !!!
  static double PrevStep; // previous stepsize
  static int ind = 0; // base index to arrays with values from previous steps
  static int DoubleCount = 0; // number of good steps for doubling stepsize

  Dprintf((" ABM4 integration step ")); // print debugging info
  Dprintf((" Time = %g, optimal step = %g", (double)Time, OptStep));

  //--------------------------------------------------------------------------
  //  Step of method
  //--------------------------------------------------------------------------

  end_it=LastIntegrator(); // end of container of integrators
  DoubleStepFlag = true; // allow doubling stepsize

begin_step:

  SIMLIB_StepSize = max(SIMLIB_StepSize, SIMLIB_MinStep); // low step limit

  if(ABM_Count>0 && PrevStep!=SIMLIB_StepSize) { // stepsize has been changed
    ABM_Count = 0;
    Dprintf(("NEW START, Time = %g",(double)Time));
  }
  PrevStep = SIMLIB_StepSize;

  Dprintf(("counter: %d, Time = %g",ABM_Count,(double)Time));

    //-----------------------------------------------------------------------
    //  method must be started
    //-----------------------------------------------------------------------

  if(ABM_Count <= abm_ord-2) {
    Dprintf(("start, step = %g, Time = %g",SIMLIB_StepSize,(double)Time));
    ind = 0;
    DoubleCount = 0;
    for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
      Z[ABM_Count][i] = (*ip)->GetOldDiff();  // store values for next steps
    }
    ABM_Count++;  // increment counter of starts
    SlavePtr()->Integrate();  // call starting method (slave)

    //-----------------------------------------------------------------------
    //  predictor-corrector
    //-----------------------------------------------------------------------

  } else {
    SIMLIB_ContractStepFlag = false; // clear reduce step flag
    SIMLIB_ContractStep = 0.5*SIMLIB_StepSize; // reduce to quater of step
    Dprintf(("own-method, step = %g, Time = %g",
             SIMLIB_StepSize,(double)Time));

    //-----------------------------------------------------------------------
    //  compute predictor
    //-----------------------------------------------------------------------

    for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
      // store values for next steps
      Z[(ind+3)%abm_ord][i] = (*ip)->GetOldDiff();
      // predictor
      (*ip)->SetState( PRED[i] = (*ip)->GetOldState() +
                    (   55.0 * Z[(ind+3)%abm_ord][i]
                      - 59.0 * Z[(ind+2)%abm_ord][i]
                      + 37.0 * Z[(ind+1)%abm_ord][i]
                      -  9.0 * Z[ind][i]
                    ) * (SIMLIB_StepSize / 24.0)
                  );
    }

    _SetTime(Time,SIMLIB_StepStartTime + SIMLIB_StepSize); // endpoint time
    SIMLIB_DeltaTime = double(Time) - SIMLIB_StepStartTime;
    SIMLIB_Dynamic();  // evaluate new state of model
    ind=(ind+1)%abm_ord; // increment base index

    //-----------------------------------------------------------------------
    //  compute corrector
    //-----------------------------------------------------------------------

    for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
      (*ip)->SetState( (*ip)->GetOldState()
                    + (    9.0 * (*ip)->GetDiff()
                        + 19.0 * Z[(ind+2)%abm_ord][i]
                        -  5.0 * Z[(ind+1)%abm_ord][i]
                        +        Z[ind][i]
                      ) * (SIMLIB_StepSize / 24.0)
                  );
    }

    //-----------------------------------------------------------------------
    //  estimate error
    //-----------------------------------------------------------------------

    SIMLIB_ERRNO = 0;
    for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
      double eerr; // estimated error
      double terr; // greatest allowed error

      eerr = 0.5 * fabs(PRED[i] - (*ip)->GetState()); // error estimation
      terr = SIMLIB_AbsoluteError + fabs(SIMLIB_RelativeError*(*ip)->GetState());

      if(eerr < err_lo*terr) // tolerantion is fulfiled with provision
        continue;

      if(eerr > err_hi*terr) { // tolerantion is overfulfiled
        if(SIMLIB_StepSize>SIMLIB_MinStep) {     // reducing step is possible
          SIMLIB_OptStep = 0.25*SIMLIB_StepSize; // quater optimal step
          if(SIMLIB_OptStep < SIMLIB_MinStep) {  // limit of optimal step
            SIMLIB_OptStep = SIMLIB_MinStep;
          }
          SIMLIB_StepSize = SIMLIB_OptStep;
          IsEndStepEvent = false;
          goto begin_step; // compute again with smaller step
        }
        // reducing step is unpossible
        SIMLIB_ERRNO++;          // requested accuracy cannot be achieved
        _Print("\n Integrator[%i] ",i);
        if(SIMLIB_ConditionFlag) // event was in half step, step reducing was
          break;                 // unpossible and accuracy cannot be achieved
      }
      DoubleStepFlag = false;    // disable increasing SIMLIB_OptStep,
    }                            // accuracy is sufficient, but not well
    if(SIMLIB_ERRNO) {
      SIMLIB_warning(AccuracyError);
    }

    //-----------------------------------------------------------------------
    //  Analyse system at the end of the step
    //-----------------------------------------------------------------------

    if(StateCond()) { // check on changes of state conditions at end of step
      goto begin_step;
    }

    //-----------------------------------------------------------------------
    //  Results of step have been accepted, take fresh step
    //-----------------------------------------------------------------------

    // if accuracy has been good, increment counter
    if(DoubleStepFlag) {
      DoubleCount++;
    } else {
      DoubleCount = 0;
    }
    // if accuracy has been good max_dbl-times in previous steps,
    // increase stepsize
    if(DoubleCount >= max_dbl) {
      DoubleCount = 0;
      SIMLIB_OptStep=min(SIMLIB_MaxStep, 2.0*SIMLIB_StepSize);
    }
  }
} // ABM4::Integrate


////////////////////////////////////////////////////////////////////////////
//  ABM4::PrepareStep
//  prepare object for integration step
//
bool ABM4::PrepareStep(void)
{
  Dprintf(("ABM4::PrepareStep()"));
  // prepare inherited part
  if(MultiStepMethod::PrepareStep()) {
    ABM_Count = 0;  // method will have to be restarted
    return true;  // some changes
  }
  return false; // no changes
} // ABM4::PrepareStep

}
// end

