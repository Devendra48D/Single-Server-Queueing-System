/////////////////////////////////////////////////////////////////////////////
//! \file ni_rke.cc   Numerical integration method - Runge-Kutta-England
//
// Copyright (c) 1991-2004 Petr Peringer
// Copyright (c) 1996-1997 David Leska
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  numerical integration: Runge-Kutta-England's method
//  this method is the default
//

////////////////////////////////////////////////////////////////////////////
//  interface
//
#include "simlib.h"
#include "internal.h"
#include "ni_rke.h"
#include <cmath>
#include <cstddef>


////////////////////////////////////////////////////////////////////////////
//  implementation
//
namespace simlib3 {

SIMLIB_IMPLEMENTATION;


////////////////////////////////////////////////////////////////////////////
//  Runge-Kutta-England's method
//
/*   Formula:

     dt=0.5*h;
     a1=dt*f(t,y);
     a2=dt*f(t+0.5*dt,y+0.5*a1);
     a3=dt*f(t+0.5*dt,y+0.25*(a1+a2));
     t+=dt;
     a4=dt*f(t,y+(2.0*a3-a2));
     yt1=y+((a1+a4)+4.0*a3)/6.0;
     a5=dt*f(t,yt1);
     a6=dt*f(t+0.5*dt,yt1+0.5*a5);
     a7=dt*f(t+0.5*dt,yt1+0.25*(a5+a6));
     t+=dt;
     r=dt*f(t,y+(-a1-96.0*a2+92.0*a3-121.0*a4+144.0*a5+6.0*a6-12.0*a7)/6.0);
     err=fabs((-a1+4.0*a3+17.0*a4-23.0*a5+4.0*a7-r)/90.0);
     yt2 = yt1+(a5+4.0*a7+dt*f(t,yt1+(2.0*a7-a6)))/6.0;
*/

void RKE::Integrate(void)
{
  static const double err_coef = 0.02; // limits an error range
  static double dthlf;         // half step
  static double dtqrt;         // quater step
  static bool DoubleStepFlag;  // flag - allow increasing (doubling) the step
  register size_t i;   // auxiliary variables for loops to go through list
  Iterator ip, end_it; // of integrators

  Dprintf((" RKE integration step ")); // print debugging info
  Dprintf((" Time = %g, optimal step = %g", (double)Time, OptStep));

  end_it=LastIntegrator(); // end of container of integrators

  //--------------------------------------------------------------------------
  //  Step of method
  //--------------------------------------------------------------------------

begin_step:

  ///////////////////////////////////////////////////////// beginning of step

  SIMLIB_StepSize = max(SIMLIB_StepSize, SIMLIB_MinStep); // low step limit
  dthlf = 0.5*SIMLIB_StepSize; // half step
  dtqrt = 0.5*dthlf;           // quater step

  SIMLIB_ContractStepFlag = false; // clear reduce step flag
  SIMLIB_ContractStep = dtqrt;     // implicitly reduce to quater of step

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A1[i] = dthlf*(*ip)->GetOldDiff();     // compute coefficient
    (*ip)->SetState((*ip)->GetOldState()+0.5*A1[i]); // state (y) for next sub-step
  }

  ////////////////////////////////////////////////////////////// 1/4 of step

  _SetTime(Time,SIMLIB_StepStartTime + dtqrt); // time (t) for next sub-step
  SIMLIB_DeltaTime = double(Time) - SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model (y'=f(t,y))      (1)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A2[i] = dthlf*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() + 0.25*(A1[i]+A2[i]));
  }

