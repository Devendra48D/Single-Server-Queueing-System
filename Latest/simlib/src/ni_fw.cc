/////////////////////////////////////////////////////////////////////////////
// ni_fw.cc
//
// Copyright (c) 1996-1997 David Leska
// Copyright (c) 1998-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  numerical integration: Fowler-Warten's method
//

////////////////////////////////////////////////////////////////////////////
//  interface
//
#include "simlib.h"
#include "internal.h"
#include "ni_fw.h"
#include <cmath>
#include <cfloat>
#include <cstddef>


////////////////////////////////////////////////////////////////////////////
//  implementation
//
namespace simlib3 {

SIMLIB_IMPLEMENTATION;


////////////////////////////////////////////////////////////////////////////
//  Fowler-Warten's method
//
/*  Formula: Description needs graphic, see documentation. */

void FW::Integrate(void)
{
  const int eul_max_count    = 7; // avoids step growing too quickly for E
  const double eul_err_coef  = 1.0/150.0; // limits an error range
  const double eul_step_coef = 0.25; // he = h * min_h
  const double eul_step_rat  = 0.01; // he = h * rat
  const int fw_max_count     = 7; // avoids step growing too quickly for FW
  const double fw_err_coef   = 1.0/150.0; // limits an error range
  const double fw_err_rnghi  = 1.5;  // ranges for step accuracy
  const double fw_err_rnglo  = 0.75; // and error estimation
  register size_t i;   // auxiliary variables for loops to go through list
  Iterator ip, end_it; // of integrators
  bool EulDoubleStepFlag; // allow increasing (doubling) the E. substepsize
  bool FWDoubleStepFlag;  // allow increasing (doubling) the FW. stepsize
  bool FWHalveStepFlag;   // allow reducing (halving) the FW. stepsize
  bool FWMayDouble;       // accuracy has been very good
  // WARNING: following variables must be static!
  // others are static only for efficiency
  static int FWDoubleCount;   // counter of doubling step requestes in FW
  static int EulDoubleCount;  // counter of doubling step requestes in Euler
  static double Eul_StepSize; // step of Euler's method
  static double PrevStep;     // previous FW step

  Dprintf((" Fowler-Warten integration step ")); // print debugging info
  Dprintf((" Time = %g, optimal step = %g", (double)Time, OptStep));

  end_it=LastIntegrator(); // end of container of integrators

  FWDoubleStepFlag  = true;  // allow doubling for FW
  EulDoubleStepFlag = true; // allow doubling for Euler
  FWHalveStepFlag   = false; // disable halving

  if(FW_First) { // method is called first time -> authomatic start
    FWDoubleCount  = 0;
    EulDoubleCount = 0;
    Eul_StepSize   = eul_step_rat*SIMLIB_StepSize;
  }

  //--------------------------------------------------------------------------
  //  Step of method
  //--------------------------------------------------------------------------

begin_step: // beginning of step

  SIMLIB_StepSize = max(SIMLIB_StepSize, SIMLIB_MinStep); // low step limit
  SIMLIB_ContractStepFlag = false; // clear reduce step flag
  SIMLIB_ContractStep = 0.5*SIMLIB_StepSize; // reduce to half step

  //--------------------------------------------------------------------------
  //  Substep of Euler's method
  //--------------------------------------------------------------------------

euler_step: // beginning of Euler's step

  Eul_StepSize = max(Eul_StepSize, eul_step_coef*SIMLIB_MinStep); // low limit
  Eul_StepSize = min(Eul_StepSize, eul_step_coef*SIMLIB_StepSize); // high

  Dprintf(("E_MIN: %g, E_MAX %g", eul_step_coef*SIMLIB_MinStep,
          eul_step_coef*SIMLIB_StepSize));

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    // state y(t+he) = y + he * y'
    (*ip)->SetState((*ip)->GetOldState()+Eul_StepSize*(*ip)->GetOldDiff());
  }

  _SetTime(Time,SIMLIB_StepStartTime + Eul_StepSize); // set time to t+he
  SIMLIB_DeltaTime = Eul_StepSize;
  SIMLIB_Dynamic();  // y'(t+he)=f(t+he, y(t+he))

  //--------------------------------------------------------------------------
  //  Check on accuracy of Euler's integration, estimate error
  //--------------------------------------------------------------------------

  SIMLIB_ERRNO = 0; // OK
  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    double eerr; // estimated error
    double terr; // greatest allowed error

    // error estimation
    eerr = Eul_StepSize*fabs((*ip)->GetDiff() - (*ip)->GetOldDiff());
    terr = SIMLIB_AbsoluteError + fabs(SIMLIB_RelativeError*(*ip)->GetState());

    if(eerr < eul_err_coef*terr) // tolerantion is fulfiled with provision
      continue;

    EulDoubleStepFlag = false; // disable increasing Eul_StepSize

