/////////////////////////////////////////////////////////////////////////////
//! \file waitunti.cc  Process::WaitUntil implementation
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  class WaitUntilList --- not good implementation
//  (uses list of waiting processes, and checks conditions after each
//  activated event)
//  better implementation will use objects in WUexpressions
//
// 199808  updated:  uses standard list<>

////////////////////////////////////////////////////////////////////////////
// interface
//
#include "simlib.h"
#include "internal.h"
#include <list>


////////////////////////////////////////////////////////////////////////////
// implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

////////////////////////////////////////////////////////////////////////////
// class WaitUntilList --- singleton
//
class WaitUntilList {
    typedef std::list<Process *> container_t;
    container_t l;
    static WaitUntilList *instance;   // unique list
  public:
    typedef container_t::iterator iterator;
    static iterator begin() { return instance->l.begin(); }
    static iterator end() { return instance->l.end(); }
    static bool empty() { return instance->l.empty(); }
    static void InsertCurrent();     // insert current process into list
    static void GetCurrent();        // get current process
    static void WU_hook(); // active: next process in WUlist or 0
    static void Remove(Process *p) { // find and remove p
        Dprintf(("WaitUntil::Remove(%s)", p->Name()));
        instance->l.remove(p); // should be in list
    }
    static void clear();    // empty
    static void create() {  // create single instance
        if(instance==0) instance = new WaitUntilList;
        else            SIMLIB_internal_error(); // called twice
        INSTALL_HOOK(WUclear, WaitUntilList::clear);
        SIMLIB_atexit(destroy); // last SIMLIB module cleanup calls this
    }
    static void destroy() {  // destroy single instance
        clear();             // remove all contents
        delete instance;
        instance = 0;
    }
  private:
    WaitUntilList() { Dprintf(("WaitUntilList::WaitUntilList()")); }
    ~WaitUntilList() { Dprintf(("WaitUntilList::~WaitUntilList()")); }
    // destructor never called ###???
    static iterator current;
#ifndef NDEBUG
    friend void WU_print();
#endif
};

#ifndef NDEBUG
    void WU_print() {
       _Print("WaitUntilList:\n");
       if(WaitUntilList::instance == 0) { _Print("none\n"); return; }
       WaitUntilList::iterator i = WaitUntilList::begin();
       for( int n=0 ; i!=WaitUntilList::end() ; ++i, ++n )
         _Print(" [%d] %s\n", n, (*i)->Name() );
    }
#endif

// WaitUntilList single instance
WaitUntilList *WaitUntilList::instance = 0; // static
WaitUntilList::iterator WaitUntilList::current; // static

////////////////////////////////////////////////////////////////////////////
static bool flag = false; // valid iterator in WUList
////////////////////////////////////////////////////////////////////////////
// main WUlist interface function
void WaitUntilList::WU_hook() { // get ptr to next process in WUlist or 0
    Dprintf(("WaitUntilList::WU_hook"));
    //  PRECONDITION: WUList not empty
    if(WaitUntilList::empty()) // this should never happen
        SIMLIB_internal_error();

    if(!flag) { // start processing, (first call or after remove)
        current = WaitUntilList::begin(); // reset to first process
        flag = true;
        // loop_count++;
        // if(loop_count>LIMIT) error("waituntil-loop");
        SIMLIB_Current = *current;  // always OK
        return;
    }
    ++current;  // next waiting process
    if( current != WaitUntilList::end() ) { // not end
        SIMLIB_Current = *current;
        return;
    }
    flag = false; // no next process --- end of WaitUntil processing
    // loop_count = 0;
    SIMLIB_Current = 0; // not needed ???###
    return;
}

////////////////////////////////////////////////////////////////////////////
// Process::
////////////////////////////////////////////////////////////////////////////
// _WaitUntil --- wait to condition
// this is hidden by macro WaitUntil(b), useable in Process::Behavior
//
bool Process::_WaitUntil(bool test)
{
  Dprintf(("%s._WaitUntil(%s)", Name(), test?"true":"false" ));
  if(test) {                    // true --- end of wait
    WaitUntilList::GetCurrent(); // ***** remove from WUList
    _wait_until = false;        // not in WUlist
    return false;               // continue checking WaitUntil condition
  } else {                      // false --- wait
    if (SIMLIB_Current != this) SIMLIB_internal_error();
    WaitUntilList::InsertCurrent(); // ***** insert into WUList
    _wait_until = true;         // is in WUlist
    Passivate();                // deactivation = wait
    return true;                // repeat test (after activation)
  }
}

////////////////////////////////////////////////////////////////////////////
// _WaitUntilRemove() --- remove process from WUlist (called from destructor)
//
void Process::_WaitUntilRemove() {
    if(_wait_until)
       WaitUntilList::Remove(this);
    _wait_until = false; // is not in WUlist
}


////////////////////////////////////////////////////////////////////////////
// InsertCurrent --- insert current process reference into WUlist
//                   (only called from Process::_WaitUntil)
//
void WaitUntilList::InsertCurrent()
{
    if(flag) return; // is in WUlist
    //CONDITION: current process is not in WUlist
    Process *e = (Process*) SIMLIB_Current; // static_cast<>
    Dprintf(("WaitUntilList.Insert(%s)", e->Name()));
    if(instance==0)
        create(); // create singleton instance
    if(empty())   // it was empty
        INSTALL_HOOK(WUget_next, WaitUntilList::WU_hook); // install hook
    iterator pos;
    for( pos = begin(); // find place from beginning
         pos != end() && (*pos)->Priority >= e->Priority;  // higher first
         ++pos ) { /*empty*/ }
    instance->l.insert(pos,e);  // insert at position
    //e->_wait_until = true; // mark process as inserted
}

////////////////////////////////////////////////////////////////////////////
// GetCurrent --- get selected process from WUlist
//                (called from Process::* )
//
void WaitUntilList::GetCurrent()
{
  if(!flag) return; // process is not in WUlist
  //PRECONDITION: WUlist is initialized, not empty
  Process *p = *current;
  Dprintf(("WaitUntilList.Get(); // \"%s\" ", p->Name()));
  instance->l.erase(current); // remove item pointed by iterator (fast)
  if(empty())
    INSTALL_HOOK(WUget_next, 0); // uninstall hook if last item removed
  flag = false;           // iterator invalid, start from beginning
}

////////////////////////////////////////////////////////////////////////////
// clear --- remove all records (called from Init())
//
void WaitUntilList::clear()
{
    if(instance==0) return;
    // remove all processes in WaitUntilList
    // we can do this, because all processes in list are passivated
    iterator i=begin();
    while(i!=end()) { // destroy all processes in WUlist
       Process *p = *i;
       ++i;
       p->_WaitUntilRemove();        // unmark and remove process
       if( p->isAllocated() ) delete p; // the same behavior as Calendar###???
    }
    if(!instance->l.empty())
        SIMLIB_internal_error(); // for sure
    INSTALL_HOOK(WUget_next, 0); // uninstall hook if empty
}


} // end

