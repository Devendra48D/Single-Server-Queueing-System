/////////////////////////////////////////////////////////////////////////////
//! \file ni_rkf5.h     Runge-Kutta-Fehlberg 5th order
//
// Copyright (c) 1996-1997 David Leska
// Copyright (c) 1998-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  numerical integration: Runge-Kutta-Fehlberg's method 5th order
//


#include "simlib.h"

namespace simlib3 {

////////////////////////////////////////////////////////////////////////////
//  class representing the integration method
//
class RKF5 : public SingleStepMethod {
private:
  Memory A1, A2, A3, A4, A5, A6;  // auxiliary memories
public:
  RKF5(const char* name) :  // registrate method and name it
    SingleStepMethod(name)
  { /*NOTHING*/ }
  virtual ~RKF5()  // destructor
  { /*NOTHING*/ }
  virtual void Integrate(void);  // integration method
}; // class RKF5

}

// end of ni_rkf5.h

