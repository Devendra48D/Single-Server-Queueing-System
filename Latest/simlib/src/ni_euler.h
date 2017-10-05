/////////////////////////////////////////////////////////////////////////////
//! \file ni_euler.h  Modified Euler method
//
// Copyright (c) 1996-1997 David Leska
// Copyright (c) 1998-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  numerical integration: Euler's method
//


#include "simlib.h"

namespace simlib3 {

////////////////////////////////////////////////////////////////////////////
//  class representing the integration method
//
class EULER : public StatusMethod {
private:
  Memory si, di, A;  // auxiliary memories
  StatusMemory xi;
public:
  EULER(const char* name) :  // registrate method and name it
    StatusMethod(name)
  { /*NOTHING*/ }
  virtual ~EULER()  // destructor
  { /*NOTHING*/ }
  virtual void Integrate(void);  // integration method
}; // class EULER


} // namespace

// end of ni_euler.h

