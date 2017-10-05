/////////////////////////////////////////////////////////////////////////////
//! \file ni_fw.h       Fowler-Warten method
//
// Copyright (c) 1996-1997 David Leska
// Copyright (c) 1998-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  numerical integration: Fowler-Warten's method
//


#include "simlib.h"

namespace simlib3 {

////////////////////////////////////////////////////////////////////////////
//  class representing the integration method
//
class FW : public SingleStepMethod {
private:
  bool FW_First;  // is method called for first time?
  Memory Y1, ERR;  // auxiliary memories
protected:
  static const double prec; // near zero number
public:
  FW(const char* name) :  // registrate method and name it
    SingleStepMethod(name),
    FW_First(true)
  { /*NOTHING*/ }
  virtual ~FW()  // destructor
  { /*NOTHING*/ }
  virtual bool PrepareStep(void);  // prepare object for integration step
  virtual void Integrate(void);  // integration method
}; // class FW

}

// end of ni_fw.h