    if(eerr > terr) { // allowed tolerantion is overfulfiled
      if(Eul_StepSize>eul_step_coef*SIMLIB_MinStep) { // reducing is possible
        // halve Euler's step with limit
        Eul_StepSize = max(0.5*Eul_StepSize, eul_step_coef*SIMLIB_MinStep);
        goto euler_step; // compute again with smaller step
      }
      // reducing step is unpossible
      SIMLIB_ERRNO++; // requested accuracy cannot be achieved
      _Print("\n Integrator[%i] ",i);
    }
    if(SIMLIB_ConditionFlag) // event has been within the step
      break;
  } // for

  if(SIMLIB_ERRNO) {
    SIMLIB_warning(AccuracyError);
  }

  //--------------------------------------------------------------------------
  //  Analyse system at the end of the Euler's step
  //--------------------------------------------------------------------------

  if(StateCond()) { // check on state conditions changes within the step
    goto begin_step;
  }

  //--------------------------------------------------------------------------
  //  Results of Euler's step have been accepted, take fresh step
  //--------------------------------------------------------------------------

  Dprintf(("E_F: %d, E_C: %d",EulDoubleStepFlag, EulDoubleCount));

  // if accuracy has been good, increment counter
  if(EulDoubleStepFlag) {
    EulDoubleCount++;
  } else {
    EulDoubleCount = 0;
  }
  // if accuracy has been good max_count-times in previous steps,
  // increase step for Euler's method
  if(EulDoubleCount >= eul_max_count) {
    EulDoubleCount = 0;
    Eul_StepSize=min(eul_step_coef*SIMLIB_StepSize, 2.0*Eul_StepSize);
  }

  Dprintf(("E_S: %g", Eul_StepSize));

  //--------------------------------------------------------------------------
  //  End of Euler's substep, FW continues
  //--------------------------------------------------------------------------

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    double yia; // formula's coefficients
    double d1;
    double d2;
    double ll;
    double c1;
    double c0;
    double denom;

    yia = FW_First ? 0 : (((*ip)->GetOldState() - Y1[i]) / PrevStep);
    d1  = (*ip)->GetOldDiff() - yia;
    d2  = ((*ip)->GetDiff() - (*ip)->GetOldDiff())/Eul_StepSize;
    ll  = (d1<=prec && d1>=-prec) ? 0 : (d2/d1);
    denom = SIMLIB_StepSize * ll;
    c1  = (denom >= -prec)
          ? (1.0 + 0.5 * denom)
          : ((exp(denom) - 1.0) / denom);
    c0  = (ll>=0) ? (1.0 + denom)
                  : exp(denom);
    // state
    (*ip)->SetState((*ip)->GetOldState() + SIMLIB_StepSize * (yia + c1 * d1));
    ERR[i] = yia + c0 * d1;
  }

  _SetTime(Time,SIMLIB_StepStartTime + SIMLIB_StepSize); // set time to t+h
  SIMLIB_DeltaTime = SIMLIB_StepSize;
  SIMLIB_Dynamic(); // compute new state of model

  //--------------------------------------------------------------------------
  //  Check on accuracy of FW integration, estimate error
  //--------------------------------------------------------------------------

  FWMayDouble = false;
  SIMLIB_ERRNO = 0; // OK
  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    double eerr; // estimated error
    double terr; // greatest allowed error

    eerr = SIMLIB_StepSize*fabs((*ip)->GetDiff() - ERR[i]); // estimation
    terr = SIMLIB_AbsoluteError + fabs(SIMLIB_RelativeError*(*ip)->GetState());

    if(eerr > fw_err_rnghi*terr) {
      // allowed tolerantion is overfulfiled,
      // halve stepsize and compute again
      FWDoubleStepFlag = false;
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
    }
    else if(eerr > fw_err_rnglo*terr) {
      // accept results of current step,
      // halve stepsize for succeeding steps
      FWHalveStepFlag  = true;
      FWDoubleStepFlag = false;
    }
    else if(eerr < fw_err_coef*terr) {
      // step may be doubled
      FWMayDouble = true;
    }

    if(SIMLIB_ConditionFlag) // event has been within the step
      break;

  } // for

  FWDoubleStepFlag = FWDoubleStepFlag && FWMayDouble;

  if(SIMLIB_ERRNO) {
    SIMLIB_warning(AccuracyError);
  }

  //--------------------------------------------------------------------------
  //  Analyse system at the end of the step
  //--------------------------------------------------------------------------

  if(StateCond()) { // check on changes of state conditions at end of step
    goto begin_step;
  }

  //--------------------------------------------------------------------------
  //  Results of step have been accepted, store values and take fresh step
  //--------------------------------------------------------------------------

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    Y1[i] = (*ip)->GetOldDiff();
  }
  FW_First = false;
  PrevStep = SIMLIB_StepSize;

  // if accuracy hasn't been good, reduce step
  if(FWHalveStepFlag) { // halving takes precedence over doubling
    FWDoubleCount = 0;
    SIMLIB_OptStep = 0.5*SIMLIB_OptStep;
    Dprintf(("Reducing"));
  } // if accuracy has been good, increase counter
  else if(FWDoubleStepFlag) {
    FWDoubleCount++;
    Dprintf(("Incrementing"));
  } else {
    FWDoubleCount = 0;
    Dprintf(("Clearing"));
  }
  // if accuracy has been good step_coef-times in previous steps,
  // increase step for FW method
  if(FWDoubleCount >= fw_max_count) {
    FWDoubleCount = 0;
    SIMLIB_OptStep += SIMLIB_OptStep;
    Dprintf(("Doubling"));
  }
  SIMLIB_OptStep = min(SIMLIB_OptStep,SIMLIB_MaxStep);
  SIMLIB_OptStep = max(SIMLIB_OptStep,SIMLIB_MinStep);
  Dprintf(("Step: %g", SIMLIB_OptStep));

} // FW::Integrate


////////////////////////////////////////////////////////////////////////////
//  FW::PrepareStep
//  prepare object for integration step
//
bool FW::PrepareStep(void)
{
  Dprintf(("FW::PrepareStep()"));
  // prepare inherited part
  if(SingleStepMethod::PrepareStep()) {
    FW_First=true;  // method will have to be re-initialized
    return true;  // some changes
  }
  return false; // no changes
} // FW::PrepareStep


////////////////////////////////////////////////////////////////////////////
//  initialize static class member
//
const double FW::prec = DBL_EPSILON; // near zero number

}
// end of ni_fw.cc

