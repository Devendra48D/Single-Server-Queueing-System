/////////////////////////////////////////////////////////////////////////////
//! \file ni_rkf8.h     Runge-Kutta-Fehlberg 8th order
//
// Copyright (c) 1996-1997 David Leska
// Copyright (c) 1998-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  numerical integration: Runge-Kutta-Fehlberg's method 8th order
//


#include "simlib.h"

namespace simlib3 {

////////////////////////////////////////////////////////////////////////////
//  class representing the integration method
//
class RKF8 : public SingleStepMethod {
private:
  Memory A1, A2, A3,  A4,  A5,  A6, A7;  // auxiliary memories
  Memory A8, A9, A10, A11, A12, A13;
public:
  RKF8(const char* name) :  // registrate method and name it
    SingleStepMethod(name)
  { /*NOTHING*/ }
  virtual ~RKF8()  // destructor
  { /*NOTHING*/ }
  virtual void Integrate(void);  // integration method
}; // class RKF8

}

// end of ni_rkf8.h

