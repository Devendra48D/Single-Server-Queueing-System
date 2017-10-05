/////////////////////////////////////////////////////////////////////////////
//! \file optimize.h      Basic optimization framework + methods
//
// Copyright (c) 2000-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

// EXPERIMENTAL
// parameters for optimization purposes

#ifndef __SIMLIB_OPTIMIZE_H
#define __SIMLIB_OPTIMIZE_H

namespace simlib3 {

class Param
{
    const char *name;           // name of parameter  c-string
    double min;
    double max;
    double value;
    void limit(double &x)
    {
        if (x > max)
            x = max;
        if (x < min)
            x = min;
    }
  public:
    Param():
        name(0), min(0), max(0), value(min)
    { } // for arrays
    Param (const char *n, double a, double b):
        name(n), min(a), max(b), value(min)
    { }
    void Print() const;
    Param & operator = (double x) {
        value = x;
        limit(value);
        return *this;
    }
    double Value() const { return value; }
    operator double () const { return value; }
    double Min() const { return min; }
    double Max() const { return max; }
    double Range() const { return max - min; }
    const char *Name() const { return name; }
    // TODO: [defaults are O.K.]
    //       Param & operator = (const Param &x);
    //       Param(const Param &x);
};

////////////////////////////////////////////////////////////////////////////
// parameter vector
//
class ParameterVector
{
    int n;
    Param *p;
    int search(const char *name);
  public:
//TODO:  ParameterVector(): n(0), p(0) {}
//TODO:  void Add(const Param&p) { add to the end } //    pp.Add(Param("x",1,5));
    ParameterVector(int sz, Param * a); // initialize by array
    ParameterVector(const ParameterVector & a);
    ParameterVector & operator = (const ParameterVector & a);
   ~ParameterVector();
    Param & operator[] (int i) { return p[i]; }
    const Param & operator[] (int i) const { return p[i]; }
    int size() const { return n; }
    void PrintValues() const;
    void Print() const;
    // Prototypes !!!###
    Param & operator[](const char *name) {
        return p[search(name)];
    }
    friend bool operator == (const ParameterVector & p1,
                             const ParameterVector & p2);
};

// Type of function to optimize
typedef double (*opt_function_t) (const ParameterVector & p);

// Predefined optimization methods
double Optimize_hooke(opt_function_t f, ParameterVector & p,
                      double rho, double epsilon, int itermax);

double Optimize_simann(opt_function_t f, ParameterVector & p, int MAXT);

double Optimize_gradient(opt_function_t f, ParameterVector & p,
                         double MAXITER);

}

#endif // __SIMLIB_OPTIMIZE_H
