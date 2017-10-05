/////////////////////////////////////////////////////////////////////////////
//! \file  simlib2D.cc   2D vector block extension
//
// Copyright (c) 1997-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
// 2D extension
//
//  - Integrator2D
//  - blocks for + - * / operations
//


////////////////////////////////////////////////////////////////////////////
// interface
//

#include "simlib.h"
#include "simlib2D.h"
#include "internal.h"
#include <cmath>

////////////////////////////////////////////////////////////////////////////
// implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

////////////////////////////////////////////////////////////////////////////
// non-block operations
//

double abs(const Value2D &a)
{
    return sqrt( a._x * a._x + a._y * a._y );
}

Value2D operator +(const Value2D& a, const Value2D &b)
{
  return Value2D( a._x + b._x,  a._y + b._y );
}

Value2D operator -(const Value2D& a, const Value2D &b)
{
  return Value2D( a._x - b._x,  a._y - b._y );
}

Value2D operator -(const Value2D& a)
{
  return Value2D( -a._x, -a._y);
}

/*
Value2D operator *(const Value2D& a, const Value2D &b)
{
  return Value2D( a._y * b._z - a._z * b._y,
                  a._z * b._x - a._x * b._z,
                  a._x * b._y - a._y * b._x
                );
}
*/

Value2D operator *(const Value2D& a, const double b)
{
  return Value2D( a._x * b,  a._y * b );
}

Value2D operator *(const double a, const Value2D& b)
{
  return b * a;
}

double scalar_product(const Value2D& a, const Value2D &b)
{
    return a._x * b._x + a._y * b._y ;
}

Value2D operator /(const Value2D& a, const double b)
{
  return Value2D( a._x / b,  a._y / b );
}


////////////////////////////////////////////////////////////////////////////
// aContiBlock2D --- constructor & destructor
//
aContiBlock2D::aContiBlock2D():
  isEvaluated(false)
{
  Dprintf(("2Dblock-ctr"));
}

aContiBlock2D::~aContiBlock2D() {
  Dprintf(("2Dblock-dtr"));
}

////////////////////////////////////////////////////////////////////////////
// aContiBlock2D::Print --- print of 2D block output value
//
void aContiBlock2D::Print() {
  Value2D a = Value();
  a.Print();
}

////////////////////////////////////////////////////////////////////////////
// aContiBlock::_Eval --- evaluation with algebraic loop detection   ###
//
void aContiBlock2D::_Eval()
{
  if(isEvaluated)                      // was evaluated
    SIMLIB_error(AlgLoopDetected);     // recursive call
  isEvaluated = true;                  // eval-flag
  Eval();                              // evaluation of block
  isEvaluated = false;
}

////////////////////////////////////////////////////////////////////////////
// Parameter2D --- parameter of model
//
Parameter2D &Parameter2D::operator= (const Value2D &x)    {
  if(SIMLIB_Phase==SIMULATION)
    SIMLIB_error(ParameterChangeErr);
  value = x;
  return *this;
}


////////////////////////////////////////////////////////////////////////////
// aContiBlock1 --- base for blocks with one input
//
aContiBlock2D1::aContiBlock2D1(Input2D i) : input(i)
{
  if(input==this)
    SIMLIB_error(AlgLoopDetected);
//  LoopCheck();
}


////////////////////////////////////////////////////////////////////////////
// Integrator2D::special_input::Value() --- expects 2 subsequent evaluations
//
// return scalar value for scalar integrator
// two evaluations are guaranteed by Integrator2D
//
double Integrator2D::special_input::Value() {  // interface class
    if(count == 0)
        a = in.Value();         // get vector input (x,y)
    switch(++count) {
        case 1: return a.x();   // first call (x)
        case 2: count=0;        // second call (y)
                return a.y();
    } // switch
    SIMLIB_internal_error();    // *** never reached ***
    return 0;                   // compiler warning elimination
}

