/////////////////////////////////////////////////////////////////////////////
//! \file simlib3D.cc  3D vector block extension
//
// Copyright (c) 1997-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
// 3D extension
//
//  - Integrator3D
//  - blocks for + - * / operations
//


////////////////////////////////////////////////////////////////////////////
// interface
//

#include "simlib.h"
#include "simlib3D.h"
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

double abs(const Value3D &a)
{
    return sqrt( a._x * a._x + a._y * a._y + a._z * a._z );
}

Value3D operator +(const Value3D& a, const Value3D &b)
{
  return Value3D( a._x + b._x,  a._y + b._y,  a._z + b._z );
}

Value3D operator -(const Value3D& a, const Value3D &b)
{
  return Value3D( a._x - b._x,  a._y - b._y,  a._z - b._z );
}

Value3D operator -(const Value3D& a)
{
  return Value3D( -a._x, -a._y, -a._z);
}

Value3D operator *(const Value3D& a, const Value3D &b)
{
  return Value3D( a._y * b._z - a._z * b._y,
                  a._z * b._x - a._x * b._z,
                  a._x * b._y - a._y * b._x
                );
}

Value3D operator *(const Value3D& a, const double b)
{
  return Value3D( a._x * b,  a._y * b,  a._z * b );
}

Value3D operator *(const double a, const Value3D& b)
{
  return b * a;
}

double scalar_product(const Value3D& a, const Value3D &b)
{
    return a._x * b._x + a._y * b._y + a._z * b._z;
}

Value3D operator /(const Value3D& a, const double b)
{
  return Value3D( a._x / b,  a._y / b,  a._z / b );
}


////////////////////////////////////////////////////////////////////////////
// aContiBlock3D --- constructor & destructor
//
aContiBlock3D::aContiBlock3D():
  isEvaluated(false)
{
  Dprintf(("3Dblock-ctr"));
}

aContiBlock3D::~aContiBlock3D() {
  Dprintf(("3Dblock-dtr"));
}

////////////////////////////////////////////////////////////////////////////
// aContiBlock3D::Print --- print of 3D block output value
//
void aContiBlock3D::Print() {
  Value3D a = Value();
  a.Print();
}

////////////////////////////////////////////////////////////////////////////
// aContiBlock::_Eval --- evaluation with algebraic loop detection   ###
//
void aContiBlock3D::_Eval()
{
  if(isEvaluated)                      // was evaluated
    SIMLIB_error(AlgLoopDetected);     // recursive call
  isEvaluated = true;                  // eval-flag
  Eval();                              // evaluation of block
  isEvaluated = false;
}

////////////////////////////////////////////////////////////////////////////
// Parameter3D --- parameter of model
//
Parameter3D &Parameter3D::operator= (const Value3D &x)    {
  if(SIMLIB_Phase==SIMULATION)
    SIMLIB_error(ParameterChangeErr);
  value = x;
  return  *this;
}


////////////////////////////////////////////////////////////////////////////
// aContiBlock1 --- base for blocks with one input
//
aContiBlock3D1::aContiBlock3D1(Input3D i) : input(i)
{
  if(input==this)
    SIMLIB_error(AlgLoopDetected);
//  LoopCheck();
}


////////////////////////////////////////////////////////////////////////////
// Integrator3D::special_input::Value() --- expects 3 subsequent evaluations
//
// method returns scalar value for input of scalar integrator
// three evaluations are guaranteed by Integrator3D implementation
//
double Integrator3D::special_input::Value() {  // interface class
    if(count == 0)
        a = in.Value();         // get vector input
    switch(++count) {
        case 1: return a.x();   // first call (x)
        case 2: return a.y();   // second call (y)
        case 3: count=0;
                return a.z();   // third and last call (z)
    } // switch
    SIMLIB_internal_error();    // *** never reached ***
    return 0;                   // compiler warning elimination
}

////////////////////////////////////////////////////////////////////////////
// Integrator3D --- constructors
//
Integrator3D::Integrator3D(Input3D i, Value3D initial_value):
    _x(in, initial_value.x()),  // linking of inputs to integrators
    _y(in, initial_value.y()),
    _z(in, initial_value.z()),
    in(i) {}

