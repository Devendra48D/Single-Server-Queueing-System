/////////////////////////////////////////////////////////////////////////////
//! \file object.cc     Root of SIMLIB/C++ class hierarchy
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
// this module contains implementation of base class SimObject
//

#include "simlib.h"
#include "internal.h"

////////////////////////////////////////////////////////////////////////////
namespace simlib3 {

SIMLIB_IMPLEMENTATION;
////////////////////////////////////////////////////////////////////////////

// static flag for IsAllocated()
static bool SimObject_allocated = false;

////////////////////////////////////////////////////////////////////////////
//! allocate memory for object
void *SimObject::operator new(size_t size) {
//  try {
  void *ptr = ::new char[size]; // global operator new
//  Dprintf(("SimObject::operator new(%u) = %p ", size, ptr));
//  }catch(...) { SIMLIB_error(MemoryError); }
//  if(!ptr) SIMLIB_error(MemoryError); // for old compilers
  SimObject_allocated = true; // update flag
  return ptr;
}

////////////////////////////////////////////////////////////////////////////
//! free memory
//
//TODO: this can create trouble if called from e.g. Behavior()
//
void SimObject::operator delete(void *ptr) {
//  Dprintf(("SimObject::operator delete(%p) ", ptr));
  SimObject *sp = static_cast<SimObject*>(ptr);
  if (sp->isAllocated()) {
      sp->_flags = 0; // clear all flags
      ::operator delete[](ptr);  // free memory
  }
}


////////////////////////////////////////////////////////////////////////////
//! constructor
//
SimObject::SimObject() :
  _name(0),
  _flags(0)
{
//  Dprintf(("SimObject::SimObject() this=%p ", this));
  if(SimObject_allocated) {
    SimObject_allocated = false;
    _flags |= (1<<_ALLOCATED_FLAG);
  }
}

////////////////////////////////////////////////////////////////////////////
//! virtual destructor
//
SimObject::~SimObject()
{
//  Dprintf(("SimObject::~SimObject() this=%p ", this));
}

////////////////////////////////////////////////////////////////////////////
//! set the name of object
//
void SimObject::SetName(const char *name)
{
  _name = name;
}

////////////////////////////////////////////////////////////////////////////
//! get the name of object
//
const char *SimObject::Name() const
{
  if(_name==0) return ""; // no name
  return _name;
}


////////////////////////////////////////////////////////////////////////////
//! print object's info
//
void SimObject::Output() const
{
  Print("SimObject: this=%p, name=%s\n", this, Name());  // default
}

} // namespace

