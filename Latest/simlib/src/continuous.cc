/////////////////////////////////////////////////////////////////////////////
//! \file continuous.cc  continuous blocks definitions
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
// description: news to version 2.0
//
//  - blocks for + - * / operations
//  - dynamical section replacement: _Dynamic()
//  - evaluation with alg. loop detections


////////////////////////////////////////////////////////////////////////////
// interface
//

#include "simlib.h"
#include "internal.h"

namespace simlib3 {


////////////////////////////////////////////////////////////////////////////
// implementation
//

SIMLIB_IMPLEMENTATION;

////////////////////////////////////////////////////////////////////////////
/// performs evaluation of integrators and status blocks
void SIMLIB_Dynamic() // called each step
{
  StatusContainer::ClearAllValueOK(); // zero flags ###
  StatusContainer::EvaluateAll();     // evaluation (with loop detection) ???
  IntegratorContainer::EvaluateAll(); // evaluation without loop detection
}


////////////////////////////////////////////////////////////////////////////
/// evaluation with algebraic loop detection
//
// TODO: use new implementation -- AlgLoopDetector, or remove
//
void aContiBlock::_Eval()
{
  if(isEvaluated)                      // was evaluated
    SIMLIB_error(AlgLoopDetected);     // recursive call
  isEvaluated = true;                  // eval-flag
  Eval();                              // evaluation of block
  isEvaluated = false;
}


////////////////////////////////////////////////////////////////////////////
/// constructor for blocks with one input
//
aContiBlock1::aContiBlock1(Input i) : input(i)
{
  if(input==this)
      SIMLIB_error(AlgLoopDetected);
}

/*
Input aContiBlock1::SetInput(Input i)
{
  if(i==this)    SIMLIB_error(AlgLoopDetected);
//  LoopCheck();                  // detect algebraic loops
  return input.Set(i);
}
*/

////////////////////////////////////////////////////////////////////////////
/// ctr for blocks with 2 inputs
//
aContiBlock2::aContiBlock2(Input i1, Input i2) : input1(i1), input2(i2)
{
  if(input1==this || input2==this)
      SIMLIB_error(AlgLoopDetected);
}

////////////////////////////////////////////////////////////////////////////
/// ctr for blocks with 3 inputs
//
aContiBlock3::aContiBlock3(Input i1, Input i2, Input i3)
  : input1(i1), input2(i2), input3(i3)
{
  if(input1==this || input2==this || input3==this )
    SIMLIB_error(AlgLoopDetected);
}

/*
void aContiBlock2::SetInputs(Input i1, Input i2)
{
  input1.Set(i1);
  input2.Set(i2);
  if(input1==this || input2==this) SIMLIB_error(AlgLoopDetected);
//  LoopCheck();                  // detect algebraic loops
}

Input aContiBlock2::SetInput1(Input inp)
{
  if(inp==this) SIMLIB_error(AlgLoopDetected);
  Input old = input1.Set(inp);
//  LoopCheck();                  // detect algebraic loops
  return old;
}

Input aContiBlock2::SetInput2(Input inp)
{
  if(inp==this) SIMLIB_error(AlgLoopDetected);
  Input old = input2.Set(inp);
//  LoopCheck();                  // detect algebraic loops
  return old;
}
*/


/// check expression for algebraic loops
double Expression::Value() { AlgLoopDetector _(this); return InputValue(); }


////////////////////////////////////////////////////////////////////////////
// _Xxxx classes are for internal use only -
// the objects are created automatically in block expressions,
// so there is no need to check algebraic loops

////////////////////////////////////////////////////////////////////////////
/// block sum of two inputs
class _Add : public aContiBlock2 {
  virtual void Eval() {}
  virtual void _Eval() {}
public:
  _Add(Input a, Input b): aContiBlock2(a,b) {
    Dprintf(("ctr: _Add[%p](in1,in2)", this));
  }
  ~_Add() {
    Dprintf(("dtr: _Add[%p]", this));
  }
  virtual double Value() { return Input1Value() + Input2Value(); }
  virtual const char *Name() const {
      if(HasName()) return _name;
      else return SIMLIB_create_tmp_name("_Add{%p}", this);
  }
};


////////////////////////////////////////////////////////////////////////////
/// block to subtract two inputs
class _Sub : public aContiBlock2 {
  virtual void Eval() {}
  virtual void _Eval() {}
public:
  _Sub(Input a, Input b): aContiBlock2(a,b) {
    Dprintf(("ctr: _Sub[%p](in1,in2)", this));
  }
  ~_Sub() {
    Dprintf(("dtr: _Sub[%p]", this));
  }
  virtual double Value() { return Input1Value() - Input2Value(); }
  virtual const char *Name() const {
      if(HasName()) return _name;
      else return SIMLIB_create_tmp_name("_Sub{%p}", this);
  }
};


////////////////////////////////////////////////////////////////////////////
/// multiplier block
class _Mul : public aContiBlock2 {
  virtual void Eval() {}
  virtual void _Eval() {}
public:
  _Mul(Input a, Input b): aContiBlock2(a,b) {
    Dprintf(("ctr: _Mul[%p](in1,in2)", this));
  }
  ~_Mul() {
    Dprintf(("dtr: _Mul[%p]", this));
  }
  virtual double Value() { return Input1Value() * Input2Value(); }
  virtual const char *Name() const {
      if(HasName()) return _name;
      else return SIMLIB_create_tmp_name("_Mul{%p}", this);
  }
};


////////////////////////////////////////////////////////////////////////////
/// divider block
class _Div : public aContiBlock2 {
  virtual void Eval() {}
  virtual void _Eval() {}
public:
  _Div(Input a, Input b): aContiBlock2(a,b) {
    Dprintf(("ctr: _Div[%p](in1,in2)", this));
  }
  ~_Div() {
    Dprintf(("dtr: _Div[%p]", this));
  }
  virtual double Value() { return Input1Value() / Input2Value(); }
  virtual const char *Name() const {
      if(HasName()) return _name;
      else return SIMLIB_create_tmp_name("_Div{%p}", this);
  }
};


////////////////////////////////////////////////////////////////////////////
// binary operators ...
////////////////////////////////////////////////////////////////////////////

/// block operator +
Input operator + (Input a, Input b) { return new _Add(a,b); }
/// block operator -
Input operator - (Input a, Input b) { return new _Sub(a,b); }
/// block operator *
Input operator * (Input a, Input b) { return new _Mul(a,b); }
/// block operator /
Input operator / (Input a, Input b) { return new _Div(a,b); }

/// square function
Input Sqr(Input x) { return new _Mul(x,x); }

////////////////////////////////////////////////////////////////////////////
/// unary minus block
class _UMinus: public aContiBlock1 {
  virtual void Eval() {}
  virtual void _Eval() {}
public:
  _UMinus(Input a): aContiBlock1(a) {
    Dprintf(("ctr: _UMinus[%p](in)", this));
  }
  ~_UMinus() {
    Dprintf(("dtr: _UMinus[%p]", this));
  }
  virtual double Value()    { return -InputValue(); }
  virtual const char *Name() const {
      if(HasName()) return _name;
      else return SIMLIB_create_tmp_name("_UMinus{%p}", this);
  }
};


////////////////////////////////////////////////////////////////////////////
// unary operators ...
////////////////////////////////////////////////////////////////////////////

/// unary - block operator
Input operator - (Input a) { return new _UMinus(a); }

////////////////////////////////////////////////////////////////////////////
/// block wrapper for simulation time
class _Time: public aContiBlock {
 public:
  _Time() {}
  virtual double Value () { return Time; }
  virtual const char *Name() const { return "T(Time)"; }
};

/// block -- simulation time
static class _Time _T;
/// simulation time block reference
aContiBlock & T = _T;   // TODO try Input

} // namespace

