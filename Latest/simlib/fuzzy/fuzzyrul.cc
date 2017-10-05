// cz - p��li� �lu�ou�k� k�� �p�l ��belsk� �dy
/////////////////////////////////////////////////////////////////////////////
// fuzzyrul.cc
//
// SIMLIB version: 2.16.3
// Date: 2001-04-04
// Copyright (c) 1999-2001  Dr. Ing. Petr Peringer, David Martinek
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//
// Warning: this is EXPERIMENTAL code, interfaces can be changed
//
// Fuzzy subsystem for SIMLIB
// version 0.6 --- We apr 4 10:33:52 CEST 2001 
// 
/////////////////////////////////////////////////////////////////////////////
// Implementation of fuzzy rules. 
/////////////////////////////////////////////////////////////////////////////

#include "simlib.h"
#include "fuzzy.h"
#include "internal.h"
#include <cstdio>
#include <cassert>
#include <cmath>

#include <typeinfo>
#include <string.h>
// global result of if(condition) rule 
// should be in static of class FuzzyInferenceRules
//static double FuzzyRuleValue=1.0f;
//static double RuleWeight=1.0;

namespace simlib3 {

/////////////////////////////////////////////////////////////////////////////
// Static class Rules

FuzzyGeneralRules * Rules::rules = NULL;
FuzzyRuleFactory * Rules::factory = NULL;
bool Rules::addedInput = false;
bool Rules::addedOutput = false;
bool Rules::addRule = false;

/** It initializes data members. */
void Rules::init()
{
  if (Rules::rules != NULL) delete rules;
  if (Rules::factory != NULL) delete factory;
  Rules::rules = NULL;
  Rules::factory = NULL;
  Rules::addedInput = false;
  Rules::addedOutput = false;
  Rules::addRule = false;
}

/**
 * It adds next input into the list. This is needed because of error checking.<br>
 * P�id� dal�� vstup do seznamu. Tato metoda je pot�eba kv�li o�et�en� chyb.
 */
void Rules::addFuzzyInput(FuzzyInput * input)
{
  if (Rules::rules == NULL) { Rules::rules = new FuzzyGeneralRules(); }
  Rules::rules->addFuzzyInput(input);
  Rules::addedInput = true;
}

/**
 * It adds next output into the list. This is needed because of error checking.<br>
 * P�id� dal�� v�stup do seznamu. Tato metoda je pot�eba kv�li o�et�en� chyb.
 */
void Rules::addFuzzyOutput(FuzzyOutput * output)
{
  if (Rules::rules == NULL) { Rules::rules = new FuzzyGeneralRules(); }
  Rules::rules->addFuzzyOutput(output);
  Rules::addedOutput = true;
}

/**
 * It returns complete definition of inference rules. See at class FuzzyExpr to see how to
 * create inference rules.<br>
 * Vrac� kompletn� definici inferen�n�ch pravidel. Pod�vejte se na t��du FuzzyExpr jak se
 * vytv��� inferen�n� pravidla.
 */
FuzzyGeneralRules * Rules::getRules()
{
  if (!(Rules::addedOutput && Rules::addedInput))
  { SIMLIB_error("In fuzzy regulator must be defined inputs and outputs!"); }
  FuzzyGeneralRules * result = Rules::rules;
  Rules::rules = NULL;
  Rules::init();
  return result;
}

/**
 * It produces error if there is not enough inputs and outputs or it returns FuzzyRuleFactory.<br>
 * Zp�sob� chybu, jestli�e nen� definov�no dostate�n� mno�stv� vstup� a v�stup� nebo vr�t�
 * FuzzyRuleFactory.
 */
FuzzyRuleFactory * Rules::Factory()
{
  if (!(Rules::addedOutput && Rules::addedInput))
  { SIMLIB_error("In fuzzy regulator must be defined inputs and outputs!"); }
  if (Rules::factory == NULL) 
  { Rules::factory = Rules::rules->createRuleFactory(); }
  return Rules::factory;
}


/**
 * It adds new rule into rules.<br>P�id� nov� pravidlo do rules.
 */
void Rules::addNewRule()
{
  Rules::rules->add(Rules::factory->createRule());
}

/////////////////////////////////////////////////////////////////////////////
// FuzzyOutput --- continuous output (aggregation, defuzzification)
//

/**
 * It adds fuzzy set and defuzzification function.<br>
 * P�id� fuzzy mno�inu a defuzzifika�n� funkci.
 * @param t   Fuzzy set.<br>Fuzzy mno�ina.
 * @param def Defuzzification function.<br>Defuzzifika�n� funkce.
 */
FuzzyOutput::FuzzyOutput(const FuzzySet &t, double (*def)(const FuzzyVariable&)/*=0*/) 
  : FuzzyVariable(t), defuzzify(def) 
{
  TRACE(printf("FuzzyOutput::FuzzyOutput()\n"));
  // default: zero all
}

/**
 * It assignes fuzzy value by word value.<br>P�i�ad� fuzzy hodnotu pomoc� slovn� hodnoty.
 */
// op = in fuzzy rule
const char *FuzzyOutput::operator = (const char *val) 
{
/*  int i = search(val);
  (*this)[i] = max((*this)[i], RuleWeight*FuzzyRuleValue);
*/
//  Rules::Factory()->addConsequent(Rules::Factory()->createNode(this, strdup(val)));  
  Rules::Factory()->addConsequent(Rules::Factory()->createNode(this, val));  
  Rules::addRule = true;
  return val; // allow a=b=c="xx" 
}

/** 
 * It defuzzifies itself.<br> Defuzzifikuje se. 
 * @return It returns sharp value.<br>Vr�t� osrou hodnotu.
 */
double FuzzyOutput::Defuzzify() 
{ 
//  Print();
  if(defuzzify==0) SIMLIB_error("no defuuzification method set");
  value = defuzzify(*this); 
//::Print("Defuzzify=%g\n",value);
  return value;
}

/** It adds defuzzification function.<br>P�id� defuzzifika�n� funkci. */
void FuzzyOutput::SetDefuzzifyMethod(double (*f)(const FuzzyVariable&)) 
{
  if(f==0) SIMLIB_error("no defuzification method");
  defuzzify = f; 
}

/**
 * Evaluates FuzzyBlock (variable where) and after defuzzification returns sharp value.<br>
 * Vyhodnot� FuzzyBlock (prom�nnou whera) a po defuzzifikaci vr�t� ostrou hodnotu.
 * @return Sharp value after defuzzification.<br>Ostr� hodnota po defuzzifikaci.
 */
double FuzzyOutput::Value() 
{ 
  if (Where() == 0) SIMLIB_error("Fuzzy set should be used inside FuzzyBlock only");
  Where()->Evaluate(); // evaluate fuzzy block
  return value; // return value;
}

FuzzyOutput::~FuzzyOutput() { TRACE(printf("~FuzzyOutput()\n")); } 

/////////////////////////////////////////////////////////////////////////////
// FuzzyExpr --- fuzzy expression value class
// TODO: check if allowed -- global state set in class FuzzyInfer
/**
 * Objects of this class are usualy created inside if command.<br>
 * Objekty t�to t��dy jsou obvykle vytv��eny uvnit� p��kazu if.
 */
FuzzyExpr::FuzzyExpr(FONode * node) : value(node) { }

/**
 * It creates object tree.<br>Vytvo�� strom objekt�.
 */
FuzzyExpr::operator bool() 
{ 
/*
  FuzzyRuleValue=value; 
  RuleWeight=1.0; 
*/
  Rules::Factory()->addCondition(dynamic_cast<FOperation *>(Value()));
  return true; // always go to then section
}


/////////////////////////////////////////////////////////////////////////////
// Fuzzy operators
//
// AND
/**
 * if ((FuzzyExpr) && (FuzzyExpr)) 
 */
FuzzyExpr operator && (FuzzyExpr o1, FuzzyExpr o2) 
{
//  return FuzzyExpr(min(o1.Value(), o2.Value()));
  return FuzzyExpr(Rules::Factory()->createNode(o1.Value(), o2.Value(), FuzzyInferenceRules::opAND));
}

// OR
/**
 * if ((FuzzyExpr) || (FuzzyExpr)) 
 */
FuzzyExpr operator || (FuzzyExpr o1, FuzzyExpr o2) 
{
//  return FuzzyExpr(max(o1.Value(), o2.Value()));
  return FuzzyExpr(Rules::Factory()->createNode(o1.Value(), o2.Value(), FuzzyInferenceRules::opOR));
}

// NOT
/**
 * if (!(FuzzyExpr)) 
 */
FuzzyExpr operator ! (FuzzyExpr o1) 
{
//  return FuzzyExpr(1-o1.Value());
  return FuzzyExpr(Rules::Factory()->createNode(o1.Value(), FuzzyInferenceRules::opNOT));
}

// is
/**
 * if ((input == "wordvalue")...) 
 */
FuzzyExpr operator == (FuzzyInput &in, const char *s) 
{
  if (Rules::addRule)
  {
    Rules::addRule = false;
    Rules::addNewRule();
  }
//  return FuzzyExpr(Rules::Factory()->createNode(&in, strdup(s)));
    return FuzzyExpr(Rules::Factory()->createNode(&in, s));
}

// TODO: kontrola neprazdne then-casti
// rule:
// if (a=="small" && b=="big") out="big"
// alternativni syntaxe:  (e1) -> (e2)
//                        (e1), (e2)
//                        (e1); (e2)

// e = ||||||||||||||||||||||
// out("big").add( e )
// 2lists:
// expression += e
// result += out

/////////////////////////////////////////////////////////////////////////////
// FuzzyBlock --- base class for inference blocks
// 

FuzzyBlock *activeFuzzyBlock = 0; // 0 == global

/**
 * If inference rules are specified by FuzzyExpr way then you must call 
 * EndConstructor method on the end of constructor.<br>
 * Jestli�e jsou inferen�n� pravidla specifikov�na pomoc� FuzzyExpr, pak mus�te
 * zavolat metodu EndConstructor na konci konstruktoru.
 */
FuzzyBlock::FuzzyBlock() 
{
  where = activeFuzzyBlock; // hierarhical position
  activeFuzzyBlock = this; 
  lastTime = -1.0;
}

/**
 * If inference rules are specified by FuzzyExpr way then you must call this
 * method on the end of constructor.<br>
 * Jestli�e jsou inferen�n� pravidla specifikov�na pomoc� FuzzyExpr, pak mus�te
 * zavolat tuto metodu na konci konstruktoru.
 */
void FuzzyBlock::EndConstructor() 
{
  activeFuzzyBlock = where;
}

/**
 * It registers fuzzy variable inside this object. If inference rules are NOT specified 
 * by FuzzyExpr way then you must call this method inside the constructor for all FuzzyInput
 * and FuzzyOutput variables.<br>
 * Registruje fuzzy prom�nnou v tomto objektu. Jestli�e NEjsou inferen�n� pravidla 
 * specifikov�na pomoc� FuzzyExpr, pak mus�te zavolat tuto metodu v konstruktoru pro v�echny
 * prom�nn� typu FuzzyInput a FuzzyOutput.
 */
void FuzzyBlock::Register(FuzzyVariable *fs) 
{
  vlist.push_back(fs);
}

/**
 * It evaluates whole block. It calls method Behavior.<br>
 * Vyhodnot� cel� blok. Vol� metodu Behavior.
 */
void FuzzyBlock::Evaluate() 
{
  // TODO: OPTIMIZATION: check if evaluated 
  // #### warning: returns-in-time, algebraic loops, discontinuities ###!!!
  if(lastTime==Time) return; // was already evaluated at this time
  lastTime = Time;
  // for_each
  dlist<FuzzyVariable*>::iterator i;
  for(i=vlist.begin(); i!=vlist.end(); i++)
     (*i)->Init(); // fuzzify input, init output
  Behavior(); // execute rules, aggregate output
  // for_each
  for(i=vlist.begin(); i!=vlist.end(); i++)
     (*i)->Done(); // defuzzify outputs
}

/////////////////////////////////////////////////////////////
//  2001-02-26 - David Martinek added class FuzzySampledBlock
//  - sampled fuzzy block with inference rules
/**
 * It evaluates fuzzy inference rules but only in sampled time steps.<br>
 * Vyhodnot� fuzzy inferen�n� pravidla, ale jenom ve vzorkovan�ch �asov�ch okam�ic�ch.
 */
void FuzzySampledBlock::Evaluate()
{
  if ((Time - lastTime >= sampler->getTimeStep()) || (Time == 0))
  {
    lastTime = Time;
    dlist<FuzzyVariable*>::iterator i;
    for (i = vlist.begin(); i != vlist.end(); i++)
      (*i)->Init(); // fuzzify input, init output
    Behavior();
    for (i = vlist.begin(); i != vlist.end(); i++)
      (*i)->Done(); // defuzzify outputs
  }
}

} // namespace 

