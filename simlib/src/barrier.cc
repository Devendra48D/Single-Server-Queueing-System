/////////////////////////////////////////////////////////////////////////////
//! \file barrier.cc  Barrier synchronization
//
// Copyright (c) 1991-2007 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  class Barrier implementation
//
// TODO add method to set activation order - FIFO/LIFO

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
/// constructor
//
Barrier::Barrier(unsigned height):
    waiting(0), n(0), maxn(height)
{
    Dprintf(("Barrier::Barrier()"));
    Init();
}

////////////////////////////////////////////////////////////////////////////
/// constructor with name parameter
//
Barrier::Barrier(const char *name, unsigned height):
    waiting(0), n(0), maxn(height)
{
    Dprintf(("Barrier::Barrier(\"%s\")", name));
    SetName(name);
    Init();
}


////////////////////////////////////////////////////////////////////////////
/// destructor
//
Barrier::~Barrier() {
    Dprintf(("Barrier::~Barrier()  // \"%s\" ", Name()));
    delete[] waiting;
}

////////////////////////////////////////////////////////////////////////////
/// enter the barrier
//
void Barrier::Enter(Entity * e) //TODO without parameter: use Current?
{
    Dprintf(("Barrier\"%s\".Enter(%s)", Name(), e->Name()));
    if (n < maxn - 1) {         // all waiting processes
        waiting[n++] = e;
        e->Passivate();
    } else {                    // last process which breaks barrier
        Break();
        Current->Activate();    // re-activation of last process - FIFO order
    }
}

////////////////////////////////////////////////////////////////////////////
/// Wait or break the barrier
/// returns: true for last process breaking the barrier, else false
//
bool Barrier::Wait()
{
    Dprintf(("Barrier\"%s\".Wait() for %s", Name(), Current->Name()));
    if (n < maxn - 1) {         // all waiting processes
        waiting[n++] = Current;
        Current->Passivate();
        return false;
    } else {                    // last process which breaks barrier
        Break();
        Current->Activate(Time);    // re-activation of last process - FIFO order
        return true;
    }
}

////////////////////////////////////////////////////////////////////////////
/// Activate all processes waiting on the barrier
/// returns: number of activated entities
/// Warning: can be called only by process not already waiting on barrier
//
int Barrier::Break() {
    int ret = n;
    if (n == 0)         // nothing to do
        return ret;
    for (unsigned i = 0; i < n; i++)    // FIFO order of activation
    {
        waiting[i]->Activate();
        waiting[i] = 0;
    }
    n = 0;
    return ret;
}

////////////////////////////////////////////////////////////////////////////
/// Barrier initializtion
//
void Barrier::Init() {
    Dprintf(("%s.Init()", Name()));
    if (maxn < 1)
        Error("Barrier size less than 1");
    waiting = new Entity *[maxn];       // allocation   maxn-1 ???
//  if(!waiting) SIMLIB_error(MemoryError);  // if without exceptions
    Clear();
}

////////////////////////////////////////////////////////////////////////////
/// Change the height of barrier
//
void Barrier::ChangeHeight(unsigned new_height) {
    Dprintf(("%s.ChangeHeight(%u)", Name(), new_height));
    if (new_height < n || new_height < 1)
        Error("Barrier height can not be changed");
    Entity **new_w = new Entity *[new_height];  // allocation
//  if(!new_w) SIMLIB_error(MemoryError);  // if compiled without exceptions
    for (unsigned i = 0; i < n; i++)    // copy
        new_w[i] = waiting[i];
    delete [] waiting;    // free old array
    waiting = new_w;
    maxn = new_height;  // new size
    for (unsigned i = n; i < maxn; i++) // zero all non-used items
        new_w[i] = 0;
}

////////////////////////////////////////////////////////////////////////////
/// Barrier re-initializtion
/// TODO: what to do with currently waiting passive processes?
//
void Barrier::Clear() {
    Dprintf(("%s.Clear()", Name()));
    for (unsigned i = 0; i < maxn; i++)
        waiting[i] = 0;
    n = 0;
}

////////////////////////////////////////////////////////////////////////////
/// Print barrier status
//
void Barrier::Output() const {
    Print("Barrier: %s\n", Name());
    for (unsigned i = 0; i < maxn; i++)
        if (waiting[i])
            Print("%3d: [%p] %s\n", i, waiting[i], waiting[i]->Name());
        else
            Print("%3d: empty\n", i);
    Print("\n");
}

} // namespace