  SIMLIB_Dynamic();  // evaluate new state of model                  (2)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A3[i] = dthlf*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() - A2[i] + A3[i] + A3[i]);
  }

  //////////////////////////////////////////////////////////////
  //                       1/2 of step
  //////////////////////////////////////////////////////////////

  _SetTime(Time, SIMLIB_StepStartTime+dthlf);
  SIMLIB_DeltaTime = double(Time)-SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model                  (3)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A4[i] = dthlf*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() + (A1[i] + 4.0*A3[i] + A4[i]) / 6.0);
  }

  if(StateCond()) { // check on changes of state conditions in 1/2 of step
    goto begin_step; // compute again
  }

  bool wasContractStepFlag = SIMLIB_ContractStepFlag; // remember value
  SIMLIB_ContractStepFlag = false; // not reduce step
  SIMLIB_ContractStep = dthlf;     // implicitly reduce to half of step

  StoreState(di, si, xi); // store  values in 1/2 of step

  SIMLIB_Dynamic();  // evaluate new state of model                  (4)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A5[i] = dthlf*(*ip)->GetDiff();
    (*ip)->SetState(si[i] + 0.5*A5[i]);
  }

  ////////////////////////////////////////////////////////////// 3/4 of step

  _SetTime(Time, SIMLIB_StepStartTime+dthlf+dtqrt);
  SIMLIB_DeltaTime = double(Time)-SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model                  (5)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A6[i] = dthlf * (*ip)->GetDiff();
    (*ip)->SetState(si[i] + 0.25*(A5[i] + A6[i]));
  }

  SIMLIB_Dynamic();  // evaluate new state of model                  (6)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A7[i] = dthlf*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState()
      + (         - A1[i]
          -  96.0 * A2[i]
          +  92.0 * A3[i]
          - 121.0 * A4[i]
          + 144.0 * A5[i]
          +   6.0 * A6[i]
          -  12.0 * A7[i]
        ) / 6.0);
  }

  //////////////////////////////////////////////////////////// end of step

  _SetTime(Time, SIMLIB_StepStartTime + SIMLIB_StepSize);
  SIMLIB_DeltaTime = SIMLIB_StepSize;

  SIMLIB_Dynamic();  // evaluate new state of model                  (7)

  //--------------------------------------------------------------------------
  //  Check on accuracy of numerical integration, estimate error
  //--------------------------------------------------------------------------

  DoubleStepFlag = true;
  SIMLIB_ERRNO = 0;
  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    double eerr; // estimated error
    double terr; // greatest allowed error

    eerr = fabs((         - A1[i]
                  +   4.0 * A3[i]
                  +  17.0 * A4[i]
                  -  23.0 * A5[i]
                  +   4.0 * A7[i]
                  - dthlf * (*ip)->GetDiff()
                ) / 90.0);  // error estimation
    terr = SIMLIB_AbsoluteError + fabs(SIMLIB_RelativeError*si[i]);

    if(eerr < err_coef*terr) // allowed tolerantion is fulfiled with provision
      continue;

    if(eerr > terr) {        // allowed tolerantion is overfulfiled
      if(SIMLIB_StepSize>SIMLIB_MinStep) {    // reducing step is possible
        SIMLIB_OptStep = 0.5*SIMLIB_StepSize; // halve optimal step
        if(SIMLIB_OptStep < SIMLIB_MinStep) { // limit of optimal step
          SIMLIB_OptStep = SIMLIB_MinStep;
        }
        SIMLIB_StepSize = SIMLIB_OptStep;
        IsEndStepEvent = false;
        goto begin_step; // compute again with smaller step
      }
      // reducing step is unpossible
      SIMLIB_ERRNO++;          // requested accuracy cannot be achieved
      _Print("\n Integrator[%lu] ",(unsigned long)i);
      if(SIMLIB_ConditionFlag) // event was in half step, step reducing was
        break;                 // unpossible and accuracy cannot be achieved
    }
    DoubleStepFlag = false;    // disable increasing SIMLIB_OptStep,
  }                            // accuracy is sufficient, but not well
  if(SIMLIB_ERRNO) {
    SIMLIB_warning(AccuracyError);
  }

  //--------------------------------------------------------------------------
  //  Computation is continuing, compute y(t2)
  //--------------------------------------------------------------------------

  if(wasContractStepFlag) {
    // step reducing has been requested in half step and it is unpossible
    RestoreState(dthlf, di, si, xi);// restore half step state
  } else { // go to half step and complete the computation

    GoToState(di, si, xi);

    for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
      (*ip)->SetState(si[i] - A6[i] + A7[i] + A7[i]);
    }

    SIMLIB_StepStartTime += dthlf;
    SIMLIB_DeltaTime = double(Time) - SIMLIB_StepStartTime;

    SIMLIB_Dynamic();  // evaluate new state of model                (8)

    for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
      // new state
      (*ip)->SetState(si[i] + (A5[i] + 4.0*A7[i] + dthlf*(*ip)->GetDiff()) / 6.0);
    }

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

} // RKE::Integrate


}
// end

