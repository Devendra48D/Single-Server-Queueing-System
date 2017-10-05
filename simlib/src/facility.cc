/////////////////////////////////////////////////////////////////////////////
//! \file facility.cc  Implementation of Facility
//
// Copyright (c) 1991-2006 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  class Facility implementation
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

// don't change!!!
#define _OWNQ1 0x01

#define CHECKQUEUE(qptr)    if (!qptr) SIMLIB_error(QueueRefError)
#define CHECKENTITY(fptr)   if (!fptr) SIMLIB_error(EntityRefError)

////////////////////////////////////////////////////////////////////////////
//  constructors
//
Facility::Facility()
{
    Dprintf(("Facility::Facility()"));
    _Qflag = 0;
    Q1 = new Queue("Q1");
    _Qflag |= _OWNQ1;
    Q2 = new Queue("Q2");
    in = NULL;
}

Facility::Facility(const char *name)
{
    Dprintf(("Facility::Facility(\"%s\")", name));
    SetName(name);
    _Qflag = 0;
    Q1 = new Queue("Q1");
    _Qflag |= _OWNQ1;
    Q2 = new Queue("Q2");
    in = NULL;
}

Facility::Facility(Queue * queue)
{
    Dprintf(("Facility::Facility(%s)", queue->Name()));
    _Qflag = 0;
    CHECKQUEUE(queue);
    Q1 = queue;
    Q2 = new Queue("Q2");
    in = NULL;
}

Facility::Facility(const char *name, Queue * queue)
{
    Dprintf(("Facility::Facility(\"%s\",%s)", name, queue->Name()));
    SetName(name);
    _Qflag = 0;
    CHECKQUEUE(queue);
    Q1 = queue;
    Q2 = new Queue("Q2");
    in = NULL;
}

////////////////////////////////////////////////////////////////////////////
//  destructor
//
Facility::~Facility()
{
    Dprintf(("Facility::~Facility()  // \"%s\" ", Name()));
    Clear();
    if (OwnQueue())
        delete Q1;              // delete input queue
    delete Q2;
}

////////////////////////////////////////////////////////////////////////////
//  SetQueue
//
void Facility::SetQueue(Queue * queue)
{
    CHECKQUEUE(queue);
    if (OwnQueue()) {
        if (QueueLen() > 0)
            SIMLIB_warning(SetQueueError);
        delete Q1;              // delete internal input queue
        _Qflag &= ~_OWNQ1;
    }
    Q1 = queue;
}
////////////////////////////////////////////////////////////////////////////
//  Seize -- seize facility by entity e
//
// possible waiting in queue
//
void Facility::Seize(Entity * e, ServicePriority_t sp)
{
//
// TODO: remove parameter e, use Current
//
    Dprintf(("%s.Seize(%s,%u)", Name(), e->Name(), (unsigned) sp));
    CHECKENTITY(e);
    if (e != Current)
        SIMLIB_error(EntityRefError);
    e->_SPrio = sp;
    if (!Busy()) {
        in = e;                 // seize by entity
        tstat(1);               // update statistics
        return;
    }
    if (sp > in->_SPrio) {      // special case: service interrupted
        Dprintf((" service interrupt "));
        if (in->Idle()) // currently serviced entity is not scheduled
            SIMLIB_error(FacInterruptError);
        // compute the remaining service time
        in->_RemainingTime = in->ActivationTime() - Time;
        QueueIn2(*in);          // insert interrupted entity into queue2
        in->Passivate();        // wait in queue2 =====================
        in = e;                 // seize by entity
        tstat(1);               // update statistics
    } else {                    // go into main queue
        QueueIn(e, sp);         // insert in priority queue
        e->Passivate();         // wait in queue, activated by Release()
        // =======================================================
        // continue after activation
        Dprintf(("%s.Seize(%s,%u) from Q1", Name(), e->Name(),
                 (unsigned) sp));
    }
}

