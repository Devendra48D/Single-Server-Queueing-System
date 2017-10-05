/////////////////////////////////////////////////////////////////////////////
//! \file sampler.cc  Periodic sampler
//
// Copyright (c) 1994-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
// description:
//    implementation of class Sampler
//    - calls global function (function) periodicaly (step)
//    - automatic initialization by Init() (set on)
//    - automatic activation by Run() (if on)
//    - Start/Stop
//

////////////////////////////////////////////////////////////////////////////
// interface
//

#include "simlib.h"
#include "internal.h"

////////////////////////////////////////////////////////////////////////////
// implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

// inicializace
Sampler *Sampler::First = 0;

////////////////////////////////////////////////////////////////////////////
// constructor
//
Sampler::Sampler(void(*pf)(), double dt) :
    Next(0),
    function(pf),
    last(-1),
    step((dt>0.0) ? dt : 0.0),
    on(true)
{
  Dprintf(("Sampler::Sampler(%p,%g)", pf, dt));
  if( First==0 ) { // first created sampler
     INSTALL_HOOK( SamplerInit, Sampler::InitAll );
     INSTALL_HOOK( SamplerAct, Sampler::ActivateAll );
  }
  // insert into list:
  Next = First;
  First = this;
}

////////////////////////////////////////////////////////////////////////////
// destructor
//
Sampler::~Sampler()
{
  Dprintf(("Sampler::~Sampler() // \"%p\" ", function));
  // remove from list:
  if (this==First)
    First = Next;
  else
  {
    Sampler *i;
    for(i=First; i && i->Next!=this; i=i->Next) { /*empty*/ }
    if (i) i->Next = Next;
  }
  if( First==0 ) {
     INSTALL_HOOK( SamplerInit, 0 );
     INSTALL_HOOK( SamplerAct, 0 );
  }
}

////////////////////////////////////////////////////////////////////////////
// Output
//
void Sampler::Output() const
{
  Print("Sampler%p, function=%p, last=%g, on=%i\n",
         this,      function,    last,    on);
}


////////////////////////////////////////////////////////////////////////////
// Sample::Behavior --- call function and reactivate sample event
//
void Sampler::Behavior() {
  Dprintf(("Sampler::Behavior()"));
  Sample();                     // call of global function
  if( on && step > 0.0 )
    Activate( Time + step );    // schedule next sample
  else
    Passivate(); // should be passivated before ###????
}


////////////////////////////////////////////////////////////////////////////
// Sampler::InitAll --- init all samplers - called by Init()
// static
void Sampler::InitAll() {
  for( Sampler *i = First; i; i = i->Next ) {
    i->last = -1;
    i->on = true;
  }
}

////////////////////////////////////////////////////////////////////////////
// ActivateAll - activate all Samplers - called by Run()
// static
void Sampler::ActivateAll()
{
  for(Sampler *i=First; i; i=i->Next)
  {
    i->last = -1;               // really needed ???? ######
    if(i->on)                   // activate this one
      i->Activate();
  }
}

////////////////////////////////////////////////////////////////////////////
// Start() -- start o fampling
//
void Sampler::Start()
{
  on=true;
  Activate();
}

////////////////////////////////////////////////////////////////////////////
// Stop -- end of sampling
//
void Sampler::Stop()
{
  on=false;
  if(last==Time) // was sample at this time
    Passivate();
  else
    Activate();
}

////////////////////////////////////////////////////////////////////////////
//  Sample -- unconditionally
//
void Sampler::Sample()
{
  if(function)
    function(); // call global function
  last = Time;
}

////////////////////////////////////////////////////////////////////////////
//  SetStep -- step of sampling                 added 13.5.95
//
double Sampler::SetStep(double dt)
{
  double laststep=step;
  step = (dt>0.0) ? dt : 0.0;
  return laststep;
}

} // end of SAMPLER.CPP

