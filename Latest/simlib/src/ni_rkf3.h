/////////////////////////////////////////////////////////////////////////////
//! \file ni_rkf3.h     Runge-Kutta-Fehlberg 3rd order
//
// Copyright (c) 1996-1997 David Leska
// Copyright (c) 1998-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  numerical integration: Runge-Kutta-Fehlberg's method 3rd order
//


#include "simlib.h"

namespace simlib3 {

////////////////////////////////////////////////////////////////////////////
//  class representing the integration method
//
class RKF3 : public SingleStepMethod {
private:
  Memory A1, A2, A3;  // auxiliary memories
public:
  RKF3(const char* name) :  // registrate method and name it
    SingleStepMethod(name)
  { /*NOTHING*/ }
  virtual ~RKF3()  // destructor
  { /*NOTHING*/ }
  virtual void Integrate(void);  // integration method
}; // class RKF3

}

// end of ni_rkf3.h

