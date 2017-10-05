/////////////////////////////////////////////////////////////////////////////
//! \file calendar.cc  Calendar queue implementation
//
// Copyright (c) 1991-2008 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

/// Implementation of class CalendarList
/// <br> interface is static - using global functions in SQS namespace
///
/// <br> uses double-linked list and dynamic calendar queue [brown1988]
//
//TODO: reference-counting entities

////////////////////////////////////////////////////////////////////////////
// interface
//

#include "simlib.h"
#include "internal.h"
#include <cmath>
#include <cstring>

//#define MEASURE // comment this to switch off
#ifdef MEASURE

// this is for measuring performance of operations only
namespace {
#include "rdtsc.h"    // RDTSC instruction on i586+

static unsigned long long T_MEASURE;

inline void START_T() {
    T_MEASURE = rdtsc();
}

inline double STOP_T() {
    T_MEASURE = rdtsc() - T_MEASURE;
    // here can be unit conversion code
    // iuse  CPU clocks for now
    return (double)T_MEASURE;
}

enum {
    OP_RESIZE=1,
    OP_ESTIMATE=2,
    OP_SWITCH2CQ=4,
    OP_SWITCH2LIST=8,
};
int OP_MEASURE = 0;

} // local namespace
#endif

////////////////////////////////////////////////////////////////////////////
// implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

/// common interface for all calendar (PES) implementations
class Calendar { // abstract base class
  public:
    bool     Empty() const { return _size == 0; }
    unsigned Size()  const { return _size; }
    /// enqueue
    virtual void     ScheduleAt(Entity *e, double t) = 0;
    /// dequeue first
    virtual Entity * GetFirst() = 0;
    /// dequeue
    virtual Entity * Get(Entity *e) = 0;
    /// remove all scheduled entities
    virtual void clear(bool destroy_entities=false) = 0;
#ifndef NDEBUG
    /// for debugging only
    virtual void debug_print() = 0;     // print the calendar contents
#endif
    /// time of activation of first item
    double MinTime() const { return mintime; }
  protected:
    /// set cache for faster access
    void SetMinTime(double t) { mintime=t; }

  ///////////////////////////////////////////////////////////////////////////
  // data
  protected:
    unsigned _size;     //!< number of scheduled items
  private:
    double mintime;     //!< activation time of first event
  ///////////////////////////////////////////////////////////////////////////
  // singleton:
  public:
    static Calendar * instance();       //!< create/get single instance (singleton)
    /// check if the instance exists
    static bool instance_exists() {
        return _instance != 0;
    }
  protected:
    Calendar(): _size(0), mintime(SIMLIB_MAXTIME) {}
    virtual ~Calendar() {} //!< clear is called in derived class dtr
    static void delete_instance();      //!< destroy single instance
  private:
    static Calendar * _instance;        //!< pointer to single instance
  ///////////////////////////////////////////////////////////////////////////
  friend void SetCalendar(const char *name); // sets _instance
};

/////////////////////////////////////////////////////////////////////////////
/// calendar item - PRIVATE for any implementation
/// <br> we use double-linked circular list
struct EventNoticeLinkBase { // base class with list pointers only
                   // used as list head in CalendarList and as part of list items
    EventNoticeLinkBase * pred;         //!< previous object in list
    EventNoticeLinkBase * succ;         //!< next object in list

    EventNoticeLinkBase() : pred(this), succ(this) {}

    /// remove from calendar list
    void remove() {
        pred->succ = succ;
        succ->pred = pred;
        pred = succ = this;     // NOT linked
    }

    /// insert at position p
    void insert(EventNoticeLinkBase * p) {
        // todo: move this test (it is not needed always)
        if(pred != this)
            remove();           // is in calendar, remove first
        // --- insert before item s
        succ = p;
        pred = p->pred;
        pred->succ = this;
        p->pred = this;
    }
};

/// activation record
struct EventNotice : public EventNoticeLinkBase {
    /// which entity is scheduled
    Entity * entity;
    /// activation time of entity
    double time;
    /// priority at the time of scheduling
    Entity::Priority_t priority;

    EventNotice(Entity *p, double t) :
        //inherited: pred(this), succ(this), // == NOT linked
        entity(p),              // which entity
        time(t),                // activation time
        priority(p->Priority)   // current scheduling priority
    {
        create_reverse_link();
    }

    /// create link from entity to activation record
    void create_reverse_link() {
        entity->_evn = this;     // reverse link to this activation record
    }
    /// delete link from entity to activation record
    void delete_reverse_link() {
        entity->_evn = 0;    // unlink reverse link
    }

    ~EventNotice() {
        if(pred != this) {
            remove();           // remove from calendar
            delete_reverse_link();
        }
    }

    /// set new values to existing (unlinked) record
    void Set(Entity *e, double t) {
        pred=succ=this;
        entity = e;             // which entity
        time = t;               // activation time
        priority = e->Priority; // current scheduling priority
        create_reverse_link();
    }

    static EventNotice *Create(Entity *p, double t);
    static void Destroy(EventNotice *en);

  private:
    EventNotice(const EventNotice&); // disable
    EventNotice &operator=(const EventNotice&);
};


