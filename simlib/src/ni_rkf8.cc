/////////////////////////////////////////////////////////////////////////////
// ni_rkf8.cc
//
// Copyright (c) 1996-1997 David Leska
// Copyright (c) 1998-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  numerical integration: Runge-Kutta-Fehlberg's method 8th order
//

////////////////////////////////////////////////////////////////////////////
//  interface
//
#include "simlib.h"
#include "internal.h"
#include "ni_rkf8.h"
#include <cmath>
#include <cstddef>


////////////////////////////////////////////////////////////////////////////
//  implementation
//
namespace simlib3 {

SIMLIB_IMPLEMENTATION;


////////////////////////////////////////////////////////////////////////////
//  Runge-Kutta-Fehlberg's method 8th order
//
/*   Formula:

     is too large, for information see next source code
     or documentation (if is any ;-))
*/

void RKF8::Integrate(void)
{
  const double safety = 0.9; // keeps the new step from growing too large
  const double max_ratio = 4.0;  // ditto
  const double pshrnk = 1.0/7.0; // coefficient for reducing step
  const double pgrow  = 1.0/8.0; // coefficient for increasing step
  register size_t i;   // auxiliary variables for loops
  Iterator ip, end_it; // to go through list of integrators
  double ratio;     // ratio for next stepsize computation
  double next_step; // recommended stepsize for next step
  size_t n;         // integrator with greatest error

  Dprintf((" RKF8 integration step ")); // print debugging info
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
    (*ip)->SetState((*ip)->GetOldState() + 0.25*A1[i]); // state (y) for next substep
  }

  ////////////////////////////////////////////////////////////// 1/4 of step

  _SetTime(Time,SIMLIB_StepStartTime + 0.25*SIMLIB_StepSize); // substep time
  SIMLIB_DeltaTime = double(Time) - SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model (y'=f(t,y))      (1)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A2[i]  = SIMLIB_StepSize*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() + (5.0*A1[i] + A2[i]) / 72.0);
  }

  ////////////////////////////////////////////////////////////// 1/12 of step

