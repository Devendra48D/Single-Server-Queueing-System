/////////////////////////////////////////////////////////////////////////////
//! \file ni_rke.h    Runge-Kutta-England method (default)
//
// Copyright (c) 1996-1997 David Leska
// Copyright (c) 1998-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  numerical integration: Runge-Kutta-England's method
//  this is the default method (TODO: change to Dormand-Prince)
//


#include "simlib.h"

namespace simlib3 {

////////////////////////////////////////////////////////////////////////////
//  class representing the integration method
//
class RKE : public StatusMethod {
private:
  Memory A1, A2, A3, A4, A5, A6, A7;  // auxiliary memories
  Memory si, di;
  StatusMemory xi;
public:
  RKE(const char* name) :  // registrate method and name it
    StatusMethod(name)
  { /*NOTHING*/ }
  virtual ~RKE()  // destructor
  { /*NOTHING*/ }
  virtual void Integrate(void);  // integration method
}; // class RKE

}

// end of ni_rke.h