// TODO: update interface to be compatible with std:: allocators
/// allocate activation records fast
// TODO: use std::list
class EventNoticeAllocator {
    static const unsigned MAXSIZELIMIT = 1000000;
    EventNoticeLinkBase *l; // single-linked list of freed items
    unsigned freed;
  public:
    EventNoticeAllocator(): l(0), freed(0) {}
    ~EventNoticeAllocator() {
        clear();  // delete freelist
    }

    /// free EventNotice, add to freelist for future allocation
    void free(EventNotice *en) {
        if(en->pred!=en) { // if scheduled
            en->remove();  // unlink from calendar list
            en->delete_reverse_link();
        }
        if(freed>MAXSIZELIMIT)  // limit the size of freelist
            delete en;
        else {
            // add to freelist
            en->succ=l;
            l=en;
            freed++;
        }
    }
    /// EventNotice allocation or reuse from freelist
    EventNotice *alloc(Entity *p, double t) {
        if(l==0)
            return new EventNotice(p, t);
        else {
            // get from freelist
            freed--;
            EventNotice *ptr = static_cast<EventNotice *>(l);
            l = l->succ;
            ptr->Set(p,t);
            return ptr;
        }
    }
    /// clear: delete all free-list items
    void clear() {
        while(l!=0) {
            EventNotice *p = static_cast<EventNotice*>(l);
            l=l->succ;
            delete p;
        }
    }
} allocator;  // global allocator TODO: improve -> singleton



////////////////////////////////////////////////////////////////////////////
/// class CalendarListImplementation --- sorted list
//    +-----------------------------------------------------+
//    |   +---+                          +---+      +---+   |
//    +-->|   |<--    ............    -->|   |<---->|   |<--+
//     +--+---+---------------------+    +---+      +---+
//     | CalendarListImplementation |    |EvN|      |EvN|
//     +----------------------------+    +---+      +---+
// double-linked circular list (should be faster than std::list)
// items sorted by: 1) time
//                  2) priority
//                  3) FIFO
//
// no checking
//
class CalendarListImplementation {
    EventNoticeLinkBase l;  //!< head of circular list
  public:
    class iterator { //!< bidirectional iterator
      EventNoticeLinkBase *p;
      public:
      iterator(EventNoticeLinkBase *pos) : p(pos) {}
      // prefix version only
      iterator &operator++() { p = p->succ; return *this; }
      iterator &operator--() { p = p->pred; return *this; }
      // implicit operator= and copy constructor are OK
      EventNotice * operator*() {
          // access to activation record
          return static_cast<EventNotice *>(p);
      }
      bool operator != (iterator q) { return p!=q.p; }
      bool operator == (iterator q) { return p==q.p; }
    }; // iterator
    iterator begin()    { return l.succ; }
    iterator end()      { return &l; }
    bool empty()        { return begin() == end(); }
    ///////////////////////////////////////////////////////////

    double first_time() { return (*begin())->time; }
    EventNotice *first() { return *begin(); }

private:
    /// search --- linear search for insert position
    iterator search(EventNotice *en) {
      if(empty())
          return end();
      double t = en->time;
      iterator evn = --end();               // search from back
      while(evn!=end() && (*evn)->time > t) {        // search time
        --evn;
      }
      Entity::Priority_t prio = en->priority;
      while(evn!=end() && (*evn)->time==t &&         // equal time...
            (*evn)->priority < prio) {               // ...search priority
        --evn;
      }
      ++evn;
      return evn;  // return insert position or end()
    }

public:
    /// enqueue operation
    void insert(Entity *e, double t) {
      EventNotice *evn = EventNotice::Create(e,t);
      iterator pos = search(evn);
      evn->insert(*pos); // insert before pos
    }

    /// special dequeue operation for rescheduling
    Entity *remove(Entity *e) {
      EventNotice::Destroy(e->GetEventNotice());   // disconnect, remove item
      return e;
    }

    /// dequeue operation
    Entity *remove_first() {
      Entity *e = first()->entity;
      EventNotice::Destroy(first());   // disconnect, remove item
      return e;
    }

    CalendarListImplementation() { }
    /// remove all items
    /// @param destroy  deallocates entities if true
    void clear(bool destroy) {
      while (!empty()) {
        Entity *e = remove_first();
        if (destroy && e->isAllocated()) delete e; // delete entity
      }
    }
    ~ CalendarListImplementation() {
        clear(true);
        allocator.clear(); // clear freelist
    }
#ifndef NDEBUG
    /// print of calendar contents - FOR DEBUGGING ONLY
    void debug_print();
#endif

    /// fast special operations for list swap  (Dangerous!):
    EventNotice *extract_first() {
      EventNotice *en = first();
      en->remove();   // disconnect only, no other changes
      return en;
    }
    /// fast special enqueue operation
    void insert_extracted(EventNotice *evn) {
      iterator pos = search(evn);
      evn->insert(*pos); // insert before pos
    }
};

////////////////////////////////////////////////////////////////////////////
/// class CalendarList --- list implementation of calendar
//
class CalendarList : public Calendar {
    CalendarListImplementation l;
  public:
    /// enqueue
    virtual void ScheduleAt(Entity *p, double t);

