/////////////////////////////////////////////////////////////////////////////
//! \file  simlib2D.h   2D vector block extension
//! \defgroup simlib2D  SIMLIB/C++ 2D extension
//
// Copyright (c) 1997-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  the SIMLIB/2D extension
//
// Warning: beta version -- this is the prototype
//

#ifndef __SIMLIB2D_H
#define __SIMLIB2D_H

#ifndef __SIMLIB__
#   error "simlib2D.h: 20: you must include simlib.h first"
#endif
#if __SIMLIB__ < 0x0212
#   error "simlib2D.h: 23: requires SIMLIB version 2.12 and higher"
#endif

namespace simlib3 {

////////////////////////////////////////////////////////////////////////////
//! 2D vector value
//! \ingroup simlib2D
class Value2D {
  double _x, _y;
 public:
  Value2D(double x, double y) : _x(x), _y(y) {}
  double x() const { return _x; }
  double y() const { return _y; }

  // using default copy constructor, destructor, and operator =

  Value2D operator + (Value2D b) { return Value2D(_x+b._x, _y+b._y); }
  Value2D operator - (Value2D b) { return Value2D(_x-b._x, _y-b._y); }
  void Print() { ::Print(" %g %g ", _x, _y); }

  // utilities:
  friend double abs(const Value2D &a);
  friend Value2D operator +(const Value2D& a, const Value2D &b);
  friend Value2D operator -(const Value2D& a, const Value2D &b);
  friend Value2D operator -(const Value2D& a);
  friend Value2D operator *(const Value2D& a, const double b);
  friend Value2D operator *(const double a, const Value2D& b);
  friend double scalar_product(const Value2D& a, const Value2D &b);
  friend Value2D operator /(const Value2D& a, const double b);

};


////////////////////////////////////////////////////////////////////////////
//! Abstract 2D block with single 2D output
//! \ingroup simlib2D
class aContiBlock2D : public aBlock {  // abstract continuous block
  aContiBlock2D(const aContiBlock2D&); // disable copy ctr
  void operator= (aContiBlock2D&);     // disable assignment
 protected:
  bool isEvaluated;                  // flag for loop checking ###---
 private:
  virtual void Eval() {};            // evaluate without loop detection
 public:
  aContiBlock2D();
  ~aContiBlock2D();
  virtual void _Eval();              // evaluate with loop detection
  virtual Value2D Value() = 0;       // output value
  void Print();                      // print value
};

////////////////////////////////////////////////////////////////////////////
//! 2D vector value that can't be changed
//! \ingroup simlib2D
class Constant2D : public aContiBlock2D {
  const Value2D value;
 public:
  explicit Constant2D(Value2D x) : value(x) {}
  explicit Constant2D(double x, double y) : value(x,y) {}
  virtual Value2D Value()  { return value; }      // without Eval
};

////////////////////////////////////////////////////////////////////////////
//! 2D vector variable
//! \ingroup simlib2D
class Variable2D : public aContiBlock2D {
 protected:
  Value2D value;
 public:
  explicit Variable2D(Value2D x=Value2D(0,0)) : value(x) {}
  Variable2D &operator= (Value2D x)  { value = x; return *this; }
  // TODO: operator=
  virtual Value2D Value()            { return value; }
};

////////////////////////////////////////////////////////////////////////////
//! Special variable (can't be changed at simulation time)
//! \ingroup simlib2D
class Parameter2D : public Variable2D {
 public:
  explicit Parameter2D(Value2D x=Value2D(0,0)) : Variable2D(x) {}
  Parameter2D &operator= (const Value2D &x);
};

////////////////////////////////////////////////////////////////////////////
//! Continuous block connection (transparent reference)
//! \ingroup simlib2D
class Input2D {                   // small objects, without virtual methods
  aContiBlock2D *bp;
 public:
  Input2D&operator=(const Input2D&x) { bp=x.bp; return *this; }
  Input2D(const Input2D &in): bp(in.bp) {}       // copy reference
  Input2D(aContiBlock2D &cb): bp(&cb) {}         // reference to 2D block
  Input2D(aContiBlock2D *cb): bp(cb)  {}         // pointer to 2D block
  Input2D Set(Input2D i)    { Input2D p=bp; bp=i.bp; return p; }
  Value2D Value()           { return bp->Value(); }   // get value
  bool operator ==(void *p) { return bp==p; }         // for tests only!
};

////////////////////////////////////////////////////////////////////////////
//! continuous block with single input and alg. loop check
//! \ingroup simlib2D
class aContiBlock2D1 : public aContiBlock2D {
  Input2D input;
 public:
  explicit aContiBlock2D1(Input2D i);
  Value2D InputValue() { return input.Value(); }
  Input2D SetInput(Input2D i) { return input.Set(i); }
};

////////////////////////////////////////////////////////////////////////////
//! Reference to 2D vector block-expression
//! \ingroup simlib2D
struct Expression2D : public aContiBlock2D1 {
  explicit Expression2D(Input2D i) : aContiBlock2D1(i) {}
  Value2D Value() { return InputValue(); }
};

////////////////////////////////////////////////////////////////////////////
//! continuous block vith 2 inputs and alg. loop check
//! \ingroup simlib2D
class aContiBlock2D2 : public aContiBlock2D {
  Input2D input1;
  Input2D input2;
 public:
  aContiBlock2D2(Input2D i1, Input2D i2);
  Value2D Input1Value() { return input1.Value(); }
  Value2D Input2Value() { return input2.Value(); }
};

////////////////////////////////////////////////////////////////////////////
//! continuous block with 3 inputs and alg. loop check
//! \ingroup simlib2D
class aContiBlock2D3 : public aContiBlock2D2 {
  Input2D input3;
 public:
  aContiBlock2D3(Input2D i1, Input2D i2, Input2D i3);
  Value2D Input3Value() { return input3.Value(); }
};


////////////////////////////////////////////////////////////////////////////
//! Convert 2 scalar inputs to single 2D vector output
//! \ingroup simlib2D
class Adaptor2D : public aContiBlock2D {
  Input x, y;
 public:
  Adaptor2D(Input _x, Input _y) : x(_x), y(_y) {}
  virtual Value2D Value() {
    return Value2D( x.Value(), y.Value() );
  }
};


////////////////////////////////////////////////////////////////////////////
//! 2D vector integrator
//! \ingroup simlib2D
class Integrator2D : public aContiBlock2D {
  Integrator _x,_y;          // 1D standard integrators