Integrator3D::Integrator3D(Input3D i):
    _x(in), _y(in), _z(in),
    in(i) {}

// --- special copy constructor
Integrator3D::Integrator3D(Integrator3D &i, Value3D initial_value):
    aContiBlock3D(),
    _x(in, initial_value.x()),  // linking of inputs to integrators
    _y(in, initial_value.y()),
    _z(in, initial_value.z()),
    in(i) {}

// zero input for default Integrator3D constructor
// we need it for creating arrays of integrators
static Constant3D Zero3D(0,0,0);

Integrator3D::Integrator3D():
    _x(in), _y(in), _z(in),
    in(Zero3D) {}

////////////////////////////////////////////////////////////////////////////
// Integrator3D output method
//
Value3D Integrator3D::Value()
{
    return Value3D(_x.Value(), _y.Value(), _z.Value());
}

////////////////////////////////////////////////////////////////////////////
// operator =
//
Integrator3D &Integrator3D::operator = (const Value3D &a)
{
    _x=a.x();
    _y=a.y();
    _z=a.z();
    return *this;
}

Integrator3D &Integrator3D::operator = (Input3D i)
{
    Value3D a = i.Value();
    _x=a.x();
    _y=a.y();
    _z=a.z();
    return *this;
}

////////////////////////////////////////////////////////////////////////////
//  aContiBlock2 --- base for 2 input blocks
//
aContiBlock3D2::aContiBlock3D2(Input3D i1, Input3D i2)
  : input1(i1), input2(i2)
{
  if(input1==this || input2==this)
     SIMLIB_error(AlgLoopDetected);
//  LoopCheck();
}

aContiBlock3D3::aContiBlock3D3(Input3D i1, Input3D i2, Input3D i3)
  : aContiBlock3D2(i1,i2), input3(i3)
{
   if(input3==this )
     SIMLIB_error(AlgLoopDetected);
}


////////////////////////////////////////////////////////////////////////////
// Add --- sum of two inputs
//
class _Add3D : public aContiBlock3D2 {
  virtual void _Eval() {}
public:
  _Add3D(Input3D a, Input3D b): aContiBlock3D2(a,b) {
    Dprintf(("ctr: _Add3D[%p](in1,in2)", this));
  }
  virtual Value3D Value() {
    Value3D a = Input1Value();
    Value3D b = Input2Value();
    return a + b;
  }
};


////////////////////////////////////////////////////////////////////////////
// Sub --- subtract two inputs
//
class _Sub3D : public aContiBlock3D2 {
  virtual void _Eval() {}
public:
  _Sub3D(Input3D a, Input3D b): aContiBlock3D2(a,b) {
    Dprintf(("ctr: _Sub3D[%p](in1,in2)", this));
  }
  virtual Value3D Value() {
    Value3D a = Input1Value();
    Value3D b = Input2Value();
    return a - b;
  }
};


////////////////////////////////////////////////////////////////////////////
// Mul --- vector multiplier
//
class _Mul3D : public aContiBlock3D2 {
  virtual void _Eval() {}
public:
  _Mul3D(Input3D a, Input3D b): aContiBlock3D2(a,b) {
    Dprintf(("ctr: _Mul3D[%p](in1,in2)", this));
  }
  virtual Value3D Value() {
    Value3D a = Input1Value();
    Value3D b = Input2Value();
    return a * b;
  }
};

////////////////////////////////////////////////////////////////////////////
// Mul --- multiplier    vector * scalar
//
class _Mul3D1D : public aContiBlock3D1 {
  Input _b;
  virtual void _Eval() {}
public:
  _Mul3D1D(Input3D a, Input b): aContiBlock3D1(a), _b(b) {
    Dprintf(("ctr: _Mul3D1D[%p](in1,in2)", this));
  }
  virtual Value3D Value() {
    Value3D a = InputValue();
    double  b = _b.Value();
    return a * b;
  }
};


////////////////////////////////////////////////////////////////////////////
// Div --- divider   vector / scalar
//
class _Div3D : public aContiBlock3D1 {
  Input _b;
  virtual void _Eval() {}
public:
  _Div3D(Input3D a, Input b): aContiBlock3D1(a), _b(b) {
    Dprintf(("ctr: _Div3D[%p](in1_3D,in2)", this));
  }
  virtual Value3D Value() {
    Value3D a = InputValue();
    double  b = _b.Value();
    return a / b;
  }
};

