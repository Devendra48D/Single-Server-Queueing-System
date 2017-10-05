/////////////////////////////////////////////////////////////////////////////
//! \file  store.cc  Store implementation
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  implementation of store class (SOL-like)
//

////////////////////////////////////////////////////////////////////////////
//  interface
//

#include "simlib.h"
#include "internal.h"

#include <cstdio>


////////////////////////////////////////////////////////////////////////////
//  implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

#define _OWNQ 0x01

#define CHECKQUEUE(qptr) if (!qptr) SIMLIB_error(QueueRefError)

////////////////////////////////////////////////////////////////////////////
//  constructors
//
Store::Store() :
  _Qflag(_OWNQ),
  capacity(1L),
  used(0L),
  Q(new Queue("Q"))
{
  Dprintf(("Store::Store()"));
}

Store::Store(unsigned long _capacity) :
  _Qflag(_OWNQ),
  capacity(_capacity),
  used(0L),
  Q(new Queue("Q"))
{
  Dprintf(("Store::Store(%lu)",_capacity));
}

Store::Store(const char * name, unsigned long _capacity) :
  _Qflag(_OWNQ),
  capacity(_capacity),
  used(0L),
  Q(new Queue("Q"))
{
  Dprintf(("Store::Store(\"%s\",%lu)",name,_capacity));
  ::SetName(this,name);
}

Store::Store(unsigned long _capacity, Queue *queue) :
  _Qflag(0),
  capacity(_capacity),
  used(0L),
  Q(queue)
{
  CHECKQUEUE(queue);
  Dprintf(("Store::Store(%lu,%s)",_capacity,queue->Name()));
}

Store::Store(const char *name, unsigned long _capacity, Queue *queue) :
  _Qflag(0),
  capacity(_capacity),
  used(0L),
  Q(queue)
{
  CHECKQUEUE(queue);
  Dprintf(("Store::Store(\"%s\",%lu, Queue\"%s\")",
            name, _capacity, queue->Name()));
  ::SetName(this, name);
}


////////////////////////////////////////////////////////////////////////////
//  destructor
//
Store::~Store()
{
  Dprintf(("Store::~Store() // \"%s\" ",Name()));
  Clear();
  if (OwnQueue()) delete Q;
}

////////////////////////////////////////////////////////////////////////////
//  SetCapacity
//
void Store::SetCapacity(unsigned long newcapacity)
{
  if (capacity<newcapacity ||
      (QueueLen()==0 && used<=newcapacity)
     ) capacity = newcapacity;
  else SIMLIB_error(SetCapacityError);
}

////////////////////////////////////////////////////////////////////////////
//  SetQueue
//
void Store::SetQueue(Queue *queue)
{
  CHECKQUEUE(queue);
  if (OwnQueue())
  {
    if (QueueLen()>0) SIMLIB_warning(SetQueueError);
    delete Q;           // delete internal queue
    _Qflag &= ~_OWNQ;
  }
  Q = queue;
}

////////////////////////////////////////////////////////////////////////////
//  Enter -- allocate requested capacity
//
void Store::Enter(Entity *e, unsigned long rcap)
{
// TODO?: remove parameter e, use Current
  Dprintf(("%s.Enter(%s,%lu)",Name(),e->Name(),rcap));

  if (e != Current)
    SIMLIB_error(EntityRefError); // current process only

  if (rcap>capacity)  SIMLIB_error(EnterCapError);
  if (Free() < rcap)         // not enough space in store
  {
    QueueIn(e,rcap);         // isert into queue
    e->Passivate();          // wait to activation from Leave()
    // ### warning: should be activated only from Leave() -- add check ???
    return;                  // after activation is allocated!
  }
  used += rcap;                 // allocate capacity
  tstat(used);                  // update statistics
}

////////////////////////////////////////////////////////////////////////////
// Leave --- free requested capacity
//
void Store::Leave(unsigned long rcap)
{
  Dprintf(("%s.Leave(%lu)", Name(), rcap));
  if (used<rcap)  SIMLIB_error(LeaveManyError);
  used -= rcap ;           // free capacity
  tstat(used);  tstat.n--; // correction of statistic ???!!!
  if(Q->empty()) return;   // empty queue
  // satisfy entities waiting in queue (starting from begin)
  Queue::iterator pp = Q->begin();   // first item in queue
  while( pp != Q->end() && !Full() ) {
      Entity *p = ((Entity*)(*pp));
      ++pp; // step forward (next action invalidate iterator)
      if (p->_RequiredCapacity > Free()) continue; // skip
      p->Out();                      // remove from queue
      Dprintf(("%s.Enter(%s,%lu) from queue",
                Name(), p->Name(), p->_RequiredCapacity));
      used += p->_RequiredCapacity;       // allocate capacity
      tstat(used);                   // update statistic
      p->Activate();                 // reactivate now
  } // while
}

////////////////////////////////////////////////////////////////////////////
// QueueIn --- insert into priority queue
//
void Store::QueueIn(Entity *e, unsigned long c)
{
  Dprintf(("%s --> input queue of %s ",e->Name(),Name()));
  e->_RequiredCapacity = c;  // mark requested capacity
  Q->Insert(e);         // insert
}

////////////////////////////////////////////////////////////////////////////
//  Clear - store initialization
//
void Store::Clear()
{
  Dprintf(("%s.Clear()", Name()));
  used = 0;                     // empty
  // !!! CHANGE ###
  // initialize only own queue
  if (OwnQueue()) Q->Clear();   // clear input queue
  tstat.Clear();                // clear store statistic
}

////////////////////////////////////////////////////////////////////////////
//  owns queue?
//
bool Store::OwnQueue() const
{
  return (_Qflag & _OWNQ) != 0;
}

}

