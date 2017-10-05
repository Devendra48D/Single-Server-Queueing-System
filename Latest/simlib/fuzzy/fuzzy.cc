/////////////////////////////////////////////////////////////////////////////
// fuzzy.cc
//
// SIMLIB version: 2.16.2
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
// Implementation of fuzzy set and fuzzy variable.
/////////////////////////////////////////////////////////////////////////////

#include "simlib.h"
#include "fuzzy.h"
#include "internal.h"

#include <cstdio>
#include <cassert>
#include <cmath>

#include <string.h>

namespace simlib3 {

/**
 * Creates fuzzy set.
 * @param min Minimal value of universum.<br>Spodní mez univerza.
 * @param max Maximal value of universum.<br>Horní mez univerza.
 */
FuzzySet::FuzzySet(const char * name, double min, double max)
 : n(0), xmin(min), xmax(max) 
{ 
  Name = strdup(name);
}

/**
 * Creates fuzzy set.
 * @param min Minimal value of universum.<br>Spodní mez univerza.
 * @param max Maximal value of universum.<br>Horní mez univerza.
 */
FuzzySet::FuzzySet(const char * name, double min, double max,
    const FuzzyMembershipFunction &m1)
 : n(0), xmin(min), xmax(max) 
{
  Name = strdup(name);
  add(m1);
}

/**
 * Creates fuzzy set.
 * @param min Minimal value of universum.<br>Spodní mez univerza.
 * @param max Maximal value of universum.<br>Horní mez univerza.
 */
FuzzySet::FuzzySet(const char * name, double min, double max,
             const FuzzyMembershipFunction &m1,
	     const FuzzyMembershipFunction &m2)
 : n(0), xmin(min), xmax(max) 
{
  Name = strdup(name);
  add(m1);
  add(m2);
}

/**
 * Creates fuzzy set.
 * @param min Minimal value of universum.<br>Spodní mez univerza.
 * @param max Maximal value of universum.<br>Horní mez univerza.
 */
FuzzySet::FuzzySet(const char * name, double min, double max,
             const FuzzyMembershipFunction &m1,
	     const FuzzyMembershipFunction &m2,
	     const FuzzyMembershipFunction &m3)
 : n(0), xmin(min), xmax(max) 
{
  Name = strdup(name);
  add(m1);
  add(m2);
  add(m3);
}

/**
 * Creates fuzzy set.
 * @param min Minimal value of universum.<br>Spodní mez univerza.
 * @param max Maximal value of universum.<br>Horní mez univerza.
 */
FuzzySet::FuzzySet(const char * name, double min, double max,
             const FuzzyMembershipFunction &m1,
	     const FuzzyMembershipFunction &m2,
	     const FuzzyMembershipFunction &m3,
	     const FuzzyMembershipFunction &m4)
 : n(0), xmin(min), xmax(max) 
{
  Name = strdup(name);
  add(m1);
  add(m2);
  add(m3);
  add(m4);
}

/**
 * Creates fuzzy set.
 * @param min Minimal value of universum.<br>Spodní mez univerza.
 * @param max Maximal value of universum.<br>Horní mez univerza.
 */
FuzzySet::FuzzySet(const char * name, double min, double max,
             const FuzzyMembershipFunction &m1,
	     const FuzzyMembershipFunction &m2,
	     const FuzzyMembershipFunction &m3,
	     const FuzzyMembershipFunction &m4,
	     const FuzzyMembershipFunction &m5)
 : n(0), xmin(min), xmax(max) 
{
  Name = strdup(name);
  add(m1);
  add(m2);
  add(m3);
  add(m4);
  add(m5);
}

/**
 * It adds next word value into the universum.<br>Pøidá dal¹í slovní hodnotu do univerza.
 * @param x Membership function representing word value.<br>Funkce pøíslu¹nosti reprezentující slovní hodnotu.
 */
void FuzzySet::add(const FuzzyMembershipFunction &x) 
{
  TRACE(printf("FuzzySet::add(%s)\n", x.wordValue()));
  if (n >= MAX) SIMLIB_error("FuzzySet limit exceeded");
  array[n++] = x.clone();
}

/** It duplicates object.<br>Duplikuje objekt. */
// implemented in fuzzy.cc
FuzzySet *FuzzySet::clone() const
{
  FuzzySet * result = new FuzzySet(*this);
  result->Name = strdup(Name);
  for (unsigned i = 0; i < n; i++)
  {
    result->array[i] = array[i]->clone();
  }
  return result;
}


/** It selects i-th member function.<br>Vybere i-tou funkci pøíslu¹nosti. */
const FuzzyMembershipFunction *FuzzySet::operator[] (int i) const 
{
  if (unsigned(i) >= n)
  	SIMLIB_error("FuzzySet[]: index out of range");
  return array[i];
}

/** It computes i-th function value (membership).<br>Vypoète i-tou funkèní hodnotu (pøíslu¹nost). */
double FuzzySet::Membership(int i, double x) const 
{
  if (unsigned(i) >= n)
    SIMLIB_error("FuzzySet::Membership: index out of range");
  return array[i]->Membership(x);
}

/** Destructor removes all membership functions.<br>Destruktor uvolní v¹echny funkce pøíslu¹nosti. */
FuzzySet::~FuzzySet() 
{
  TRACE(printf("~FuzzySet()\n"));
  for (unsigned i = 0; i < n; i++)
	  delete array[i]; // clean memory
  delete [] Name;
}

//// info functions

/** Minimum of maxims.<br>Minimum z maxim. */
double FuzzySet::min1() 
{ 
  double min;
  assert(count()>0);
  min = array[0]->min1();
  for(int i=1; i<count(); i++) 
  {
    double m = array[i]->min1(); 
    if(m<min) min = m;
  }
  return min;
}

/** Maximum of maxims.<br>Maximum z maxim. */
double FuzzySet::max1() 
{ 
  double max;
  assert(count()>0);
  max = array[0]->max1();
  for(int i=1; i<count(); i++) 
  {
    double m = array[i]->max1(); 
    if(m<max) max = m;
  }
  return max;
}


extern FuzzyBlock *activeFuzzyBlock;

/////////////////////////////////////////////////////////////////////////////
// fuzzy set
//
// TODO: add refcount to setType
/**
 * It connects fuzzy set with variable.<br>Spojí fuzzy mno¾inu s fuzzy promìnnou.
 * @param t Fuzzy set.<br>Fuzzy mno¾ina.
 */
FuzzyVariable::FuzzyVariable(const FuzzySet &t) : n(t.count()) 
{
  TRACE(printf("FuzzyVariable::FuzzyVariable()\n"));
  assert(n > 0);
  m = t.clone();
  mval = new double[n];
  // create empty/zero Fuzzy set ???
  for(unsigned i = 0; i < n; i++) mval[i] = 0.0F;

  if (activeFuzzyBlock != 0)
    registerOwner(activeFuzzyBlock);
  // jinak se bude registrovat pozdeji explicitne.  
/*
  where = activeFuzzyBlock;
  if(where==0) SIMLIB_error("Fuzzy set should be used inside FuzzyBlock only");
  where->Register(this); // register this object in list of block contents
*/    
}

/**
 * It registers owner of this variable.<br>Zaregistruje vlastníka této promìnné.
 * @param owner Owner of this variable.<br>Vlastník této promìnné.
 */
void FuzzyVariable::registerOwner(FuzzyBlock *owner)
{
  where = owner;
  where->Register(this);
}

/** I-th member function.<br>I-tá funkce pøíslu¹nosti. */
// get membership function
const FuzzyMembershipFunction *FuzzyVariable::mf(int i) const 
{
  if(unsigned(i)>=n) 
    SIMLIB_error("FuzzyVariable::mf(i) index out of range");
  return (*m)[i];
}

/** It gets center of i-th member function.<br>Vrací støed i-té funkce pøíslu¹nosti. */
double FuzzyVariable::center(int i) const 
{
  return mf(i)->center();
}

/** It gets i-th word value.<br>Vrací i-tou slovní hondotu. */
const char *FuzzyVariable::wordValue(int i) const 
{
  return mf(i)->wordValue();
}

/** Get/set fuzzy value.<br>Vra»/nastav fuzzy hodnotu. */
double &FuzzyVariable::operator[] (int i) const 
{
  if(unsigned(i)>=n) 
    SIMLIB_error("FuzzyVariable::[i] index out of range");
  return mval[i];
}

// print fuzzy set contents
void FuzzyVariable::Print() 
{
  printf("%s : {", m->name());
  for (unsigned i = 0; i < n; i++) 
  {
    if(mval[i]<0.0||mval[i]>1.0) printf(" ***");
    if (mval[i] != 0) printf(" %s=%5.3f%c", wordValue(i), mval[i], (i<n-1)?',':' ');
  }
  printf("}\n");
}

/** Search by member name.<br>Hledá podle jména. */
unsigned FuzzyVariable::search(const char *s) const 
{
  for (unsigned i = 0; i < n; i++)
    if (strcmp(s, (*m)[i]->wordValue()) == 0) return i;
  SIMLIB_error("FuzzyVariable::search(name) bad name '%s'",s); // no return
  return 0;
}

// compute and set membership of i-th member
double FuzzyVariable::SetMembership(int i, double x) 
{
  return mval[i] = m->Membership(i,x);
}

} // namespace

// end
