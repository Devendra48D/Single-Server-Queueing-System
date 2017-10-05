// cz - pøíli¹ ¾lu»ouèký kùò úpìl ïábelské ódy
/////////////////////////////////////////////////////////////////////////////
// fuzzymf.cc
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
// Implementation of fuzzy membership functions.
/////////////////////////////////////////////////////////////////////////////

#include "simlib.h"
#include "fuzzy.h"
#include "internal.h"

#include <cstdio>
#include <cassert>
#include <cmath>

#include <string.h>

//inline double min(double x, double y) { return (x < y) ? x : y; }
//inline double max(double x, double y) { return (x > y) ? x : y; }

#if 0
   static int trace = 1;
#  define TRACE(x)  do{if(trace) x;}while(0)
#else
#  define TRACE(x)
#endif

namespace simlib3 {

/////////////////////////////////////////////////////////////////////////////
// general membership function
/////////////////////////////////////////////////////////////////////////////
/**
 * It assigns word value to the membership function. This does not copy poiner of parameter
 * name, but it cretes copy of memory.<br>
 * Pøiøadí slovní hodnotu funkci pøíslu¹nosti. Nekopíruje ukazatel parametru name, ale vytváøí
 * kopii pamìti.
 * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
 */
FuzzyMembershipFunction::FuzzyMembershipFunction(const char *name)
: Name(NULL), defValues(0) 
{
  Name = strdup(name);
}

/**
 * Copy constructor. It creates copy of all member variables. For poiners creates copy of 
 * memory block.<br>
 * Kopy konstruktor. Vytváøí kopii v¹ech èlenských promìnných. Pro ukazatele vytvoøí kopii
 * pamìti.
 * @param duplicate Object that will be duplicated.<br>Objekt, který bude duplikován.
 */
FuzzyMembershipFunction::FuzzyMembershipFunction(const FuzzyMembershipFunction &duplicate)
: Name(NULL), defValues(duplicate.defValues) 
{
  Name = strdup(duplicate.Name);
}


/** Destructor */
FuzzyMembershipFunction::~FuzzyMembershipFunction()
{
  TRACE(printf("~FuzzyMemberShipFunction\n")); 
  delete [] Name;
}


/** Print of membership table.<br>Tisk tabulky pøíslu¹nosti. */
// debugging tool:
void FuzzyMembershipFunction::Print(double a, double b) const 
{
  double step=(b-a)/100;
  for(double i=a; i<=b; i+=step)
    //cout << i << ' ' << Membership(i) << endl;
    printf("%g %g\n", i, Membership(i) );
}

/////////////////////////////////////////////////////////////////////////////
// special membership functions:
/////////////////////////////////////////////////////////////////////////////
// singleton
/**
 * It assigns word value to the membership function and sets definition value.<br>
 * Pøiøadí slovní hodnotu funkci pøíslu¹nosti a nastaví definièní hodnotu.
 * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
 * @param a A definition value.<br>Definièní hodnota.
 */
FuzzySingleton::FuzzySingleton(const char *name, double a):
        FuzzyMembershipFunction(name), x0(a)
{
  defValues = 1;
  TRACE(printf("FuzzySingleton::FuzzySingleton(%s)\n",name));
}

/**
 * It assigns word value to the membership function.<br>
 * Pøiøadí slovní hodnotu funkci pøíslu¹nosti.
 * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
 */
FuzzySingleton::FuzzySingleton(const char *name)
: FuzzyMembershipFunction(name)
{
  TRACE(printf("FuzzySingleton::FuzzySingleton(%s)\n",name));
}

/** It computes function value (membership).<br>Vypoète funkèní hodnotu (pøíslu¹nost). */
double FuzzySingleton::Membership(double x) const 
{
  TRACE(printf("FuzzySingleton::Membership(%g)\n",x));
  if (x == x0) return 1; 
  return 0;
}

/** It duplicates object.<br>Duplikuje objekt. */
FuzzySingleton *FuzzySingleton::clone() const
{
  return new FuzzySingleton(*this);
}

/** This adds next definition value.<br>Pøidá dal¹í definièní hodnotu. */
void FuzzySingleton::addDefValue(double value)
{
  if (isComplete()) SIMLIB_error("Bad singleton membership function definition.");
  defValues = 1;
  x0 = value;
}

/////////////////////////////////////////////////////////////////////////////
// triangle
/**
 * It assigns word value to the membership function and sets definition values.<br>
 * Pøiøadí slovní hodnotu funkci pøíslu¹nosti a nastaví definièní hodnotu.
 * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
 * @param a A left vertex.<br>Levý vrchol.
 * @param b A center vertex.<br>Prostøední vrchol.
 * @param c A right vertex.<br>Pravý vrchol.
 */
FuzzyTriangle::FuzzyTriangle(const char *name, double a, double b, double c)
: FuzzyMembershipFunction(name), x0(a), x1(b), x2(c) 
{
  defValues = 3;
  TRACE(printf("FuzzyTriangle::FuzzyTriangle(%s)\n",name));
  if (a > b || b > c /*|| a == c*/)
    SIMLIB_error("Bad triangle membership function definition. Too much definition values.");
}

/**
 * It assigns word value to the membership function.<br>
 * Pøiøadí slovní hodnotu funkci pøíslu¹nosti.
 * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
 */
FuzzyTriangle::FuzzyTriangle(const char* name)
: FuzzyMembershipFunction(name)
{
  TRACE(printf("FuzzyTriangle::FuzzyTriangle(%s)\n",name));
}

/** It computes function value (membership).<br>Vypoète funkèní hodnotu (pøíslu¹nost). */
double FuzzyTriangle::Membership(double x) const 
{
  TRACE(printf("FuzzyTriangle::Membership(%g)\n",x));
  if (x == x1) return 1; // problem if x0==x1 || x1==x2
  if (x > x0 && x <= x1)  return (x - x0) / (x1 - x0);
  if (x > x1 && x <= x2)  return (x2 - x) / (x2 - x1);
  return 0;
}

/** It duplicates object.<br>Duplikuje objekt. */
FuzzyTriangle *FuzzyTriangle::clone() const 
{
  return new FuzzyTriangle(*this);
}

/** This adds next definition value.<br>Pøidá dal¹í definièní hodnotu. */
void FuzzyTriangle::addDefValue(double value)
{
  if (isComplete())
    SIMLIB_error("Bad triangle membership function definition. Too much definition values.");
  switch (defValues++)
  {
    case 0:
      x0 = value;
      break;
    case 1:
      x1 = value;
      if (x1 < x0) SIMLIB_error("Bad triangle membership function definition.");
      break;
    case 2:
      x2 = value;
      if (x2 < x1) SIMLIB_error("Bad triangle membership function definition.");
      break;
    default: break;
  }
}


/////////////////////////////////////////////////////////////////////////////
// trapez
//
/**
 * It assigns word value to the membership function and sets definition values.<br>
 * Pøiøadí slovní hodnotu funkci pøíslu¹nosti a nastaví definièní hodnotu.
 * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
 * @param a A left down vertex.<br>Levý spodní vrchol.
 * @param b A left top vertex.<br>Levý horní vrchol.
 * @param c A right top vertex.<br>Pravý horní vrchol.
 * @param d A right down vertex.<br>Pravý spodní vrchol.
 */
FuzzyTrapez::FuzzyTrapez(const char *name, double a, double b, double c, double d)
: FuzzyMembershipFunction(name), x0(a), x1(b), x2(c), x3(d) 
{
  defValues = 4;
  TRACE(printf("FuzzyTrapez::FuzzyTrapez(%s)\n",name));
  if (a > b || b > c || c > d )
    SIMLIB_error("Bad trapez membership function definition.");
}

/**
 * It assigns word value to the membership function.<br>
 * Pøiøadí slovní hodnotu funkci pøíslu¹nosti.
 * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
 */
FuzzyTrapez::FuzzyTrapez(const char* name)
: FuzzyMembershipFunction(name)
{
  TRACE(printf("FuzzyTrapez::FuzzyTrapez(%s)\n",name));
}

/** It computes function value (membership).<br>Vypoète funkèní hodnotu (pøíslu¹nost). */
double FuzzyTrapez::Membership(double x) const 
{
  TRACE(printf("FuzzyTrapez::Membership(%g)\n",x));
  if (x >=x1 && x <=x2)  return 1;
  if (x > x0 && x < x1)  return (x - x0) / (x1 - x0);
  if (x > x2 && x < x3)  return (x3 - x) / (x3 - x2);
  return 0;
}

/** It duplicates object.<br>Duplikuje objekt. */
FuzzyTrapez *FuzzyTrapez::clone() const 
{
  return new FuzzyTrapez(*this);
}

/** This adds next definition value.<br>Pøidá dal¹í definièní hodnotu. */
void FuzzyTrapez::addDefValue(double value)
{
  if (isComplete())
    SIMLIB_error("Bad trapez membership function definition. Too much definition values.");
  switch (defValues++)
  {
    case 0:
      x0 = value;
      break;
    case 1:
      x1 = value;
      if (x1 < x0) SIMLIB_error("Bad trapez membership function definition.");
      break;
    case 2:
      x2 = value;
      if (x2 < x1) SIMLIB_error("Bad trapez membership function definition.");
      break;
    case 3:
      x3 = value;
      if (x3 < x2) SIMLIB_error("Bad trapez membership function definition.");
      break;
    default: break;
  }
}
/////////////////////////////////////////////////////////////////////////////
// gauss
//
/**
 * It assigns word value to the membership function and sets definition values.<br>
 * Pøiøadí slovní hodnotu funkci pøíslu¹nosti a nastaví definièní hodnotu.
 * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
 * @param center A center of function.(mi) <br>Støed funkce. (mi)
 * @param radius A radius of function. (3*sigma)<br>Polomìr funkce. (3*sigma)
 */
FuzzyGauss::FuzzyGauss(const char *name, double center, double radius)
: FuzzyMembershipFunction(name), sigma(radius/3), c(center)
{ 
  TRACE(printf("FuzzyGauss::FuzzyGauss(%s)\n",name));
  defValues = 2;
  twoSqrSigma = 2*sigma*sigma;  // this variable must! be reinitialized everywhere, when sigma
//  if (sigma == 0)
//	  SIMLIB_error("Bad Gaussian membership function parameter sigma.");
}

/**
 * It assigns word value to the membership function.<br>
 * Pøiøadí slovní hodnotu funkci pøíslu¹nosti.
 * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
 */
FuzzyGauss::FuzzyGauss(const char* name)
: FuzzyMembershipFunction(name)
{
  TRACE(printf("FuzzyGauss::FuzzyGauss(%s)\n",name));
}

/** It computes function value (membership).<br>Vypoète funkèní hodnotu (pøíslu¹nost). */
double FuzzyGauss::Membership(double x) const 
{
  TRACE(printf("FuzzyGauss::Membership(%g)\n",x));
  if (twoSqrSigma == 0) return (x == c)? 1.0 : 0.0;
  else return exp(-((x-c)*(x-c))/twoSqrSigma);
}

/** It duplicates object.<br>Duplikuje objekt. */
FuzzyGauss *FuzzyGauss::clone() const 
{
  return new FuzzyGauss(*this);
}

/** This adds next definition value.<br>Pøidá dal¹í definièní hodnotu. */
void FuzzyGauss::addDefValue(double value)
{
  if (isComplete())
    SIMLIB_error("Bad Gaussian membership function definition. Too much definition values.");
  switch (defValues++)
  {
    case 0:
      c = value;
      break;
    case 1:
      sigma = value / 3;
      twoSqrSigma = 2*sigma*sigma;  // this variable must! be reinitialized everywhere, when sigma
//      if (sigma == 0)
//	      SIMLIB_error("Bad gauss membership function parameter sigma.");
      break;
    default: break;
  }
}

/////////////////////////////////////////////////////////////////////////////
// gauss2
//
/**
 * It assigns word value to the membership function and sets definition values.<br>
 * Pøiøadí slovní hodnotu funkci pøíslu¹nosti a nastaví definièní hodnoty.
 * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
 * @param leftCenter A center of the left function.(mi) <br>Støed levé funkce. (mi)
 * @param leftRadius A radius of the left function. (3*sigma)<br>Polomìr levé funkce. (3*sigma)
 * @param rightCenter A center of the right function.(mi) <br>Støed pravé funkce. (mi)
 * @param rightRadius A radius of the right function. (3*sigma)<br>Polomìr pravé funkce. (3*sigma)
 */
FuzzyGauss2::FuzzyGauss2(const char* name, 
                         double leftCenter, double leftRadius, 
                         double rightCenter, double rightRadius)
: FuzzyMembershipFunction(name)
{ 
  TRACE(printf("FuzzyGauss2::FuzzyGauss2(%s)\n",name));
  defValues = 4;
  this->leftCenter = leftCenter;
  this->rightCenter = rightCenter;
  this->leftSigma = leftRadius/3;
  this->rightSigma = rightRadius/3;
  twoSqrSigmaL = 2*leftSigma*leftSigma;  // this variable must! be reinitialized everywhere, when sigma
  twoSqrSigmaR = 2*rightSigma*rightSigma;
//  if (leftSigma == 0)
//	  SIMLIB_error("Bad Gaussian2 membership function parameter leftSigma.");
//  if (rightSigma == 0)
//	  SIMLIB_error("Bad Gaussian2 membership function parameter rightSigma.");
}

/**
 * It assigns word value to the membership function.<br>
 * Pøiøadí slovní hodnotu funkci pøíslu¹nosti.
 * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
 */
FuzzyGauss2::FuzzyGauss2(const char* name)
: FuzzyMembershipFunction(name)
{
  TRACE(printf("FuzzyGauss2::FuzzyGauss2(%s)\n",name));
}

/** It duplicates object.<br>Duplikuje objekt. */
FuzzyGauss2 *FuzzyGauss2::clone() const 
{
  return new FuzzyGauss2(*this);
}

/** It computes function value (membership).<br>Vypoète funkèní hodnotu (pøíslu¹nost). */
double FuzzyGauss2::Membership(double x) const 
{
  TRACE(printf("FuzzyGauss2::Membership(%g)\n",x));
  double y1;
  double y2;
  if (twoSqrSigmaL == 0) y1 = (x < leftCenter)? 0.0 : 1.0;
  else y1 = (x < leftCenter)? exp(-(x-leftCenter)*(x-leftCenter)/twoSqrSigmaL) : 1.0;

  if (twoSqrSigmaR == 0) y2 = (x > rightCenter)? 0.0 : 1.0;
  else y2 = (x > rightCenter)? exp(-(x-rightCenter)*(x-rightCenter)/twoSqrSigmaR) : 1.0;
  
  return y1*y2;
}

/** Center of this function.<br>Støed této funkce. */
//!! this is only the crude formula
double FuzzyGauss2::center() const
{
  if (leftCenter <= rightCenter)
    return (leftCenter-leftSigma*3 + 
            2*leftCenter + 
            2*rightCenter + 
            rightCenter+rightSigma*3)/6;
  else
    return (leftCenter-leftSigma*3 + vertexPosition() + rightCenter+rightSigma*3)/3;
} 

/** Position of vertex when leftCenter > rightCenter.<br>Pozice vrcholu, kdy¾ leftCenter > rightCenter. */
double FuzzyGauss2::vertexPosition() const
{
  return ( twoSqrSigmaL*rightCenter + twoSqrSigmaR*leftCenter )/( twoSqrSigmaL + twoSqrSigmaR );
}


/** First occurence of maximum in this function.<br>První výskyt maxima v této funkci. */
double FuzzyGauss2::min1() const 
{
  if (leftCenter <= rightCenter) return leftCenter; 
  else return vertexPosition();
}
/** Last occurence of maximum in this function.<br>Poslední výskyt maxima v této funkci. */
double FuzzyGauss2::max1() const 
{
  if (leftCenter <= rightCenter) return rightCenter; 
  else return vertexPosition();
}

/** This adds next definition value.<br>Pøidá dal¹í definièní hodnotu. */
void FuzzyGauss2::addDefValue(double value)
{
  if (isComplete())
    SIMLIB_error("Bad Gaussian membership function definition. Too much definition values.");
  switch (defValues++)
  {
    case 0:
      leftCenter = value;
      break;
    case 1:
      leftSigma = value/3;
      twoSqrSigmaL = 2*leftSigma*leftSigma;  // this variable must! be reinitialized everywhere, when sigma is
      break;
    case 2:
      rightCenter = value;
      break;
    case 3:
      rightSigma = value/3;
      twoSqrSigmaR = 2*rightSigma*rightSigma;  // this variable must! be reinitialized everywhere, when sigma is
      break;
    default: break;
  }
}

} // namespace

