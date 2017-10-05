/////////////////////////////////////////////////////////////////////////////
//! \file  stat.cc Statistics
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
// class Stat implementation
//

////////////////////////////////////////////////////////////////////////////
// interface
//

#include "simlib.h"
#include "internal.h"

#include <cmath>     // sqrt()


////////////////////////////////////////////////////////////////////////////
// implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

////////////////////////////////////////////////////////////////////////////
//  operator ()  --- record value
//
void Stat::operator () (double x)
{
  sx  += x;
  sx2 += x*x;
  if(++n==1) min=max=x;
  else {
    if(x<min) min = x;
    if(x>max) max = x;
  }
};


////////////////////////////////////////////////////////////////////////////
//  constructors
//
Stat::Stat(const char *name) :
  sx(0), sx2(0),
  min(0), max(0),
  n(0)
{
  Dprintf(("Stat::Stat(\"%s\")",name));
  SetName(name);
}

Stat::Stat() :
  sx(0), sx2(0),
  min(0), max(0),
  n(0)
{
  Dprintf(("Stat::Stat()"));
}

////////////////////////////////////////////////////////////////////////////
//  destructor
//
Stat::~Stat()
{
  Dprintf(("Stat::~Stat() // \"%s\" ", Name()));
}

////////////////////////////////////////////////////////////////////////////
// Stat::Clear --- initialize
//
void Stat::Clear()
{
  sx = sx2 = 0;    // sums
  min = max = 0;
  n = 0;           // # of records
}


////////////////////////////////////////////////////////////////////////////
//  Stat::MeanValue
//
double Stat::MeanValue() const
{
  if (n==0) SIMLIB_error(StatNoRecError);
  return sx/n;
}

////////////////////////////////////////////////////////////////////////////
//  Stat::StdDev  --- standard deviation
//
double Stat::StdDev() const
{
  if (n<2)  SIMLIB_error(StatDispError);
  double mv = sx/n;
  return sqrt((sx2-n*mv*mv)/(n-1));
}

}
// end
