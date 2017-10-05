/////////////////////////////////////////////////////////////////////////////
//! \file delay.cc  Delay block implementation (experimental)
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  This module contains implementation of continuous delay class
//
//  classes:
//     Delay -- real delay blocks
//     SIMLIB_Delay -- internal class for registration of delay blocks
//     SIMLIB_DelayBuffer -- delay memory
//
// LIMITS:
//     dt <= MaxStep --- problem with too small delay time
//     increasing dt --- problem with buffer length
//

////////////////////////////////////////////////////////////////////////////
// interface
//

#include "simlib.h"
#include "delay.h"              // extra header, TODO: move to simlib.h
#include "internal.h"

#include <deque>                // for buffer implementation
#include <list>                 // for registration list of all delay blocks


////////////////////////////////////////////////////////////////////////////
// implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

////////////////////////////////////////////////////////////////////////////
/// continuous delay block
class SIMLIB_Delay {
    static std::list<Delay *> *listptr; //!< list of delay objects -- singleton
  public:
    static void Register(Delay *p) {    //!< must be called by Delay ctr
        if( listptr == 0 ) Initialize();
        listptr->push_back(p);
    }
      static void UnRegister(Delay *p) {//!< must be called from Delay destructor
        listptr->remove(p);
        if( listptr->empty() ) Destroy();// is really important???
    }
  private:
    static void Initialize() {          //!< initialize delay subsystem
        listptr = new std::list<Delay*>();      // create new list of delays
        // install 'hooks' into simulation control algorithm:
        INSTALL_HOOK( Delay, SIMLIB_Delay::SampleAll );
        INSTALL_HOOK( DelayInit, SIMLIB_Delay::InitAll );
    }
    static void Destroy() {     //!< should be called by ExitSimulation()? ###???
        delete listptr;         // remove list
        listptr = 0;
        // disable hooks into simulation control algorithm
        INSTALL_HOOK( Delay, 0 );
        INSTALL_HOOK( DelayInit, 0 );
    }
    /// function to scan inputs of all delay objects
    static void SampleAll() {   // called each continuous step (and more)
        if( listptr == 0 ) return;      // ### should never be reached (2remove)
        std::list<Delay *>::iterator i;
        for( i=listptr->begin(); i!=listptr->end(); ++i) // for each delay object
            (*i)->Sample();     // sample input value
    }
    /// function to initialize all delay objects
    static void InitAll() {     // called at Init()
        if( listptr == 0 ) return;      // no delays ###
        std::list<Delay *>::iterator i;
        for( i=listptr->begin(); i!=listptr->end(); ++i) // for each delay object
            (*i)->Init();       // set initial value
    }
};

// static member must be initializad
std::list<Delay *> *SIMLIB_Delay::listptr = 0;


#ifndef SIMLIB_public_Delay_Buffer
/// abstract base class for delayed signal samples storage
struct Delay::Buffer {
    virtual void put(double value, double time) = 0;    //!< store value
    virtual double get(double time) = 0;        //!< read interpolated value
    virtual void clear() = 0;                   //!< initialize buffer
    virtual ~Buffer() {};
};
#endif

/////////////////////////////////////////////////////////////////////////////
/// memory for delayed pairs (Time,value)
///
/// This buffer inherits interface from Delay::Buffer (we can use various
/// implementations later)
/// method get() does linear interpolation ? TODO: should be split
///
class SIMLIB_DelayBuffer : public Delay::Buffer { // memory for delayed signal
    /// pair (t,val) for storing in buffer TODO: use std::tuple
    struct Pair {
        double time;    //<! sample time
        double value;   //<! sampled value
        Pair(double t, double v) : time(t), value(v) {}
        bool operator == (Pair &p) { return p.time==time && p.value==value; }
    };
    std::deque<Pair> buf;       //!< storage for samples
    Pair last_insert;           //!< last inserted value (for optimization)

 public:

    SIMLIB_DelayBuffer(): buf(), last_insert(-2,0) { /*empty*/ }

    virtual void clear() {
        last_insert = Pair(-2,0); // we need it for optimization
        buf.clear();            // empty buffer
    }