    /// dequeue
    virtual Entity *Get(Entity *p);              // remove process p from calendar
    /// dequeue first entity
    virtual Entity *GetFirst();
    /// remove all
    virtual void clear(bool destroy=false); // remove/destroy all items

    /// create calendar instance
    // TODO: we need this only if calendar use is allowed before Init()
    static CalendarList * create() {  // create instance
        Dprintf(("CalendarList::create()"));
        CalendarList *l = new CalendarList;
        SIMLIB_atexit(delete_instance);     // last SIMLIB module cleanup calls it
        //else             SIMLIB_error(DuplicateCalendar);
        return l;
    }
    virtual const char* Name() { return "CalendarList"; }

 private:
    CalendarList() {
        Dprintf(("CalendarList::CalendarList()"));
        SetMinTime( SIMLIB_MAXTIME ); // empty
    }
    ~CalendarList() {
        Dprintf(("CalendarList::~CalendarList()"));
        clear(true);
    }

public:
#ifndef NDEBUG
    virtual void debug_print(); // print of calendar contents - FOR DEBUGGING ONLY
#endif
};


////////////////////////////////////////////////////////////////////////////
// CalendarList implementation
//



////////////////////////////////////////////////////////////////////////////
/// creates new EventNotice,
inline EventNotice *EventNotice::Create(Entity *e, double t)
{
  register EventNotice *evn = e->GetEventNotice();

  if(evn) { // Entity is scheduled already --- REUSE EventNotice
    evn->remove(); // remove from calendar list, should stay connected
    evn->time = t; // change activation time
    evn->priority = e->Priority; // use current scheduling priority
//  Dprintf(("EventNotice::Create(entity=%p, time=%g, [priority=%d]): %p [RECYCLED]",
//                evn->entity, evn->time, evn->priority, this));
  }
  else {
    evn = allocator.alloc(e,t);
//  Dprintf(("EventNotice::Create(entity=%p, time=%g, [priority=%d]): %p [NEW]",
//                evn->entity, evn->time, evn->priority, this));
  }
  return evn;
}

////////////////////////////////////////////////////////////////////////////
/// delete EventNotice, if inserted in calendar remove it first
//
inline void EventNotice::Destroy(EventNotice *en)
{
  allocator.free(en);   // disconnect, remove item
}

////////////////////////////////////////////////////////////////////////////
///  schedule entity e at time t
void CalendarList::ScheduleAt(Entity *e, double t)
{
//  Dprintf(("CalendarList::ScheduleAt(%s,%g)", e->Name(), t));
  if(t<Time)
      SIMLIB_error(SchedulingBeforeTime);
  l.insert(e,t);
  ++_size;
  // update mintime:
  if(t < MinTime())
      SetMinTime(l.first_time());
}

////////////////////////////////////////////////////////////////////////////
/// delete first entity
Entity *CalendarList::GetFirst()
{
//  Dprintf(("CalendarList::GetFirst(): size=%u", Size()));

  if(Empty())
      SIMLIB_error(EmptyCalendar);

  Entity *e = l.remove_first();
  --_size;

  if(Empty())
      SetMinTime(SIMLIB_MAXTIME);
  else
      SetMinTime(l.first_time());
  return e;
}

////////////////////////////////////////////////////////////////////////////
/// remove entity e from calendar
Entity * CalendarList::Get(Entity * e)
{
//  Dprintf(("CalendarList::Get(e=%s): size=%u",e->Name(), Size()));

  if(Empty())
    SIMLIB_error(EmptyCalendar);  // internal --> TODO:remove
  if(e->Idle())
    SIMLIB_error(EntityIsNotScheduled);

  l.remove(e);
  --_size;

  if(Empty())
    SetMinTime(SIMLIB_MAXTIME);
  else
    SetMinTime(l.first_time());
  return e;
}

////////////////////////////////////////////////////////////////////////////
/// remove all event notices, and optionally destroy them
//
//TODO analyze possible problems
//
void CalendarList::clear(bool destroy)
{
  Dprintf(("CalendarList::clear(%s)", destroy?"true":"false"));
  l.clear(destroy);
  _size = 0;
  SetMinTime(SIMLIB_MAXTIME);
}

























/////////////////////////////////////////////////////////////////////////////
// CalendarQueue tunable parameters:

// when to switch from list to calendar queue
const unsigned LIST_MAX   = 512; // TODO:tune parameter: 64-1024
const unsigned MINBUCKETS = LIST_MAX>4?LIST_MAX:4; // should be power of 2 and >=4
const unsigned LIST_MIN   = LIST_MAX/2; // TODO:tune parameter

// average bucket list length
const double COEF_PAR     = 1.5;  // TODO:tune parameter: cca 1.5

// bucket width = MUL_PAR * average delta t
const double MUL_PAR      = 1.0; // TODO:tune parameter: 1.0--5.0