////////////////////////////////////////////////////////////////////////////
//  Release -- release fafility by entity e
//
// release causes Seize if queues not empty
//
void Facility::Release(Entity * e)
{
//
// TODO: remove parameter e, use Current
//
    Dprintf(("%s.Release(%s)", Name(), e->Name()));
    CHECKENTITY(e);
    if (!in)
        SIMLIB_error(ReleaseNotSeized); // not seized
    if (e != in)
        SIMLIB_error(ReleaseError);     // seized by other entity
    in = NULL;                  // empty
    tstat(0);                   // record
    tstat.n--;                  // correction !!

    bool flag = false;          // correction: 5.12.91, bool:1998/08/10
    if (!(Q1->empty() || Q2->empty())) {
        flag =
            (static_cast<Entity *>(Q1->front())->_SPrio >
             static_cast<Entity *>(Q2->front())->_SPrio);
    }

    if (!flag && !Q2->empty())  // interrupt queue not empty
    {                           // seize from interrupt queue...
// TODO: this is old implementation -- error in statistics, problems
        Entity *ent;
        ent = static_cast<Entity *>(Q2->GetFirst());        // remove from queue
        Dprintf(("%s.Seize(%s,%u) from Q2",
                 Name(), ent->Name(), (unsigned) ent->_SPrio));
        in = ent;               // seize again
        tstat(1);
        tstat.n--;              // correction !!!
        ent->Activate(Time + ent->_RemainingTime);  // schedule end of service
        return;
    }
    if (!Q1->empty()) {         // input queue not empty -- seize from Q1
        Entity *ent;
        ent = Q1->front();      // points to first entity in queue
        ent->Out();             // remove from queue
        in = ent;               // seize by entity [should be here]
        tstat(1);               // update statistics
        ent->Activate();        // activation of entity behavior
        return;
    }
}

////////////////////////////////////////////////////////////////////////////
// QueueIn -- go into input queue
//
void Facility::QueueIn(Entity * e, ServicePriority_t sp)
{
//
// TODO: remove parameter e, use Current ???
//
    Dprintf((" %s --> Q1 of %s ", e->Name(), Name()));
    CHECKENTITY(e);
    e->_SPrio = sp;
#if  0                          // _INS_FROM_BEGIN_SLOWER
    Queue::iterator p = Q1->begin();
    Queue::iterator end = Q1->end();
    ServicePriority_t Sprio = e->_SPrio;
    for (; p != end && static_cast<Entity *>(*p)->_SPrio > Sprio;       // higher service priority first
         ++p);
    ServicePriority_t prio = e->Priority;
    for (; p != end && static_cast<Entity *>(*p)->_SPrio == Sprio && static_cast<Entity *>(*p)->Priority >= prio;       // higher priority first
         ++p);
#else
    Queue::iterator begin = Q1->begin();
    Queue::iterator p = Q1->end();
    ServicePriority_t Sprio = e->_SPrio;
    while (p != begin) {
        Queue::iterator q = p;
        --p;
        if (static_cast<Entity *>(*p)->_SPrio >= Sprio) {       // higher service priority first
            p = q;
            break;
        }
    }
    ServicePriority_t prio = e->Priority;
    while (p != begin) {
        Queue::iterator q = p;
        --p;
        if (static_cast<Entity *>(*p)->_SPrio > Sprio ||
            static_cast<Entity *>(*p)->Priority >= prio) { // higher priority first
            p = q;
            break;
        }
    }
#endif
    Q1->PredIns(e, p);
}

////////////////////////////////////////////////////////////////////////////
//  go into interrupt queue
//
void Facility::QueueIn2(Entity * e)
{
    Dprintf((" %s --> Q2 of %s", e->Name(), Name()));
    ServicePriority_t ps = e->_SPrio;
    Queue::iterator p = Q2->begin();
    for (; p != Q2->end()
            && static_cast<Entity *>(*p)->_SPrio > ps;    // higher service priority first
         ++p) { /*empty*/ }
    ServicePriority_t prio = e->Priority;
    for (; p != Q2->end()
            && static_cast<Entity *>(*p)->_SPrio == ps
            && static_cast<Entity *>(*p)->Priority >= prio;    // higher priority first
         ++p) { /*empty*/ }
    // next sorting -- _RestTime????###
    Q2->PredIns(e, p);
}

////////////////////////////////////////////////////////////////////////////
//  initialization
//
void Facility::Clear()
{
    Dprintf(("%s.Clear()", Name()));
    //////// check !!! ###
    // only own queues!!!
    //
    if (OwnQueue())
        Q1->Clear();
    Q2->Clear();
    tstat.Clear();
    in = NULL;                  // empty
}


////////////////////////////////////////////////////////////////////////////
//  have own queue?
//
bool Facility::OwnQueue() const
{
    return (_Qflag & _OWNQ1) != 0;
}

}
// end