    virtual void put(double value, double time) {
        Pair p(time,value);
#ifndef NO_DELAY_OPTIMIZATION
        // TODO: can be improved using interpolation
        if( last_insert == p )  // do not allow duplicate records
            return;
        last_insert = p;
#endif
        buf.push_back(p);       // add at buffer end
    }

    virtual double get(double time) // get delayed value (with interpolation)
    {
        // TODO: this code is EXPERIMENTAL ### (needs optimization)
        Pair p(-1,0);
        Pair l(-1,0);
        std::deque<Pair>::iterator i;
        int n; // number of records (samples) skipped
        // ASSERT: there should be at least one record in the buffer
        for( n=0, i=buf.begin(); i!=buf.end(); ++i ) {
            l = p;
            p = *i;
            n++;
            if( p.time > time ) break;
        }
        // ASSERT: n>0
        if( n < 2 )     // we want time before first recorded sample
            return p.value;     // use first buffer value as default
        else {          // there are at least two samples recorded
            if( p.time < time ) {          // delay too small ###
                SIMLIB_error(DelayTimeErr);// TODO: ### do it better
            }
            // standard situation
            const int LIMIT=2; // >=2  --- allows change of delay if bigger
                               // WARNING: slow for bigger LIMIT
            // remove old items in buffer
            for(; n>LIMIT; n--)
                buf.pop_front(); // remove front records
            // linear interpolation
            double dt = p.time - l.time;
            double dy = p.value - l.value;
            if( dt <= 0.0) { // ASSERT: dt > 0
                SIMLIB_error(DelayTimeErr);
            }
            return l.value + dy*(time-l.time)/dt;
        }
    } // get
}; // class SIMLIB_DelayBuffer


/////////////////////////////////////////////////////////////////////////////
/// initialize and register delay block
Delay::Delay(Input i, double _dt, double ival) :
    aContiBlock1( i ),                  // input block-expression
    last_time( Time ),                  // last sample time
    last_value( ival ),                 // last sample value
    buffer( new SIMLIB_DelayBuffer ),   // allocate delay buffer
    dt( _dt ),                          // Parameter: delay time
    initval( ival )                     // initial value of delay block
{
    Dprintf(("Delay::Delay(in=%p, dt=%g, ival=%g)", &i, _dt, ival));
    SIMLIB_Delay::Register( this );     // register delay in list of delays
    Init(); // initialize -- important for dynamically created delays
}

/////////////////////////////////////////////////////////////////////////////
/// remove buffer and delay block from list
Delay::~Delay()
{
    Dprintf(("Delay::~Delay()"));
    delete buffer;                      // free the delay buffer
    SIMLIB_Delay::UnRegister(this);     // remove from delay list
}

/////////////////////////////////////////////////////////////////////////////
/// initialize delay status
///
/// called automatically by Init(....) function
/// initialize: buffer and last output value
/// WARNING: does not change dt ### (maybe problem for multiple experiments)
/// TODO: evaluate input expression of delay block?
void Delay::Init() {
    buffer->clear();                    // empty buffer
    buffer->put( last_value=initval, last_time=Time );  // set initial value
}


/////////////////////////////////////////////////////////////////////////////
/// sample input value
void Delay::Sample()
{
    Dprintf(("Delay::Sample()"));
    buffer->put( InputValue(), Time );  // store into buffer
}

/////////////////////////////////////////////////////////////////////////////
/// get delayed output value
double Delay::Value()
{
    Dprintf(("Delay::Value()"));
    double oldtime = Time - dt;         // past time
    if( last_time != oldtime ) {        // is not already computed?
        last_value = buffer->get( oldtime );    // get delayed value
        last_time = oldtime;
    }
    return last_value;
}

/////////////////////////////////////////////////////////////////////////////
/// change delay time
///
/// TODO: EXPERIMENTAL --- this should be used with care
///
double Delay::Set(double newdelay)
{
   double last = dt;
   if( newdelay>=0.0 && newdelay<=Time )  // FIXME: condition is too weak ###
      dt = newdelay;
   return last;
}

} // namespace

