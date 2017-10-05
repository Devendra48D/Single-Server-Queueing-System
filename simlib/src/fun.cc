/////////////////////////////////////////////////////////////////////////////
//! \file fun.cc    Function blocks
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
// standard continuous function blocks
//


////////////////////////////////////////////////////////////////////////////
// interface
//

#include "simlib.h"
#include "internal.h"

#include <cmath>          // functions

////////////////////////////////////////////////////////////////////////////
// implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

////////////////////////////////////////////////////////////////////////////
/// constructor --- functions with single argument
//
Function1::Function1(Input i, double (*pf)(double))
  : aContiBlock1(i), f(pf) {
  Dprintf(("Function1::Function1(in)"));
}

double Function1::Value() {
  AlgLoopDetector _(this);
  double ret = f(InputValue());
  return ret;
}

const char *Function1::Name() const {
  if(HasName()) return _name;
  else return SIMLIB_create_tmp_name("Function1{%p}", this);
}

////////////////////////////////////////////////////////////////////////////
/// constructor --- functions with two arguments
//
Function2::Function2(Input i1, Input i2, double (*pf)(double,double))
  : aContiBlock2(i1,i2), f(pf) {
  Dprintf(("Function2::Function2(in,in)"));
}

double Function2::Value() {
  AlgLoopDetector _(this);
  double ret = f(Input1Value(), Input2Value());
  return ret;
}

const char *Function2::Name() const {
  if(HasName()) return _name;
  else return SIMLIB_create_tmp_name("Function2{%p}", this);
}


////////////////////////////////////////////////////////////////////////////
// SIMLIB local definitions:
//

static double sign(double x)
{
    if (x == 0.0)
        return 0.0;
    if (x > 0.0)
        return 1.0;
    else
        return -1.0;
}


////////////////////////////////////////////////////////////////////////////
// Implementation of block functions (prototypes are in simlib.h)
// using Fun(block expression) creates dynamic object/block, connects it
// to block expression, and returns reference to this block

Input Abs(Input x)              { return new Function1(x, fabs); }

Input Sin(Input x)              { return new Function1(x, sin); }
Input Cos(Input x)              { return new Function1(x, cos); }
Input Tan(Input x)              { return new Function1(x, tan); }

Input ASin(Input x)             { return new Function1(x, asin); }
Input ACos(Input x)             { return new Function1(x, acos); }
Input ATan(Input x)             { return new Function1(x, atan); }
Input ATan2(Input y, Input x)   { return new Function2(y, x, atan2); }

Input Exp(Input x)              { return new Function1(x, exp); }
Input Log10(Input x)            { return new Function1(x, log10); }
Input Ln(Input x)               { return new Function1(x, log); }
Input Pow(Input x, Input y)     { return new Function2(x, y, pow); }

Input Sign(Input x)             { return new Function1(x, sign); }

Input Sqrt(Input x)             { return new Function1(x, sqrt); }

Input Min(Input x, Input y)     { return new Function2(x, y, min); }
Input Max(Input x, Input y)     { return new Function2(x, y, max); }


} // namespace

