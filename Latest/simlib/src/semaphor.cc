/////////////////////////////////////////////////////////////////////////////
//! \file semaphor.cc   Process synchronization - semaphore
//
// Copyright (c) 1994-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  class Semaphore implementation
//  TODO: correct too simple semantics, add capacity=1 default
//  it is like Store, but without statistics (different queue sort order?)
//

////////////////////////////////////////////////////////////////////////////
//  interface
//

#include "simlib.h"
#include "internal.h"

////////////////////////////////////////////////////////////////////////////
//  implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

////////////////////////////////////////////////////////////////////////////
//  constructors
//
Semaphore::Semaphore()
{
  Dprintf(("Semaphore::Semaphore()"));
  n = 1;
}

Semaphore::Semaphore(const char *name)
{
  Dprintf(("Semaphore::Semaphore(\"%s\")",name));
  SetName(name);
  n = 1;
}


////////////////////////////////////////////////////////////////////////////
//  destructor
//
Semaphore::~Semaphore()
{
  Dprintf(("Semaphore::~Semaphore()  // \"%s\", %d ",
           Name(), n));
}

////////////////////////////////////////////////////////////////////////////
//  initialization of state
//
void Semaphore::Clear() {
  Dprintf(("%s.Clear()", Name()));
  n = 1;
  Q.Clear(); // queue initialization ###!!!
}


////////////////////////////////////////////////////////////////////////////
//  Output
//
void Semaphore::Output() const {
  Print("Semaphore: %s [%d]\n", Name(), n);
}


////////////////////////////////////////////////////////////////////////////
//
//
void Semaphore::P()
{
  Dprintf(("Semaphore'%s'.P()", Name()));

  while(n == 0) {
    Q.Insert(Current);  // Current==this
    Passivate(Current);
    Q.Get(Current);
  }
  n--;
}

////////////////////////////////////////////////////////////////////////////
//
//
void Semaphore::V()
{
  Dprintf(("%s.V()", Name()));
  if(n>=1)
    SIMLIB_error(SemaphoreError);
  n++;
  Entity *p = Q.front(); // first entity in queue
  if(p) p->Activate();
}

}
// end