  _SetTime(Time,SIMLIB_StepStartTime + 1.0/12.0*SIMLIB_StepSize); // substep
  SIMLIB_DeltaTime = double(Time) - SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model                  (2)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A3[i]  = SIMLIB_StepSize*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() + (A1[i] + 3.0*A3[i]) / 32.0);
  }

  ////////////////////////////////////////////////////////////// 1/8 of step

  _SetTime(Time, SIMLIB_StepStartTime + 0.125*SIMLIB_StepSize);
  SIMLIB_DeltaTime = double(Time)-SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model                  (3)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A4[i]  = SIMLIB_StepSize*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() + (   106.0 * A1[i]
                         - 408.0 * A3[i]
                         + 352.0 * A4[i]
                       ) / 125.0);
  }

  ////////////////////////////////////////////////////////////// 2/5 of step

  _SetTime(Time, SIMLIB_StepStartTime + 0.4*SIMLIB_StepSize);
  SIMLIB_DeltaTime = double(Time)-SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model                  (4)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A5[i]  = SIMLIB_StepSize*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() +   1.0 /  48.0 * A1[i]
                     +   8.0 /  33.0 * A4[i]
                     + 125.0 / 528.0 * A5[i]);
  }

  ///////////////////////////////////////////////////////////// 1/2 of step

  _SetTime(Time, SIMLIB_StepStartTime + 0.5*SIMLIB_StepSize);
  SIMLIB_DeltaTime = double(Time)-SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model                  (5)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A6[i]  = SIMLIB_StepSize*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() -  1263.0 /  2401.0 * A1[i]
                     + 39936.0 / 26411.0 * A4[i]
                     - 64125.0 / 26411.0 * A5[i]
                     +  5520.0 /  2401.0 * A6[i]);
  }

  ///////////////////////////////////////////////////////////// 6/7 of step

  _SetTime(Time, SIMLIB_StepStartTime + 6.0/7.0*SIMLIB_StepSize);
  SIMLIB_DeltaTime = double(Time)-SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model                  (6)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A7[i]  = SIMLIB_StepSize*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() +   37.0 /  392.0 * A1[i]
                     + 1625.0 / 9408.0 * A5[i]
                     -    2.0 /   15.0 * A6[i]
                     +   61.0 / 6720.0 * A7[i]);
  }

  ///////////////////////////////////////////////////////////// 1/7 of step

  _SetTime(Time, SIMLIB_StepStartTime + 1.0/7.0*SIMLIB_StepSize);
  SIMLIB_DeltaTime = double(Time)-SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model                  (7)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A8[i]  = SIMLIB_StepSize*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() + 17176.0 /  25515.0 * A1[i]
                     - 47104.0 /  25515.0 * A4[i]
                     +  1325.0 /    504.0 * A5[i]
                     - 41792.0 /  25515.0 * A6[i]
                     + 20237.0 / 145800.0 * A7[i]
                     +  4312.0 /   6075.0 * A8[i]);
  }

  ///////////////////////////////////////////////////////////// 2/3 of step

  _SetTime(Time, SIMLIB_StepStartTime + 2.0/3.0*SIMLIB_StepSize);
  SIMLIB_DeltaTime = double(Time)-SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model                  (8)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A9[i]  = SIMLIB_StepSize*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() -  23834.0 /  180075.0 * A1[i]
                     -  77824.0 / 1980825.0 * A4[i]
                     - 636635.0 /  633864.0 * A5[i]
                     + 254048.0 /  300125.0 * A6[i]
                     -    183.0 /    7000.0 * A7[i]
                     +      8.0 /      11.0 * A8[i]
                     -    324.0 /    3773.0 * A9[i]);
  }

  ///////////////////////////////////////////////////////////// 2/7 of step

  _SetTime(Time, SIMLIB_StepStartTime + 2.0/7.0*SIMLIB_StepSize);
  SIMLIB_DeltaTime = double(Time)-SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model                  (9)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A10[i]  = SIMLIB_StepSize*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() +  12733.0 /   7600.0 * A1[i]
                     -  20032.0 /   5225.0 * A4[i]
                     + 456485.0 /  80256.0 * A5[i]
                     -  42599.0 /   7125.0 * A6[i]
                     + 339227.0 / 912000.0 * A7[i]
                     -   1029.0 /   4180.0 * A8[i]
                     +   1701.0 /   1408.0 * A9[i]
                     +   5145.0 /   2432.0 * A10[i]);
  }

  ///////////////////////////////////////////////////////////// 1/1 of step

  _SetTime(Time, SIMLIB_StepStartTime + SIMLIB_StepSize);
  SIMLIB_DeltaTime = double(Time)-SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model                  (10)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A11[i]  = SIMLIB_StepSize*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() -   27061.0 /  204120.0 * A1[i]
                     +   40448.0 /  280665.0 * A4[i]
                     - 1353775.0 / 1197504.0 * A5[i]
                     +   17662.0 /   25515.0 * A6[i]
                     -   71687.0 / 1166400.0 * A7[i]
                     +      98.0 /     225.0 * A8[i]
                     +       1.0 /      16.0 * A9[i]
                     +    3773.0 /   11664.0 * A10[i]);
  }

  ///////////////////////////////////////////////////////////// 1/3 of step

  _SetTime(Time, SIMLIB_StepStartTime + 1.0/3.0*SIMLIB_StepSize);
  SIMLIB_DeltaTime = double(Time)-SIMLIB_StepStartTime;

  SIMLIB_Dynamic();  // evaluate new state of model                  (11)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A12[i]  = SIMLIB_StepSize*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState() +   11203.0 /    8680.0 * A1[i]
                     -   38144.0 /   11935.0 * A4[i]
                     + 2354425.0 /  458304.0 * A5[i]
                     -   84046.0 /   16275.0 * A6[i]
                     +  673309.0 / 1636800.0 * A7[i]
                     +    4704.0 /    8525.0 * A8[i]
                     +    9477.0 /   10912.0 * A9[i]
                     -    1029.0 /     992.0 * A10[i]
                     +     729.0 /     341.0 * A12[i]);
  }

  ////////////////////////////////////////////////////////////// 1/1 of step

  _SetTime(Time, SIMLIB_StepStartTime+SIMLIB_StepSize);
  SIMLIB_DeltaTime = SIMLIB_StepSize;

  SIMLIB_Dynamic();  // evaluate new state of model                  (9)

  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    A13[i]  = SIMLIB_StepSize*(*ip)->GetDiff();
    (*ip)->SetState((*ip)->GetOldState()+   31.0/720.0   * (A1[i]+A13[i])
                    +   16.0/75.0    *  A6[i]
                    +16807.0/79200.0 * (A7[i]+A8[i])
                    +  243.0/1760.0  * (A9[i]+A12[i]));
  }

  //--------------------------------------------------------------------------
  //  Check on accuracy of numerical integration, estimate error
  //--------------------------------------------------------------------------

  SIMLIB_ERRNO = 0; // OK
  ratio = 256.0;    // 2^8 - ratio for stepsize computation - initial value
  n=0;              // integrator with greatest error
  for(ip=FirstIntegrator(),i=0; ip!=end_it; ip++,i++) {
    double eerr; // estimated error
    double terr; // greatest allowed error
    eerr = fabs(    -1.0 /    480.0 * A1[i]
                -   16.0 /    375.0 * A6[i]
                - 2401.0 / 528000.0 * A7[i]
                + 2401.0 / 132000.0 * A8[i]
                +  243.0 /  14080.0 * A9[i]
                - 2401.0 /  19200.0 * A10[i]
                -   19.0 /    450.0 * A11[i]
                +  243.0 /   1760.0 * A12[i]
                +   31.0 /    720.0 * A13[i]
               );
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

} // RKF8::Integrate

}
// end of ni_rkf8.cc