////////////////////////////////////////////////////////////////////////////
/// CQ implementation of calendar
class CalendarQueue : public Calendar {
    typedef CalendarListImplementation BucketList;
    BucketList *buckets;    // bucket array
    BucketList list;        // list for small number of items
    unsigned nbuckets;      // current number of buckets
    unsigned hi_bucket_mark;        // high bucket threshold for resize
    unsigned low_bucket_mark;       // low bucket threshold for resize
    unsigned nextbucket;    // next bucket to check for first item

    unsigned numop;         // number of operations performed from last tuning
    double bucket_width;    // parameter: width of each bucket
    double buckettop;       // top time of current bucket
    double last_dequeue_time; // for deleta computation
    double sumdelta;        // sum for bucket_width estimation
    unsigned ndelta;        // count

  private:
    bool list_impl() { return buckets==NULL; }

    /// Convert time to bucket number
    inline int time2bucket (double t) {
      return static_cast<int>(fmod(t/bucket_width, static_cast<double>(nbuckets)));
    }

    /// Compute bucket top limit
    // the *1.5 is good for bucket number >= 3
    // XXXX++++++--XXXX          X = time can be in bucket 1
    // 1111222233331111
    inline double time2bucket_top(double t) {
        return t + 1.5*bucket_width;
    }

    double estimate_bucket_width();
    void switchtocq();
    void switchtolist();

  public:
    /// enqueue
    virtual void ScheduleAt(Entity *p, double t);

    /// dequeue
    virtual Entity *Get(Entity *p);              // remove process p from calendar
    /// dequeue first
    virtual Entity *GetFirst();
    /// remove all
    virtual void clear(bool destroy=false); // remove/destroy all items

    /// create calendar instance
    // TODO: we need this only if calendar use is allowed before Init()
    static CalendarQueue * create() {  // create instance
        Dprintf(("CalendarQueue::create()"));
        CalendarQueue *l = new CalendarQueue;
        SIMLIB_atexit(delete_instance);     // last SIMLIB module cleanup calls it
        //TODO:SIMLIB_error(DuplicateCalendar);
        return l;
    }

    virtual const char* Name() { return "CalendarQueue"; }
 private:
    CalendarQueue();
    ~CalendarQueue();

    void Resize(int grow=0);                // grow/shrink operation
    void SearchMinTime(double starttime);   // search for new minimum

public:
#ifndef NDEBUG
    virtual void debug_print(); // print of calendar contents - FOR DEBUGGING ONLY
    void visualize(const char *msg);
#endif
}; // CalendarQueue


// TODO: tune this parameter
#define MAX_OP (Size()/2)

/////////////////////////////////////////////////////////////////////////////
/// Initialize calendar queue
CalendarQueue::CalendarQueue():
    buckets(0), // ptr to bucketarray
    nbuckets(0),        // number of buckets

    hi_bucket_mark(0),          // high bucket threshold
    low_bucket_mark(0),         // low bucket threshold

    nextbucket(0),      // next bucket to check
    numop(0),      // number of operations from last tuning
    bucket_width(0.0),  // width of each bucket
    buckettop(0.0),     // high time limit of current bucket
    last_dequeue_time(-1.0),
    sumdelta(0.0), ndelta(0) // statistics
{
    Dprintf(("CalendarQueue::CalendarQueue()"));
    SetMinTime( SIMLIB_MAXTIME ); // empty
    // init: Allocate initial buckets
    // Resize();
}

/// schedule
void CalendarQueue::ScheduleAt(Entity *e, double t)
{
    Dprintf(("CalendarQueue::ScheduleAt(%s,%g)", e->Name(), t));
    if(t<Time)
        SIMLIB_error(SchedulingBeforeTime);

    // if overgrown
    if(_size>LIST_MAX && list_impl())
        switchtocq();

    if(list_impl()) {
        // insert at right position
        list.insert(e,t);
    }
    else {
        // if too many items, resize bucket array
        if(_size + 1 > hi_bucket_mark)
            Resize(+1);

        if(++numop > MAX_OP) // tune each MAX_OP edit operations
            Resize();

        // select bucket
        BucketList &bp = buckets[time2bucket(t)];
        // insert at right position
        bp.insert(e,t);
    }
    ++_size;
    // update mintime
    if (MinTime() > t) {
        SetMinTime(t);
    }
}


////////////////////////////////////////////////////////////////////////////
///  dequeue
Entity * CalendarQueue::GetFirst()
{
//  Dprintf(("CalendarQueue::GetFirst()"));
  if(Empty())
      SIMLIB_error(EmptyCalendar);

  if(_size<LIST_MIN && !list_impl())
      switchtolist();

  if(list_impl()) {
      Entity * e = list.remove_first();
      // update size
      --_size;
      if(Empty())
          SetMinTime(SIMLIB_MAXTIME);
      else
          SetMinTime(list.first_time());
      return e;
  }

  // else

  // get statistics for tuning
  double min_time = MinTime();
  if(last_dequeue_time >= 0.0) {
      double diff = min_time - last_dequeue_time;
      if(diff>0.0) {
          sumdelta += diff;
          ndelta++;
      }
  }
  last_dequeue_time=min_time;

  // select bucket
  nextbucket = time2bucket(min_time); // TODO: optimization
  BucketList & bp = buckets[nextbucket];
  // get first item
  Entity * e = bp.remove_first();
  // update size
  --_size;
  if (_size < low_bucket_mark)
      Resize(-1);
  if(++numop > MAX_OP)
      Resize();
  // update mintime
  SearchMinTime(MinTime());
  return e;
}