  // special_input transforms 2D values into 2 scalar values
  class special_input : public aContiBlock {
    Value2D a;                  // temporary value
    Input2D in;                 // 2D input
    int count;                  // # of evaluations
   public:
    special_input(Input2D i) : a(0,0), in(i), count(0) {}
    double Value();             // expects 2 subsequent evaluations
    friend class Integrator2D;
  } in;

 public:
  Integrator2D(Input2D i, Value2D initial_value);
  Integrator2D(Input2D i);
  Integrator2D(Integrator2D &i, Value2D initial_value=Value2D(0,0));
  Integrator2D();       // for arrays: implicit input value (0,0)

  Input2D SetInput(Input2D i) { return in.in.Set(i); }

  Integrator2D &operator = (const Value2D &a);
  Integrator2D &operator = (Input2D i);


  virtual Value2D Value();      // 2D output
};


////////////////////////////////////////////////////////////////////////////
// Continuous block arithmetic operators
//

// binary operators:
Input2D operator + (Input2D a, Input2D b);      // add vectors
Input2D operator - (Input2D a, Input2D b);      // subtract vectors
Input2D operator * (Input2D a, Input b);        // vector * scalar
Input2D operator * (Input a, Input2D b);        // scalar * vector
Input2D operator / (Input2D a, Input b);        // vector / scalar

// unary operators:
Input2D operator - (Input2D a);                 // unary -

//! absolute value of vector  (1D block)
Input Abs(Input2D x);

//! make unit vector from input (Abs(output_vec)==1)
Input2D UnitVector(Input2D v);

//! dot product:  xvec . yvec
Input ScalarProduct(Input2D x, Input2D y);

//! get x part of (x,y) vector value
Input Xpart(Input2D a);

//! get y part of (x,y) vector value
Input Ypart(Input2D a);

inline void Print(Value2D a) { a.Print(); }

} // namespace

#endif // __SIMLIB2D_H
