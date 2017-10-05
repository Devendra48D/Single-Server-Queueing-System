/////////////////////////////////////////////////////////////////////////////
// ruletree.cc
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
// Implementation of genereal interface for typing in, representation and
// evaluation of inference rules in a tree structure.
/////////////////////////////////////////////////////////////////////////////

#include "fuzzy.h"
#include <internal.h>
#include <algorithm>
#include <typeinfo>

namespace simlib3 {

/**
 * Constructor for an unary operator. <br> Konstruktor pro un�rn� oper�tor.
 */
FOperation::FOperation(FONode *operand, FuzzyInferenceRules::Operations operation)
: //FONode(FONode::ntOperation), 
  op(operation), L(operand), R(NULL)
{
  if (operation != FuzzyInferenceRules::opNOT)
  {
    SIMLIB_error("FOperation::FOperation: if operand is single then operation have to be opNOT!");
  }
}

/** 
 * Constructor is protected against user. Object of this class can create only
 * object of class FuzzyInferenceRules.<br>
 * Konstruktor je chr�n�n p�ed u�ivatelem. Objekt t�to t��dy m��e vytvo�it
 * pouze objekt t��dy FuzzyInferenceRules.
 */
FuzzyRuleFactory::FuzzyRuleFactory(FuzzyInferenceRules * owner)
{
  this->owner = owner; 
  rule = new FuzzyRule();
}

/**
 * It safely creates fuzzy inference rule.<br>
 * Vrac� bezpe�n�m zp�sobem vytvo�en� pravidlo.
 */
FuzzyRule * FuzzyRuleFactory::createRule()
{
  FuzzyRule *result = rule;
  rule = new FuzzyRule();
  return result;
}

/**
 * It creates a leaf knot of rule. <br>
 * Vytvo�� listov� uzel pravidla.
 */
FPair * FuzzyRuleFactory::createNode(FuzzyVariable *var, const char * wordvalue, bool equal)
{
  if (typeid(*var) == typeid(FuzzyInput))
  {
    std::vector<FuzzyInput *>::iterator where = find(owner->in.begin(), owner->in.end(), var);
    if (where == owner->in.end())
      SIMLIB_error("FuzzyRuleFactory::createNode: can not find this FuzzyInput inside FuzzyInferenceRules!");
  }
  else if (typeid(*var) == typeid(FuzzyOutput))
  {
    std::vector<FuzzyOutput *>::iterator where = find(owner->out.begin(), owner->out.end(), var);
    if (where == owner->out.end())
      SIMLIB_error("FuzzyRuleFactory::createNode: can not find this FuzzyOutput inside FuzzyInferenceRules!");
  }
  return new FPair(var, wordvalue, equal);
}

/**
 * It creates a nonleaf knot of rule representing binary operator.<br>
 * Vytvo�� nelistov� uzel pravidla reprezentuj�c� bin�rn� oper�tor.
 */
FOperation * FuzzyRuleFactory::createNode(FONode *left, FONode *right, FuzzyInferenceRules::Operations operation)
{
  return new FOperation(left, right, operation);
}

/**
 * It creates a nonleaf knot of rule representing unary operator.<br>
 * Vytvo�� nelistov� uzel pravidla reprezentuj�c� un�rn� oper�tor.
 */
FOperation * FuzzyRuleFactory::createNode(FONode *operand, FuzzyInferenceRules::Operations operation)
{
  return new FOperation(operand, operation);
}

/**
 * It adds condition of rule.<br>
 * P�id� podm�nkovou ��st do pravidla.
 */
void FuzzyRuleFactory::addCondition(FOperation * operation)
{
//  if (operation->getNodeType() != FONode::ntOperation)
//   if (typeid(*operation) != typeid(FOperation))
//   {
//     SIMLIB_error("FuzzyRuleFactory::addCondition: operation have to be ntOperation!");
//   }
//   
  rule->addLeft(operation);
}

/**
 * It adds next assign command into rule.<br>
 * P�id� dal�� p�i�azovac� p��kaz do p��kazov� ��sti pravidla (konsekvent). 
 */
void FuzzyRuleFactory::addConsequent(FPair * consequent)
{
//  if (consequent->getNodeType() != FONode::ntPair)
//   if (typeid(*consequent) != typeid(FPair))
//   {
//     SIMLIB_error("FuzzyRuleFactory::addConsequent: consequent have to be ntPair!");
//   }
//   FPair * consq = dynamic_cast<FPair *>(consequent);
  if (typeid(*(consequent->var)) != typeid(FuzzyOutput))
  {
    SIMLIB_error("FuzzyRuleFactory::addConsequent: consequent have to contain FuzzyOutput!");
  }
  rule->addRight(consequent);
}

/**
 * Constructor.
 */
FuzzyRule::FuzzyRule()
{
  left = NULL;
}

/**
 * Function for deleting objects in vector.<br>
 * Funkce pro maz�n� objekt� ve vektoru.
 */
void del(FPair *p) { delete p; }

/**
 * Destructor releases memory alocated by tree. <br> Destruktor uvoln� pam� alokovanou stromem.
 */
FuzzyRule::~FuzzyRule()
{
  if (left != NULL)
  for_each(right.begin(), right.end(), del);
  right.erase(right.begin(), right.end());
  TRACE(printf("~FuzzyRule\n")); 
}

/**
 * It adds the lvalue of rule. <br> P�id� levou stranu pravidla.
 */
void FuzzyRule::addLeft(FOperation *left) 
{
  this->left = left;
}

/**
 * It adds one command into list. <br> P�id� dal�� p��kaz do seznamu.
 */
void FuzzyRule::addRight(FPair *right) 
{
  this->right.push_back(right);
}

////////////////////////////////////////////////////////////////////////////////////
// Rule evaluation
////////////////////////////////////////////////////////////////////////////////////

/**
 * If var is the FuzzyInput then this method returns fuzzified value of input.
 * In other words - value of membership function with name wordValue. Do not use if var is
 * a class FuzzyOutput.
 *
 * Jestli�e parametr var je FuzzyInput, tato metoda vrac� fuzzifikovanou hodnotu vstupu.
 * Jin�mi slovy - hodnotu funkce p��slu�nosti se jm�nem wordValue. Nepou��vejte jestli�e je 
 * var t��dy FuzzyOutput.
 */
double FPair::getValue()
{
  return (*var)[indexWV];
}


/* pomocn� funkce */
inline double rmin(double x, double y) { return (x < y) ? x : y; }
inline double rmax(double x, double y) { return (x > y) ? x : y; }
/**
 * It returns value after doing operation op.<br>Vrac� hodnotu po proveden� operace op.
 */
double FOperation::getValue()
{
  switch (op)
  {
    case FuzzyInferenceRules::opAND:
      return rmin(L->getValue(), R->getValue());
      break;
    case FuzzyInferenceRules::opOR:
      return rmax(L->getValue(), R->getValue());
      break;
    case FuzzyInferenceRules::opNAND:
      return 1 - rmin(L->getValue(), R->getValue());
      break;
    case FuzzyInferenceRules::opNOR:
      return 1 - rmax(L->getValue(), R->getValue());
      break;
    case FuzzyInferenceRules::opNOT:
      return 1 - L->getValue();
      break;
    default:
      return 0;
      break;
  }
  
}

/**
 * It evaluates one inference rule according to fuzzy model Mamdani.<br>
 * Vyhodnot� jedno inferen�n� pravidlo podle fuzzy modelu Mamdani. 
 */
void FuzzyRule::evaluate()
{
  for (unsigned int i = 0; i < right.size(); i++)
  {
    FuzzyVariable *out = right[i]->var;
    int indexWV = right[i]->indexWV;
    (*out)[indexWV] = rmax((*out)[indexWV], left->getValue());
  }
}

} // namespace 

