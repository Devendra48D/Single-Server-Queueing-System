/////////////////////////////////////////////////////////////////////////////
//! \file delay.h       Simple delay block interface
//
// Copyright (c) 1998-2016 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  This is the interface for delay blocks
//
//  TODO: WARNING: needs some testing --- this is the prototype
//

#ifndef __SIMLIB__
#   error "delay.h: 16: you should include simlib.h first"
#endif
#if __SIMLIB__ < 0x0213
#   error "delay.h: 19: requires SIMLIB version 2.13 and higher"
#endif

namespace simlib3 {

////////////////////////////////////////////////////////////////////////////
/// continuous signal delay block
class Delay : public aContiBlock1 {
    Delay(const Delay&);                 // disable copy ctor
    void operator= (const Delay&);       // disable assignment
  protected: // status
  public:  //TODO:## repair
#ifdef SIMLIB_public_Delay_Buffer
    /// abstract base class: memory for delayed signal samples
    struct Buffer {
        virtual void put(double value, double time) = 0; //!< sample
        virtual double get(double time) = 0; //!< get interpolated value
        virtual void clear() = 0; //!< initialize buffer
        virtual ~Buffer() {};
    };
#else
    struct Buffer;              //!< memory for delayed signal samples
#endif
  protected: //TODO:## repair
    double last_time;           //!< last output time (for optimization)
    double last_value;          //!< last output value
    Buffer *buffer;             //!< memory for past values
  protected: // parameters
    double dt;                  //!< Parameter: delay time (should be > MaxStep)
    double initval;             //!< initial value (used at start)
  public: // interface
    Delay(Input i, double dt, double initvalue=0); // dt > MaxStep
    ~Delay();
    void Init();                //!< initialize delay block
    void Sample();              //!< sample input (called automatically)
    virtual double Value();     //!< output of delay block
    double Set(double newDT);   //!< change delay time (EXPERIMENTAL)
}; // class Delay

} // namespace

