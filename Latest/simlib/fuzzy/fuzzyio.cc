// cz - pøíli¹ ¾lu»ouèký kùò úpìl ïábelské ódy
/////////////////////////////////////////////////////////////////////////////
// fuzzyio.cc
//
// SIMLIB version: 2.16.3
// Date: 2001-04-04
// Copyright (c) 1999-2001  David Martinek, Dr. Ing. Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//
// Warning: this is EXPERIMENTAL code, interfaces can be changed
//
// Fuzzy subsystem for SIMLIB
// version 0.6 --- We apr 4 10:33:52 CEST 2001 
// 
/////////////////////////////////////////////////////////////////////////////
// Implementation of fuzzification and defuzzification part of module.
/////////////////////////////////////////////////////////////////////////////

#include "simlib.h"
#include "fuzzy.h"
#include "internal.h"
#include <cstdio>
#include <cassert>
#include <cmath>

namespace simlib3 {

/////////////////////////////////////////////////////////////////////////////
// fuzzification method
//

/** Fuzzify all membership functions.<br>Fuzzifikuje v¹echny funkce pøíslu¹nosti.*/
void FuzzyVariable::Fuzzify(double x)
{
    // optimize !
    // use m and count membership of x
  if ((x > m->max()) || (x < m->min()))
  {
    SIMLIB_error("Fuzzification error: value %lf out of range in fuzzy set \"%s\".", x, m->name());
  }
  for( unsigned i = 0; i < count(); i++ ) 
  { SetMembership(i,x); }
}

/////////////////////////////////////////////////////////////////////////////
// defuzzification methods
//

// indexed-centre-of-gravity
double i_g_centre(const FuzzyVariable &fs, double lim=0.0)
{
   double up = 0.0;
   double low = 0.0f;
   for(unsigned i = 0; i < fs.count(); i++) {
      double val = fs[i];
      if(val >= lim) {
         up += val*(i+1);
         low += val;
      }
   }
   up -= 1.0;
   if(low == 0.0) // ????? error ???
      return -1.0;
   return up/low;
}

// find maximum
static double find_max(const FuzzyVariable &fs)
{
   double value = 0.0;
   for(unsigned i=0; i<fs.count(); i++) 
      value = max(value, fs[i]);
   return value;
}

// mean-of-maximum 
double defuzMeanOfMax(const FuzzyVariable &fs)
{
   int n = 0;
   double up = 0.0;
   double top = find_max(fs); // height = maximum
   for(int i=fs.count()-1; i>=0; i--) {
      if(fs[i] == top) { // maximum
         up += fs.center(i);
         n++;
      }
   }
   assert(n != 0); // is never 0
   return up/n;  // mean value
}

// min-of-maximum 
double defuzMinOfMax(const FuzzyVariable &fs)
{
   double top = find_max(fs); // height = maximum
   for(unsigned i=0; i<fs.count(); i++) {
      if(fs[i] == top) { // first maximum
         return fs.center(i);
      }
   }
   assert(0);
}

// max-of-maximum
double defuzMaxOfMax(const FuzzyVariable &fs)
{
   double top = find_max(fs); // height = maximum
   for(int i=fs.count()-1; i>=0; i--) {
      if(fs[i] == top) { // last maximum
         return fs.center(i);
      }
   }
   assert(0);
}

// Defuzzification method "discrete-center-of-gravity".
double defuzDCOG(const FuzzyVariable &fs)
{
    double sum=0.0;
    double sumw=0.0;
    for(unsigned i = 0; i < fs.count(); i++) {
      double weight = fs[i];
      double x = fs.center(i); // center of membership function
      sum += x*weight;
      sumw += weight;
    }
    assert(sumw > 0.0);
    return sum/sumw;
}

} // namespace
