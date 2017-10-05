/////////////////////////////////////////////////////////////////////////////
//! \file opt-param.cc  Optimization parameters
//
// Copyright (c) 2000-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

// EXPERIMENTAL
// parameters for optimization purposes

#include "simlib.h"
#include "internal.h"
#include "optimize.h"
#include <cstring> // strcmp()

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

void Param::Print() const
{
    ::Print("#Parameter %s: value=%g (min=%g, max=%g)\n", name, value, min,
            max);
}

#if 0
Param & Param::operator = (double x) {
    if (x > max)
        x = max;
    if (x < min)
        x = min;
    value = x;
}
#endif

////////////////////////////////////////////////////////////////////////////
//
//
int ParameterVector::search(const char *name)
{
    for (int i = 0; i < n; i++)
        if (std::strcmp(p[i].Name(), name) == 0)
            return i;
    return -1;
}

//  ParameterVector(): n(0), p(0) {}
//  void Add(const Param&p) { add to the end }
//    pp.Add(Param("x",1,5));

// initialize by array
ParameterVector::ParameterVector(int sz, Param * a):
n(sz), p(new Param[n])
{
    for (int i = 0; i < n; i++)
        p[i] = a[i];
}

// copy constructor
ParameterVector::ParameterVector(const ParameterVector & a):n(a.n),
p(new Param[n])
{
    for (int i = 0; i < n; i++)
        p[i] = a[i];
}

// assignment
ParameterVector & ParameterVector::operator = (const ParameterVector & a) {
    if (this == &a)
        return *this;           // a=a
    n = a.n;
    delete[] p;                 // TODO add exception-safety
    p = new Param[n];
    for (int i = 0; i < n; i++)
        p[i] = a[i];            // copy objects
    return *this;
}

ParameterVector::~ParameterVector()
{
    delete[]p;
}

// PROTOTYPE, values only !!!!!!!#########
bool operator == (const ParameterVector & p1, const ParameterVector & p2) {
    int n = p1.size();
    if (n != p2.size())
        return false;
    for (int i = 0; i < n; i++)
        if (p1[i].Value() != p2[i].Value())
            return false;
    return true;
}

// print
void ParameterVector::PrintValues() const
{
    for (int i = 0; i < n; i++)
        ::Print("%g ", p[i].Value());
}

void ParameterVector::Print() const
{
    for (int i = 0; i < n; i++)
        p[i].Print();
}

}
// end