////////////////////////////////////////////////////////////////////////////
/// remove entity e from calendar
/// <br>called only if rescheduling
Entity * CalendarQueue::Get(Entity * e)
{
//  Dprintf(("CalendarQueue::Get(e=%s)",e->Name()));
  if(Empty())
    SIMLIB_error(EmptyCalendar);  // internal use only --> TODO:remove
  if(e->Idle ())
    SIMLIB_error(EntityIsNotScheduled);

  if(_size<LIST_MIN && !list_impl())
      switchtolist();

  if(list_impl()) {
      list.remove(e);
      // update size
      --_size;
      // update mintime
      if(Empty())
          SetMinTime(SIMLIB_MAXTIME);
      else
          SetMinTime(list.first_time());
      return e;
  }

  // else

  // TODO: Get statistics!

  double t = e->ActivationTime();
  unsigned b = time2bucket(t);
  BucketList & bp = buckets[b];
  bp.remove(e);
  // update size
  --_size;
  if (_size < low_bucket_mark) // should be resized
      Resize(-1);
  if(++numop > MAX_OP)
      Resize();
  // update mintime
  if(t==MinTime()) // maybe first item removed - update mintime
      SearchMinTime(t);
  return e;
}

/////////////////////////////////////////////////////////////////////////////
/// compute new mintime
// TODO: compute dequeue price here
//
void CalendarQueue::SearchMinTime (double starttime)
{
  if(Empty()) {
      SetMinTime(SIMLIB_MAXTIME);
      return;
  }

  // search for minimum from starttime
  double tmpmin=SIMLIB_MAXTIME;
//  unsigned tmpnextbucket = 0;

  nextbucket = time2bucket(starttime);
  buckettop = time2bucket_top(starttime);

  // search buckets
  for (int n__ = nbuckets; n__ > 0; --n__) { // n__ is not used
      BucketList & bp = buckets[nextbucket];
      if (!bp.empty()) {
          // check first item in bucket
          double t = bp.first_time();
          if (t < buckettop) { // time is in this "year"
              // debug only -- TODO: leave out after tests
              if (t < starttime) SIMLIB_error("CalendarQueue implementation error in SearchMinTime");
              // first item is OK
              SetMinTime(t);
              return;
          }
          // search minimum time of all buckets for fallback
          if (t < tmpmin) {
              tmpmin = t;
//              tmpnextbucket = nextbucket;
          }
      }

      // go to next bucket (modulo nbuckets)
      // TODO: can be optimized if nbuckets is power of 2
      ++nextbucket;
      if (nextbucket == nbuckets)
          nextbucket = 0;
      buckettop += bucket_width;
    }

    // all buckets checked and none record found in current "year"
    // we use tmpmin
    // (happens when the queued times are sparse)

    SetMinTime(tmpmin);
//  nextbucket = tmpnextbucket;
//  buckettop = time2bucket_top (mintime);

}

/////////////////////////////////////////////////////////////////////////////
/// compute new bucket width --- EXPERIMENTAL
//
double CalendarQueue::estimate_bucket_width() {
  Dprintf(("Calendar bucket width estimation:"));
#ifdef MEASURE
  OP_MEASURE |= OP_ESTIMATE;
#endif
  if(ndelta>10 && sumdelta>0) { // do not use bad statistics
        double bu_width = MUL_PAR * sumdelta/ndelta;
        Dprintf(("  estm1: %g", bu_width));
        if(bu_width < 1e-12*MinTime())
            SIMLIB_error("CalendarQueue:e1 bucketwidth < 1e-12*Time -- total loss of precision");
        return bu_width;
  }

// =========================== EXPERIMENTAL CODE ========================
  // no past statistics - use future events
  const unsigned LIMIT = nbuckets>1000?1000:nbuckets; // limit # of intervals skipped (cca 1 "year")
  double last_time = MinTime();
  unsigned count=0; // number of non-zero intervals

for(int a__ = 0; a__<2; ++a__) {
  double tmpmin = SIMLIB_MAXTIME;
  unsigned next_bucket = time2bucket(last_time);
  double bucket_top = time2bucket_top(last_time);
  // search all buckets
  for (int n__ = nbuckets; n__ > 0; --n__) { // n__ is not used
      BucketList & bp = buckets[next_bucket];
      // check items in bucket
      for(BucketList::iterator i=bp.begin(); i != bp.end(); ++i) {
          double t=(*i)->time;
          if (t > bucket_top||t < last_time) { // time is NOT in this "year"
              // search minimum time of next years
              if (t < tmpmin)
                  tmpmin = t;
              break;
          }
          double diff = t - last_time;
          if(diff>0.0) {
             ++count;
          }
          last_time=t;
          if(count>LIMIT) break;
      }
      if(count>LIMIT) break;
      // go to next bucket (modulo nbuckets)
      // TODO: can be optimized if nbuckets is power of 2
      ++next_bucket;
      if (next_bucket == nbuckets)
          next_bucket = 0;
      bucket_top += bucket_width;
    }

    if(count>10) {
        double avg = (last_time-MinTime())/count;
        Dprintf(("  estm2: avg=%g", avg));
        double bu_width = MUL_PAR * avg;
        if(bu_width < 1e-12*MinTime())
            SIMLIB_error("CalendarQueue:e2 bucketwidth < 1e-12*Time -- total loss of precision");
        return bu_width;
    }

    if(tmpmin < SIMLIB_MAXTIME) {
        Dprintf(("  estm3: next tmpmin=%g", tmpmin));
        last_time = tmpmin;
        continue; // scan next year
    } else
      break;
} // for ==================================================

    // should (almost) never be reached
    // we have problem - bad calendar structure
    // TODO
    return 1.0;
}

