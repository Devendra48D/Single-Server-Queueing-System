/////////////////////////////////////////////////////////////////////////////
//! \file ni_abm4.h  Adams-Bashforth-Moulton 4th order
//
// Copyright (c) 1996-1997 David Leska
// Copyright (c) 1998-2004 Petr Peringer
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  numerical integration: Adams-Bashforth-Moulton's
//                         predictor-corrector 4th order
//


#include "simlib.h"

namespace simlib3 {

// order of the ABM4 method
const int abm_ord=4;


////////////////////////////////////////////////////////////////////////////
//  class representing the integration method
//
class ABM4 : public MultiStepMethod {
private:
  int ABM_Count;  // # of start steps (done by starting method)
  Memory Z[abm_ord];  // auxiliary memories
  Memory PRED;  // auxiliary memories -- value of predictor
public:
  ABM4(const char* name, const char* slave_name):
    MultiStepMethod(name, slave_name),
    ABM_Count(0)
  { /*NOTHING*/ }
  virtual ~ABM4()  // destructor
  { /*NOTHING*/ }
  virtual void Integrate(void);  // integration method
  virtual bool PrepareStep(void);  // prepare object for integration step
}; // class ABM4

} // namespace

// end of ni_abm4.h

