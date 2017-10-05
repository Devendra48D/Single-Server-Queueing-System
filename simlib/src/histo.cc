/////////////////////////////////////////////////////////////////////////////
//! \file histo.cc   Histogram implamentation
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  Histogram implementation
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

// LIMIT: maximum number of histogram items -- implementation dependent
// 80x86 => segment 64KB =>
const unsigned MAXHISTOCOUNT = 10000;

////////////////////////////////////////////////////////////////////////////
//  allocate
//
static unsigned *Alloc(unsigned n)
{
  unsigned *dptr = new unsigned[n];
//  if (!dptr)
//    SIMLIB_error(MemoryError);         // check (old version)
  for(unsigned i=0; i<n; i++)
    dptr[i] = 0;                       // initialize
  return dptr;
}


////////////////////////////////////////////////////////////////////////////
//  constructors
//
Histogram::Histogram() :
  low(0),
  step(1),
  count(10)
{
  Dprintf(("Histogram::Histogram()"));
  dptr = Alloc(count+2);
}

Histogram::Histogram(double l, double s, unsigned c) :
  low(l),       // init low limit
  step(s),      // init step
  count(c)      // init count
{
  Dprintf(("Histogram::Histogram(%g,%g,%u)",l,s,c));
  if(s<=0)                      SIMLIB_error(HistoStepError);
  if(c==0 || c>MAXHISTOCOUNT)   SIMLIB_error(HistoCountError);
  dptr = Alloc(count+2);
}

Histogram::Histogram(const char *n, double l, double s, unsigned c) :
  low(l),       // init low limit
  step(s),      // init step
  count(c)      // init count
{
  Dprintf(("Histogram::Histogram(\"%s\",%g,%g,%u)",n,l,s,c));
  SetName(n);                   // set object name
  if(s<=0)                      SIMLIB_error(HistoStepError);
  if(c==0 || c>MAXHISTOCOUNT)   SIMLIB_error(HistoCountError);
  dptr = Alloc(count+2);
}

////////////////////////////////////////////////////////////////////////////
//  destructor
//
Histogram::~Histogram()
{
  Dprintf(("Histogram::~Histogram() // \"%s\" ", Name()));
  delete[] dptr;
}

////////////////////////////////////////////////////////////////////////////
//  operator []
//
unsigned Histogram::operator [] (unsigned i) const
{
  if (i>count) i = count+1;
  return dptr[i];
}

////////////////////////////////////////////////////////////////////////////
//  operator ()  - value recording
//
void Histogram::operator () (double x)
{
  stat(x);
  if(x<low)
  {
    dptr[0]++;
    return;
  }
  unsigned ix = unsigned((x-low)/step);
  if(ix>count)
    dptr[count+1]++;
  else
    dptr[ix+1]++;
}

////////////////////////////////////////////////////////////////////////////
//  Init
//
void Histogram::Init(double l, double s, unsigned c)
{
  Dprintf(("Histogram::Init(%g,%g,%i)",l,s,c));
  low = l;                             // init low limit
  if(s<=0) SIMLIB_error(HistoStepError);
  step = s;                            // init step
  if(c==0 || c>MAXHISTOCOUNT) SIMLIB_error(HistoCountError);
  if (dptr && count!=c)
  {
    delete[] dptr;
    dptr = 0;
  }
  if (!dptr)
  {
    count = c;                         // init count
    dptr = Alloc(count+2);
  }
  Clear();                             // init
}

////////////////////////////////////////////////////////////////////////////
//  Clear
//
void Histogram::Clear()
{
  Dprintf(("Histogram::Clear()"));
  for(unsigned i=0; i<count+2; i++)
    dptr[i] = 0;
  stat.Clear();
}

}
// end