/////////////////////////////////////////////////////////////////////////////
/// Resize bucket array
// - parameter:
//    grow > 0    grow
//    grow == 0   update bucket_width, init
//    grow < 0    shrink
// - does not change mintime !
//
// PROBLEM: TODO: too high-cost for 512-256-128 resize down !
void CalendarQueue::Resize(int grow)  // TODO: is it better to use target size?
{

    // visualize("before Resize");
#ifdef MEASURE
  OP_MEASURE |= OP_RESIZE;
#endif

    // first tune bucket_width
    bool bucket_width_changed = false;
    numop=0; // number of operations from last tuning/checking
    // test/change bucket_width
    double new_bucket_width = estimate_bucket_width();
    // TODO: 1.3/0.7 -- this needs improvement
    if(new_bucket_width>1.3*bucket_width || new_bucket_width<0.7*bucket_width) {
        bucket_width = new_bucket_width;
        bucket_width_changed = true;
    }

    // save old contents
    unsigned oldnbuckets = nbuckets;
    BucketList *oldbuckets = buckets;

    // change nbuckets
    if (grow>0)
        nbuckets = oldnbuckets * 2;
    if (grow<0)
        nbuckets = oldnbuckets / 2;

    // check low limit
    if(nbuckets < MINBUCKETS)
        nbuckets = MINBUCKETS;  // minimal number of buckets is 4

// TODO: switch to list if <LOWLIMIT
//       switch to calqueue if >HIGHLIMIT

    Dprintf(("Calendar resize: nbuckets=%d->%d", oldnbuckets, nbuckets));
    // nbuckets is a power of 2

    if(oldnbuckets == nbuckets && !bucket_width_changed) // no change
        return;

    // allocate new bucket array
    buckets = new BucketList[nbuckets];  // initialized by default constructors

    // TODO: search optimal PARAMETERS
    hi_bucket_mark = static_cast<unsigned>(nbuckets * COEF_PAR);  // cca 1.5 TODO: benchmarking
    low_bucket_mark = (nbuckets / 2) - 2;

    // if initialization only, we can stop here
    if (oldbuckets == NULL)
            return;

    // move old contents into new bucket array
    _size = 0;
    for (unsigned n = 0; n < oldnbuckets; ++n) {
        BucketList &oldbp = oldbuckets[n];
        while(!oldbp.empty()) {
            // extract EventNotice from old bucket
            EventNotice * en = oldbp.extract_first(); // no change of e,t,p
            // select new bucket
            BucketList &bp = buckets[time2bucket(en->time)];
            // insert EventNotice at right position
            bp.insert_extracted(en);
            ++_size;
        }
    }
    delete [] oldbuckets; // all are empty

} // Resize


/// switch to list implementation
void CalendarQueue::switchtolist()
{
    // _size, _mintime does not change
    // assert (buckets != NULL);
    // assert list.empty()
#ifdef MEASURE
  OP_MEASURE |= OP_SWITCH2LIST;
#endif

    // fill list from CQ
    for (unsigned n = 0; n < nbuckets; ++n) {
        BucketList &oldbp = buckets[n];
        while(!oldbp.empty()) {
            // extract EventNotice from old bucket
            EventNotice * en = oldbp.extract_first(); // no change of e,t,p
            // insert EventNotice at right position
            list.insert_extracted(en); // list is short
        }
    }
    delete [] buckets; // all are empty
    buckets = NULL;
    nbuckets = 0;

    // visualize("after switchtolist");
} // switch2list

