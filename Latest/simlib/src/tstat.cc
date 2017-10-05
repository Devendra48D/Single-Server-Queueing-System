/////////////////////////////////////////////////////////////////////////////
//! \file tstat.cc time dependent statistics
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
// class TStat --- time statistic
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


////////////////////////////////////////////////////////////////////////////
//  constructors
//
TStat::TStat(double initval):
  sxt(0), sx2t(0),
  min(initval), max(initval),
  t0(Time), tl(Time),     // time of initialization and last op
  xl(initval),            // last value
  n(0UL)                  // # of rec
{
  Dprintf(("TStat::TStat()"));
  // Clear()
}

TStat::TStat(const char *name, double initval) :
  sxt(0), sx2t(0),
  min(initval), max(initval),
  t0(Time), tl(Time),
  xl(initval),
  n(0UL)
{
  Dprintf(("TStat::TStat(\"%s\")",name));
  SetName(name);
  // Clear()
}

////////////////////////////////////////////////////////////////////////////
//  destructor
//
TStat::~TStat()
{
  Dprintf(("TStat::~TStat() // \"%s\" ", Name()));
}

////////////////////////////////////////////////////////////////////////////
//  operator ()
//
void TStat::operator () (double x)
{
  if (Time<tl) SIMLIB_warning(TStatNotInitialized);
  double tt = xl*(double(Time)-tl);
  sxt  += tt;
  sx2t += xl*tt;
  xl = x;
  tl = Time;
  if(++n==1) min=max=x;   /// ??? is OK ??? ###
  else
  {
    if(x<min) min = x;
    if(x>max) max = x;
  }
}

////////////////////////////////////////////////////////////////////////////
//  Clear
//
void TStat::Clear(double initval)
{
  Dprintf(("TStat::Clear() // \"%s\" ", Name()));
  sxt = sx2t = 0;
  min = max = initval;
  t0 = tl = Time;
  xl = initval;       // last value
  n = 0UL;
}

////////////////////////////////////////////////////////////////////////////
//  TStat::MeanValue
//
double TStat::MeanValue() const
{
//  if(n==0)     Error(111); // return 0 ???? nesmysl !!!!
  if(Time<t0)
    SIMLIB_error(TStatNotInitialized);;
  if(Time==t0)  return xl;
  double sumxt = sxt + xl*(double(Time)-tl); // count last period
  return sumxt/(double(Time)-t0);
}

}
// end

