/////////////////////////////////////////////////////////////////////////////
//! \file zdelay.h   unit delay block interface
//
// Copyright (c) 1997-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  This is the interface for ZDelay blocks
//
//  WARNING: needs some testing --- this is the prototype
//

#ifndef __SIMLIB__
#   error "zdelay.h: 16: you must include simlib.h first"
#endif
#if (__SIMLIB__ < 0x0214)
#   error "zdelay.h: 19: requires SIMLIB version 2.14 and higher"
#endif

namespace simlib3 {

class ZDelay;
class ZDelayTimer;

////////////////////////////////////////////////////////////////////////////
// class ZDelayTimer --- clock for ZDelay blocks
//
class ZDelayTimer : public Event {
    ZDelayTimer(const ZDelayTimer&); // ##
    ZDelayTimer&operator=(const ZDelayTimer&); // ##
    double dt;          // clock period
    class ZDelayContainer;  // implementation-defined container
    ZDelayContainer *c;
    void Register(ZDelay*);   // insert into container
    void UnRegister(ZDelay*); // remove from container
    friend class ZDelay; // no one else can register
  public:
    static const bool Default = true;
    ZDelayTimer(double dt, bool is_default = false);
    //ZDelayTimer(ZDelayTimer *p, unsigned long divide_by);
    operator ZDelayTimer * () { return this; }
    void Behavior();
    void Init();  // called each Run()
    void Start(); // clock activation
    void Stop();  // stop clock
    void Set(double new_dt) { dt = new_dt; }
    ~ZDelayTimer();
};

////////////////////////////////////////////////////////////////////////////
// class ZDelay --- continuous signal / discrete time  delay blocks
//
class ZDelay : public aContiBlock1 {
    ZDelay(const ZDelay&);              // disable copy ctor
    void operator= (const ZDelay&);     // disable assignment
    double input_value;                 // temporary
    ZDelayTimer *clock;                 // timer-event for this block
    friend class ZDelayTimer;
  protected:
    virtual void SampleIn();    // sample input (called automatically by ZDelayTimer)
    virtual void SampleOut();   // sample output (called automatically)
  protected: // status
    double new_value;   // stored input value
    double old_value;   // output value (delayed signal)
  protected: // parameters
    double initval;     // initial output value
    static ZDelayTimer * default_clock;
  public: // interface
    ZDelay( Input i, ZDelayTimer * clock = default_clock, double initvalue = 0 );
    ZDelay( Input i, double initvalue );
    ~ZDelay();
    void Init(double iv);       // set initial value of ZDelay block
    virtual void Init();        // initialize ZDelay block
    virtual double Value();     // output of ZDelay block
}; // class ZDelay

}

// end