/// switch to calendar queue implementation
void CalendarQueue::switchtocq()
{
    // first some initialization:
#ifdef MEASURE
  OP_MEASURE |= OP_SWITCH2CQ;
#endif

    // _size does not change
    // MinTime unchanged
    // assert (buckets == NULL);

    // nit CQ statistics:
    last_dequeue_time = -1.0;
    sumdelta = 0.0;
    ndelta = 0;

    numop=0; // number of operations from last tuning/checking

    // compute bucket_width
    CalendarListImplementation::iterator i=list.begin();
    double t0=(*i)->time; // mintime
    unsigned count = 0;
    for(unsigned n=0; i!=list.end() && n<100; ++i, ++n) {
       double t1=(*i)->time;
       if(t1==t0) continue;
       t0 = t1;
       ++count;
    }
    if(count > 5) {
        double avg = (t0-MinTime())/count;
        bucket_width = MUL_PAR * avg;
    } else
        bucket_width = 1.0;  // TODO: ?

    // assert
    if(bucket_width < 1e-12*MinTime())
        SIMLIB_warning("CalendarQueue:switchtocq bucketwidth<1e-12*Time = loss of precision");

    // search for first power of 2 greater than size
    for(nbuckets=4; nbuckets<_size; nbuckets<<=1) { /*empty*/ }
    // assert: nbuckets is power of 2

    // allocate new bucket array
    buckets = new BucketList[nbuckets];  // initialized by default constructors

    // TODO: search optimal PARAMETERS
    hi_bucket_mark = static_cast<unsigned>(nbuckets * COEF_PAR);  // cca 1.5 TODO: benchmarking
    low_bucket_mark = (nbuckets / 2) - 2;

    // move list contents into new bucket array
    while(!list.empty()) {
        // extract EventNotice from list
        EventNotice * en = list.extract_first(); // no change of e,t,p
        // select new bucket
        BucketList &bp = buckets[time2bucket(en->time)];
        // insert EventNotice at right position
        bp.insert_extracted(en);
    }

    // visualize("after switchtocq");
} // switch2cq


/////////////////////////////////////////////////////////////////////////////
/// clear calendar queue
//
// DIFFERENCE from simple list implementation:
//   - order of entity destruction is different
//
void CalendarQueue::clear(bool destroy)
{
    Dprintf(("CalendarQueue::clear(%s)",destroy?"true":"false"));
    // clear non-empty calendar
    if(!Empty()) {
        if(list_impl())
            list.clear(destroy);
        else
            // empty all buckets
            for(unsigned i=0; i<nbuckets; i++)
                buckets[i].clear(destroy);
        _size = 0;
    }
    // delete bucketarray
    delete [] buckets;
    buckets = NULL;
    nbuckets = 0;
    //
    // re-initialize
    // statistics:
    last_dequeue_time = -1.0;
    sumdelta = 0.0;
    ndelta = 0;

    numop = 0; // for tuning

    // list implementation is active now.
    SetMinTime(SIMLIB_MAXTIME);
}

/////////////////////////////////////////////////////////////////////////////
/// Destroy calendar queue
CalendarQueue::~CalendarQueue()
{
    Dprintf(("CalendarQueue::~CalendarQueue()"));
    clear(true);
    allocator.clear(); // clear freelist
}


/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


#ifndef NDEBUG
////////////////////////////////////////////////////////////////////////////
void CalendarListImplementation::debug_print() // print of list contents
{
  int n=0;
  for(iterator i = begin(); i!=end(); ++i) {
    Print("  [%03u]:", ++n );                   // order
    Print("\t %s", (*i)->entity->Name() );      // print entity ID
    Print("\t at=%g", (*i)->time );             // schedule time
    Print("\n");
  }
  if(n==0)
      Print("  <empty>\n");
}
////////////////////////////////////////////////////////////////////////////
void CalendarList::debug_print() // print of calendar contents
{
  Print("CalendarList:\n");
  if(CalendarList::instance_exists())
      l.debug_print();
  Print("\n");
}
////////////////////////////////////////////////////////////////////////////
void CalendarQueue::debug_print() // print of calendar queue contents
{
  Print("CalendarQueue:\n");
  if(CalendarQueue::instance_exists())
      for(unsigned i=0; i<nbuckets; i++) {
          Print(" bucket#%03u:\n", i);
          buckets[i].debug_print();
          Print("\n");
      }
  Print("\n");
}
////////////////////////////////////////////////////////////////////////////
/// CalendarQueue::visualize -- output suitable for Gnuplot
void CalendarQueue::visualize(const char *msg)
{
  Print("# CalendarQueue::visualize  %s\n",msg);
  if(list_impl())
      Print("# size=%u, mintime=%g (list)\n", Size(), MinTime());
  else
      Print("# size=%u, nbuckets=%d, mintime=%g, operations=%u, bucket_width=%g\n",
           Size(), nbuckets, MinTime(), numop, bucket_width);
  if(Empty()) return;
  for(unsigned b=0; b<nbuckets; b++) {
      BucketList &bl = buckets[b];
      unsigned cnt=0;
      Print("%d:", b );
      for(BucketList::iterator i = bl.begin(); i!=bl.end(); ++i) {
        //Print("%g %d\n", (*i)->time, b ); // schedule time
        Print(" %g", (*i)->time ); // schedule time
        cnt++;
      }
      Print("\n");
      //if(cnt>0) Print("#[%u].len = %u\n", b, cnt );
  }
  Print("\n");
}
////////////////////////////////////////////////////////////////////////////
#endif



////////////////////////////////////////////////////////////////////////////

/// static pointer to singleton instance
Calendar * Calendar::_instance = 0;

/// interface to singleton instance
inline Calendar * Calendar::instance() {
  if(_instance==0) {
#if 1 // choose default
      _instance = CalendarList::create(); // create default calendar
#else
      _instance = CalendarQueue::create(); // create default calendar
#endif
  }
  return _instance;
}