////////////////////////////////////////////////////////////////////////////
// Integrator2D --- constructors
//
Integrator2D::Integrator2D(Input2D i, Value2D initial_value):
    _x(in, initial_value.x()),  // linking of inputs to integrators
    _y(in, initial_value.y()),
    in(i) {}

Integrator2D::Integrator2D(Input2D i):
    _x(in), _y(in),
    in(i) {}

// --- special copy constructor
Integrator2D::Integrator2D(Integrator2D &i, Value2D initial_value):
    aContiBlock2D(),
    _x(in, initial_value.x()),  // linking of inputs to integrators
    _y(in, initial_value.y()),
    in(i) {}

// zero input for default Integrator2D constructor
// we need it for creating arrays of integrators
static Constant2D Zero2D(0,0);

Integrator2D::Integrator2D():
    _x(in), _y(in),
    in(Zero2D) {}


////////////////////////////////////////////////////////////////////////////
// operator =  --- assign new value
//
Integrator2D &Integrator2D::operator = (const Value2D &a)
{
    _x=a.x();
    _y=a.y();
    return *this;
}

Integrator2D &Integrator2D::operator = (Input2D i)
{
    Value2D a = i.Value();
    _x=a.x();
    _y=a.y();
    return *this;
}


////////////////////////////////////////////////////////////////////////////
// Integrator2D output method
//
Value2D Integrator2D::Value()
{
    return Value2D(_x.Value(), _y.Value());
}

////////////////////////////////////////////////////////////////////////////
//  aContiBlock2 --- base for 2 input blocks
//
aContiBlock2D2::aContiBlock2D2(Input2D i1, Input2D i2)
  : input1(i1), input2(i2)
{
  if(input1==this || input2==this)
     SIMLIB_error(AlgLoopDetected);
//  LoopCheck();
}

aContiBlock2D3::aContiBlock2D3(Input2D i1, Input2D i2, Input2D i3)
  : aContiBlock2D2(i1,i2), input3(i3)
{
   if(input3==this )
     SIMLIB_error(AlgLoopDetected);
}


////////////////////////////////////////////////////////////////////////////
// Add --- sum of two inputs
//
class _Add2D : public aContiBlock2D2 {
  virtual void _Eval() {}
public:
  _Add2D(Input2D a, Input2D b): aContiBlock2D2(a,b) {
    Dprintf(("ctr: _Add2D[%p](in1,in2)", this));
  }
  virtual Value2D Value() {
    Value2D a = Input1Value();
    Value2D b = Input2Value();
    return a + b;
  }
};


////////////////////////////////////////////////////////////////////////////
// Sub --- subtract two inputs
//
class _Sub2D : public aContiBlock2D2 {
  virtual void _Eval() {}
public:
  _Sub2D(Input2D a, Input2D b): aContiBlock2D2(a,b) {
    Dprintf(("ctr: _Sub2D[%p](in1,in2)", this));
  }
  virtual Value2D Value() {
    Value2D a = Input1Value();
    Value2D b = Input2Value();
    return a - b;
  }
};

////////////////////////////////////////////////////////////////////////////
// Mul --- multiplier    vector * scalar
//
class _Mul2D1D : public aContiBlock2D1 {
  Input _b;
  virtual void _Eval() {}
public:
  _Mul2D1D(Input2D a, Input b): aContiBlock2D1(a), _b(b) {
    Dprintf(("ctr: _Mul2D1D[%p](in1,in2)", this));
  }
  virtual Value2D Value() {
    Value2D a = InputValue();
    double  b = _b.Value();
    return a * b;
  }
};


////////////////////////////////////////////////////////////////////////////
// Div --- divider   vector / scalar
//
class _Div2D : public aContiBlock2D1 {
  Input _b;
  virtual void _Eval() {}
public:
  _Div2D(Input2D a, Input b): aContiBlock2D1(a), _b(b) {
    Dprintf(("ctr: _Div2D[%p](in1_2D,in2)", this));
  }
  virtual Value2D Value() {
    Value2D a = InputValue();
    double  b = _b.Value();
    return a / b;
  }
};

