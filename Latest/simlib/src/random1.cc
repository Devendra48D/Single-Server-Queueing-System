/////////////////////////////////////////////////////////////////////////////
//! \file random1.cc  Random number generators - basic
//
// Copyright (c) 1991-2007 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
// base random number generator
// generate uniform distribution in the range 0 .. 0.99999999999....
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

// external functions
void   RandomSeed(long seed);     // initialize random number seed
double Random();                  // base uniform generator 0-0.999999...
void   SetBaseRandomGenerator(double (*new_gen)()); // change base gen.

SIMLIB_IMPLEMENTATION;


// TODO: change the basic generator to something better

#if (LONG_MAX<(1ULL<<32))
typedef long myint32;     // long has 32 bits
#else
typedef int myint32;      // long has >32 bits
#endif

////////////////////////////////////////////////////////////////////////////
// some constants for base generator
//

const myint32 INICONST   = 1537L;
const myint32 MULCONST   = 1220703125L;
// period = 536870912 only!

const myint32 MAXLONGINT = 0x7FFFFFFFUL;
const myint32 SIGNBIT    = 0x80000000UL;

////////////////////////////////////////////////////////////////////////////
// random generator seed
//

static myint32 SIMLIB_RandomSeed = INICONST ;

////////////////////////////////////////////////////////////////////////////
// RandomSeed - initialization of random generator
//
void RandomSeed(long seed)
{
  SIMLIB_RandomSeed = seed;
}

////////////////////////////////////////////////////////////////////////////
// SIMLIB_RandomBase --- default base uniform random number generator
//
// uses linear congruential method
// (not very good)
//
double SIMLIB_RandomBase()  // range <0..1)
{
  SIMLIB_RandomSeed *= MULCONST;
  SIMLIB_RandomSeed &= MAXLONGINT; // strip sign bit
//  _Print("random=%lx\n", (long)SIMLIB_RandomSeed);
  double r = static_cast<double>(SIMLIB_RandomSeed)/MAXLONGINT;
  // assert: if( r<0.0 || r>=1.0 ) SIMLIB_error("Random() out of range");
  return r;
}

////////////////////////////////////////////////////////////////////////////
// pointer to base generator
//
static double (*SIMLIB_RandomBasePtr)() = SIMLIB_RandomBase;

////////////////////////////////////////////////////////////////////////////
// Random --- base uniform random number generator
//
// generate pseudorandom number in the range  0 .. 0.999999999999999999999
//
double Random()
{
  return SIMLIB_RandomBasePtr();
}

////////////////////////////////////////////////////////////////////////////
// SetBaseRandomGenerator --- change base random number generator
//
void SetBaseRandomGenerator(double (*new_gen)())
{
  if(new_gen)
    SIMLIB_RandomBasePtr = new_gen;
  else
    SIMLIB_RandomBasePtr = SIMLIB_RandomBase; // default value
}

}
// end