////////////////////////////////////////////////////////////////////////////
// binary operators ...
//
// wrapper operator functions for block expressions
//

Input3D operator + (Input3D a, Input3D b) { return new _Add3D(a,b); }
Input3D operator - (Input3D a, Input3D b) { return new _Sub3D(a,b); }
Input3D operator * (Input3D a, Input3D b) { return new _Mul3D(a,b); }
Input3D operator * (Input3D a, Input b)   { return new _Mul3D1D(a,b); }
Input3D operator * (Input a, Input3D b)   { return new _Mul3D1D(b,a); }
Input3D operator / (Input3D a, Input b)   { return new _Div3D(a,b); }

////////////////////////////////////////////////////////////////////////////
// UMinus --- unary minus
//
class _UMinus3D: public aContiBlock3D1 {
  virtual void _Eval() {}
public:
  _UMinus3D(Input3D a): aContiBlock3D1(a) {
    Dprintf(("ctr: _UMinus3D[%p](in)", this));
  }
  virtual Value3D Value() {
    Value3D a = InputValue();
    return  -a;
  }
};

////////////////////////////////////////////////////////////////////////////
// unary operators ...
//
Input3D operator - (Input3D a) { return new _UMinus3D(a); }

////////////////////////////////////////////////////////////////////////////
// functions for block expressions
//

////////////////////////////////////////////////////////////////////////////
// _Abs3D --- absolute value of vector x
//
class _Abs3D: public aContiBlock {
  virtual void _Eval() {}
  Input3D in;
public:
  _Abs3D(Input3D a): in(a) {}
  virtual double Value() {
    Value3D a = in.Value();
    return  abs(a);
  }
};

////////////////////////////////////////////////////////////////////////////
// _Norm3D --- unit vector = vector x/Abs(x)
//
class _Norm3D: public aContiBlock3D1 {
  virtual void _Eval() {}
public:
  _Norm3D(Input3D a): aContiBlock3D1(a) {}
  virtual Value3D Value() {
    Value3D a = InputValue();
    return a/abs(a);
  }
};

////////////////////////////////////////////////////////////////////////////
// _ScalarMul3D --- scalar multiply
//
class _ScalarProduct3D: public aContiBlock {
  virtual void _Eval() {}
  Input3D input1;
  Input3D input2;
public:
  _ScalarProduct3D(Input3D a, Input3D b): input1(a), input2(b) {}
  virtual double Value() {
    Value3D a = input1.Value();
    Value3D b = input2.Value();
    return scalar_product(a,b);
  }
};

////////////////////////////////////////////////////////////////////////////
// wrapper functions for block expressions
//

Input Abs(Input3D x) { return new _Abs3D(x); } // absolute value of vector x
Input3D UnitVector(Input3D x) { return new _Norm3D(x); }
Input ScalarProduct(Input3D x, Input3D y) { return new _ScalarProduct3D(x,y); }


////////////////////////////////////////////////////////////////////////////
// _XYZpart --- vector part
//
class _XYZpart: public aContiBlock {
  virtual void _Eval() {}
  Input3D input;
 public:
  enum WhichPart { x,y,z }; // part identification
  _XYZpart(Input3D a, WhichPart w): input(a), which(w) {}
  virtual double Value() {
    Value3D a = input.Value();
    switch(which) {
      case x: return a.x();
      case y: return a.y();
      case z: return a.z();
    }
    SIMLIB_internal_error(); // never reached
    return 0; // compiler warning elimination
  }
 private:
  WhichPart which;
};

////////////////////////////////////////////////////////////////////////////
// get x,y,z part of vector block
//
// wrapper functions for block expressions
//

Input Xpart(Input3D a) { return new _XYZpart(a, _XYZpart::x); }
Input Ypart(Input3D a) { return new _XYZpart(a, _XYZpart::y); }
Input Zpart(Input3D a) { return new _XYZpart(a, _XYZpart::z); }


} // end