////////////////////////////////////////////////////////////////////////////
// binary operators ...
//
// wrapper operator functions for block expressions
//

Input2D operator + (Input2D a, Input2D b) { return new _Add2D(a,b); }
Input2D operator - (Input2D a, Input2D b) { return new _Sub2D(a,b); }
Input2D operator * (Input2D a, Input b)   { return new _Mul2D1D(a,b); }
Input2D operator * (Input a, Input2D b)   { return new _Mul2D1D(b,a); }
Input2D operator / (Input2D a, Input b)   { return new _Div2D(a,b); }

////////////////////////////////////////////////////////////////////////////
// UMinus --- unary minus
//
class _UMinus2D: public aContiBlock2D1 {
  virtual void _Eval() {}
public:
  _UMinus2D(Input2D a): aContiBlock2D1(a) {
    Dprintf(("ctr: _UMinus2D[%p](in)", this));
  }
  virtual Value2D Value() {
    Value2D a = InputValue();
    return  -a;
  }
};

////////////////////////////////////////////////////////////////////////////
// unary operators ...
//
Input2D operator - (Input2D a) { return new _UMinus2D(a); }

////////////////////////////////////////////////////////////////////////////
// functions for block expressions
//

////////////////////////////////////////////////////////////////////////////
// _Abs2D --- absolute value of vector x
//
class _Abs2D: public aContiBlock {
  virtual void _Eval() {}
  Input2D in;
public:
  _Abs2D(Input2D a): in(a) {}
  virtual double Value() {
    Value2D a = in.Value();
    return  abs(a);
  }
};

////////////////////////////////////////////////////////////////////////////
// _Norm2D --- unit vector = vector x/Abs(x)
//
class _Norm2D: public aContiBlock2D1 {
  virtual void _Eval() {}
public:
  _Norm2D(Input2D a): aContiBlock2D1(a) {}
  virtual Value2D Value() {
    Value2D a = InputValue();
    return a/abs(a);
  }
};

////////////////////////////////////////////////////////////////////////////
// _ScalarProduct2D --- scalar multiply
//
class _ScalarProduct2D: public aContiBlock {
  virtual void _Eval() {}
  Input2D input1;
  Input2D input2;
public:
  _ScalarProduct2D(Input2D a, Input2D b): input1(a), input2(b) {}
  virtual double Value() {
    Value2D a = input1.Value();
    Value2D b = input2.Value();
    return scalar_product(a,b);
  }
};

////////////////////////////////////////////////////////////////////////////
// wrapper functions for block expressions
//

Input Abs(Input2D x) { return new _Abs2D(x); } // absolute value of vector x
Input2D UnitVector(Input2D x) { return new _Norm2D(x); }
Input ScalarProduct(Input2D x, Input2D y) { return new _ScalarProduct2D(x,y); }


////////////////////////////////////////////////////////////////////////////
// _XYpart --- vector part
//
class _XYpart: public aContiBlock {
  virtual void _Eval() {}
  Input2D input;
 public:
  enum WhichPart { x,y }; // part identification
  _XYpart(Input2D a, WhichPart w): input(a), which(w) {}
  virtual double Value() {
    Value2D a = input.Value();
    switch(which) {
      case x: return a.x();
      case y: return a.y();
    }
    SIMLIB_internal_error();    // *** never reached ***
    return 0;                   // compiler warning elimination
  }
 private:
  WhichPart which;
};

////////////////////////////////////////////////////////////////////////////
// get x,y part of vector block
//
// wrapper functions for block expressions
//

Input Xpart(Input2D a) { return new _XYpart(a, _XYpart::x); }
Input Ypart(Input2D a) { return new _XYpart(a, _XYpart::y); }

} // end