/// destroy single instance
void Calendar::delete_instance() {
    Dprintf(("Calendar::delete_instance()"));
    if(_instance) {
        delete _instance;           // remove all, free
        _instance = 0;
    }
}


#if 0
class Calendars {
    typedef Calendar * (*create_fun_ptr_t)();
    std::map<string, create_fun_ptr_t> record;
    Calendars() {}
    Register(create_fun_ptr_t f, const char *name) {
        string n(name);
        for(string::iterator i=n.begin(); i!=n.end(); ++i)
            *i = std::tolower(*i);
        record[n] = f;
    }
    Calendar *create(const char *name) {
        string n(name);
        for(string::iterator i=n.begin(); i!=n.end(); ++i)
            *i = std::tolower(*i);
        if(record.count(n)==0) // not present
            SIMLIB_error("Bad calendar type name");
        create_fun_ptr_t f = record[n];
        return f();
    }
};
#endif

/// choose calendar implementation
/// default is list
void SetCalendar(const char *name) {
  if( SIMLIB_Phase == INITIALIZATION ||
      SIMLIB_Phase == SIMULATION ) SIMLIB_error("SetCalendar() can't be used after Init()");

    if(Calendar::_instance) // already initialized
        Calendar::delete_instance();
    if(name==0 || std::strcmp(name,"")==0 || std::strcmp(name,"default")==0)
        Calendar::_instance = CalendarList::create();
    else if(std::strcmp(name,"list")==0)
        Calendar::_instance = CalendarList::create();
    else if(std::strcmp(name,"cq")==0)
        Calendar::_instance = CalendarQueue::create();
    else
        SIMLIB_error("SetCalendar: bad argument");
}


////////////////////////////////////////////////////////////////////////////
// public INTERFACE = exported functions...
//

#ifdef MEASURE
// global measurement results (in simlib3 namespace)
unsigned cal_cost_size;
double   cal_cost_time;
int      cal_cost_flag;
const char * cal_cost_op;
#endif

/// empty calendar predicate
bool SQS::Empty() {                       // used by Run() only
  return Calendar::instance()->Empty();
}

/// schedule entity e at given time t using scheduling priority from e
/// @param e entity
/// @param t time of activation
void SQS::ScheduleAt(Entity *e, double t) { // used by scheduling operations
  if(!e->Idle())
      SIMLIB_error("ScheduleAt call if already scheduled");
#ifdef MEASURE
  START_T();
#endif
  Calendar::instance()->ScheduleAt(e,t);
#ifdef MEASURE
  double ttt=STOP_T();
//  Print("enqueue %d %g %d\n", Calendar::instance()->size(), ttt, OP_MEASURE);
cal_cost_size = Calendar::instance()->Size();
cal_cost_time = ttt;
cal_cost_flag = OP_MEASURE;
cal_cost_op = "enqueue";
OP_MEASURE=0;
//  if(Calendar::instance()->size() < 300) Calendar::instance()->visualize("");
#endif
  _SetTime(NextTime, Calendar::instance()->MinTime());
}

/// remove selected entity activation record from calendar
void SQS::Get(Entity *e) {             // used by Run() only
#ifdef MEASURE
  START_T();
#endif
  Calendar::instance()->Get(e);
#ifdef MEASURE
  double ttt=STOP_T();
//  Print("dequeue2 %d %g %d\n", Calendar::instance()->size(), ttt, OP_MEASURE);
cal_cost_size = Calendar::instance()->Size();
cal_cost_time = ttt;
cal_cost_flag = OP_MEASURE;
cal_cost_op = "delete";
OP_MEASURE=0;
#endif
  _SetTime(NextTime, Calendar::instance()->MinTime());
}

/// remove entity with minimum activation time
/// @returns pointer to entity
Entity *SQS::GetFirst() {                  // used by Run()
#ifdef MEASURE
  START_T();
#endif
  Entity * ret = Calendar::instance()->GetFirst();
#ifdef MEASURE
  double ttt=STOP_T();
//  Print("dequeue %d %g %d\n", Calendar::instance()->size(), ttt, OP_MEASURE);
cal_cost_size = Calendar::instance()->Size();
cal_cost_time = ttt;
cal_cost_flag = OP_MEASURE;
cal_cost_op = "dequeue";
OP_MEASURE=0;
#endif
  _SetTime(NextTime, Calendar::instance()->MinTime());
  return ret;
}

/// remove all scheduled entities
void SQS::Clear() {                       // remove all
  Calendar::instance()->clear(true);
  _SetTime(NextTime, Calendar::instance()->MinTime());
}

int SQS::debug_print() {                 // for debugging only
  Calendar::instance()->debug_print();
  return Calendar::instance()->Size();
}

/// get activation time of entity - iff scheduled <br>
/// it is here, because Entity has no knowledge of calendar activation record structure
double Entity::ActivationTime() { // activation time
    //if(Idle()) SIMLIB_internal_error();  // passive entity
    if(Idle()) return SIMLIB_MAXTIME;  // passive entity
    return GetEventNotice()->time;
}


} // end of namespace

