/////////////////////////////////////////////////////////////////////////////
//! \file zdelay.cc   unit delay block implementation
//
// Copyright (c) 1998-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  This module contains implementation of discrete-time delay class
//
//  classes:
//     ZDelay -- discrete-time delay blocks
//     ZDelayTimer -- clock generating class for ZDelay objects
//     SIMLIB_ZDelayTimer -- internal class
//
// TODO: add dividers

////////////////////////////////////////////////////////////////////////////
// interface
//

#include "simlib.h"
#include "zdelay.h"
#include "internal.h"

// we use standard C++ library (STL)
#include <set>          // for ZDelay objects container implementation
#include <list>         // for list of ZDelayTimers

// using namespace std; // GNU C++ doesn't have namespaces yet :-(

////////////////////////////////////////////////////////////////////////////
// implementation
//
namespace simlib3 {

SIMLIB_IMPLEMENTATION;

////////////////////////////////////////////////////////////////////////////
// SIMLIB_ZDelayTimer --- register of all ZDelayTimer objects
//
class SIMLIB_ZDelayTimer {
    typedef std::list<ZDelayTimer *> container_t; // type of container we use
    static container_t *container;      // list of delay objects -- singleton
  public: // interface
    static void Register(ZDelayTimer *p) { // called from ZDelayTimer constructor
        if( container == 0 )
            Initialize();
        container->push_back(p);
    }
    static void UnRegister(ZDelayTimer *p) { // called from ZDelayTimer destructor
        container->remove(p);
        if( container->size() == 0 )
            Destroy();
    }
  private: // implementation
    static void Initialize() {          // initialize delay subsystem
        container = new container_t();  // create new list of delays
        // install hook in simulation control algorithm:
        INSTALL_HOOK( ZDelayTimerInit, SIMLIB_ZDelayTimer::InitAll );
    }
    static void Destroy() {     // should be called by ExitSimulation()? ###???
        delete container;       // remove list
        container = 0;
        // disable hook in simulation control algorithm
        INSTALL_HOOK( ZDelayTimerInit, 0 );
    }
    // InitAll --- function to initialize all ZDelayTimer objects
    static void InitAll() {     // called at Init()
        if( container == 0 ) return;    // no delays ###
        container_t::iterator i;
        for( i=container->begin(); i!=container->end(); i++) // for each delay object
            (*i)->Init();       // set initial value
    }
};

// SINGLETON: static member must be initializad
SIMLIB_ZDelayTimer::container_t * SIMLIB_ZDelayTimer::container = 0;



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// ZDelayTimer --- discrete-time delay block timer implementation
//

// singleton -- default ZDelayTimer
ZDelayTimer * ZDelay::default_clock = 0;

/////////////////////////////////////////////////////////////////////////////
// ZDelayTimer::ZDelayContainer --- container for associated ZDelay blocks
//
class ZDelayTimer::ZDelayContainer { // implementation-defined container
        typedef std::set<ZDelay*> container_t;
        container_t c;
    public:
        ZDelayContainer(): c() {}
        typedef container_t::iterator iterator;
        iterator begin()        { return c.begin(); }
        iterator end()          { return c.end(); }
        void insert(ZDelay * p) { c.insert(p); }
        void erase(ZDelay * p)  { c.erase(p); }
        void clear()            { c.clear(); }
}; // class ZDelayTimer::ZDelayContainer

/////////////////////////////////////////////////////////////////////////////
// constructor --- init & insert into global register
//
ZDelayTimer::ZDelayTimer(double step, bool is_default) :
    dt(step),
    c(new ZDelayContainer)
{   // constructor body
    if(is_default) {
        ZDelay::default_clock = this;
    }
    SIMLIB_ZDelayTimer::Register(this); // register in list
}

ZDelayTimer::~ZDelayTimer()
{
    if( ZDelay::default_clock == this)
        ZDelay::default_clock = 0; // default timer deleted
    // clear all items
    ZDelayContainer::iterator i=c->begin();
    for( ; i!=c->end(); i++) // unregister all ZDelays
        (*i)->clock = 0;
    c->clear(); // remove all items
    delete c;
    SIMLIB_ZDelayTimer::UnRegister(this); // remove from list
}

/////////////////////////////////////////////////////////////////////////////
// ZDelayTimer::Init --- initialize all connected ZDelays and activate
//
void ZDelayTimer::Init()  // called each Run()
{
    ZDelayContainer::iterator i=c->begin();
    for( ; i!=c->end(); i++) // init all assotiated ZDelays
        (*i)->Init();
    Start();
}

/////////////////////////////////////////////////////////////////////////////
// ZDelayTimer::Start --- activate
//
void ZDelayTimer::Start() // clock activation
{
    Activate();
}

/////////////////////////////////////////////////////////////////////////////
// ZDelayTimer::Stop --- passivate
//
void ZDelayTimer::Stop() // clock deactivation
{
    Passivate();
}

/////////////////////////////////////////////////////////////////////////////
// ZDelayTimer::Behavior --- sample all associated ZDelays, reschedule
//
void ZDelayTimer::Behavior()
{
    ZDelayContainer::iterator i;
    for( i=c->begin(); i!=c->end(); i++) // evaluate all inputs
        (*i)->SampleIn();
    for( i=c->begin(); i!=c->end(); i++) // store all new output values
        (*i)->SampleOut();
    Activate( Time + dt );
}

/////////////////////////////////////////////////////////////////////////////
// ZDelayTimer::Register --- add ZDelay to list
//
void ZDelayTimer::Register(ZDelay*p)
{
    c->insert(p);
    p->clock = this;
}

/////////////////////////////////////////////////////////////////////////////
// ZDelayTimer::UnRegister --- remove ZDelay from list
//
void ZDelayTimer::UnRegister(ZDelay*p)
{
    c->erase(p);
    p->clock = 0;
}



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// ZDelay --- discrete-time delay block implementation
//

/////////////////////////////////////////////////////////////////////////////
// ZDelay constructor --- initialize and register delay block
//
ZDelay::ZDelay(Input i, ZDelayTimer *p, double ival) :
    aContiBlock1( i ),                  // input expression
    input_value( ival ),
    clock( p ),
    new_value( ival ),
    old_value( ival ),
    initval( ival )                     // initial value of delay
{   // constructor body
    Dprintf(("ZDelay::ZDelay%p(in=%p, timer=%p, ival=%g)", this, &i, p, ival));
    if(clock==0)
        SIMLIB_internal_error(); // change ##############!!!!!
    clock->Register( this ); // register in timer
    Init(); // initialize -- important for dynamically created delays
}

ZDelay::ZDelay( Input i, double ival ) :
    aContiBlock1( i ),                  // input expression
    input_value( ival ),
    clock( default_clock ),
    new_value( ival ),
    old_value( ival ),
    initval( ival )                     // initial value of delay
{   // constructor body
    Dprintf(("ZDelay::ZDelay%p(in=%p, ival=%g)", this, &i, ival));
    if(clock==0)
        SIMLIB_internal_error(); // change ##############!!!!!
    clock->Register( this ); // register in timer
    Init(); // initialize -- important for dynamically created delays
}


/////////////////////////////////////////////////////////////////////////////
// ZDelay destructor --- remove buffer and delay from list
//
ZDelay::~ZDelay()
{
    Dprintf(("ZDelay::~ZDelay%p()", this));
    if(clock)
        clock->UnRegister( this );      // remove from delay list
}

/////////////////////////////////////////////////////////////////////////////
// ZDelay::Init --- initialize delay status
// called automatically by Run()
void ZDelay::Init() {
    Dprintf(("ZDelay::Init()"));
    input_value = new_value = old_value = initval;
}

void ZDelay::Init(double iv) { // set initial value of ZDelay block
    initval = iv;
    Init();
}

/////////////////////////////////////////////////////////////////////////////
// ZDelay::Sample --- sample input value
//
void ZDelay::SampleIn()
{
    Dprintf(("ZDelay::SampleIn()"));
    input_value = InputValue(); // store new input value
}

void ZDelay::SampleOut()
{
    Dprintf(("ZDelay::SampleOut()"));
    old_value = new_value;      // set output value (delayed)
    new_value = input_value;    // store new input value for delay
}

/////////////////////////////////////////////////////////////////////////////
// ZDelay::Value --- get delay output value
//
double ZDelay::Value()
{
    Dprintf(("ZDelay::Value()"));
    return old_value; // delayed value
}

}
// end

