/////////////////////////////////////////////////////////////////////////////
// rules.cc
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
// FuzzyIIORules - specialized interface for typing in, representation and evaluation
// of inference rules for a fuzzy controller with two inputs and one output.
// FuzzyGeneralRules - interface for general fuzzy controller
/////////////////////////////////////////////////////////////////////////////
#include "fuzzy.h"
#include <internal.h>
#include <stdio.h>
#include <typeinfo>

namespace simlib3 {

/**
 * It safely creates FuzzyRuleFactory.<br>
 * Bezpeèným zpùsobem vytvoøí FuzzyRuleFactory.
 */
FuzzyRuleFactory * FuzzyInferenceRules::createRuleFactory()
{
  return new FuzzyRuleFactory(this);
}

/////////////////////////////////////////////////////////////////////////////////////////
// FuzzyIIORules
/////////////////////////////////////////////////////////////////////////////////////////

/**
 * Parameterless constructor. Fuzzy variables must be added explicitly.<br>
 * Konstruktor bez parametru. Fuzzy promìnné je nutno pøiøadit pomocí dal¹ích funkcí explicitnì.
 */
FuzzyIIORules::FuzzyIIORules() 
{
  init();
}

/**
 * It initializes all input and output variables.<br>
 * Inicializuje v¹echny vstupní a výstupní promìnné.
 */
FuzzyIIORules::FuzzyIIORules(FuzzyInput *in1, FuzzyInput *in2, FuzzyOutput *out)
{
  init();
  addFuzzyInput(in1);
  addFuzzyInput(in2);
  addFuzzyOutput(out);
}

/**
 * Destructor - does NOT free memory allocated by FuzzyInput and FuzzyOutput!<br>
 * Destruktor - NEuvolòuje pamì» zabranou promìnnými FuzzyInput a FuzzyOutput! 
 * Uvolnìní této pamìti se musí provést JINDE!
 */
FuzzyIIORules::~FuzzyIIORules() 
{
  TRACE(printf("~FuzzyIIORules()\n"));
  delete[] operation;
  delete[] outWV;
}


/**
 * It adds next rule into list. If there is too much rules, an error is indicated.<br>
 * Pøidá dal¹í pravidlo do seznamu. Pokud u¾ je definováno pøíli¹ pravidel, nastane chyba.
 * @param rule Inference rule with tree structure.<br>Inferenèní pravidlo reprezentované stromovou strukturou.
 */
void FuzzyIIORules::add(FuzzyRule *rule, bool release/*=true*/)
{
  if (
      (typeid(*(rule->left->L)) != typeid(FPair)) ||
      (typeid(*(rule->left->R)) != typeid(FPair))
     )
  {
    SIMLIB_error("FuzzyIIORules::add: Tree in parameter (FuzzyRule) has bad structure!");
  }
  FPair * L = dynamic_cast<FPair *>(rule->left->L);
  FPair * R = dynamic_cast<FPair *>(rule->left->R);
  
  if (
       (typeid(*(L->var)) != typeid(FuzzyInput)) ||
       (typeid(*(R->var)) != typeid(FuzzyInput)) ||
       !L->eq ||
       !R->eq
     )
  {
    SIMLIB_error("FuzzyIIORules::add: Tree in parameter (FuzzyRule) has bad structure!");
  }
  
  if (rule->right.size() != 1)
  {
    SIMLIB_error("FuzzyIIORules::add: In rule is one command in consequent expected!");
  }
  // není tøeba ovìøovat, jestli je Lvar a Rvar ve vektoru in
  add(rule->left->op, L->wordValue, R->wordValue, rule->right[0]->wordValue);
  if (release)
  {
    delete rule;
    rule = NULL;
  }
}


/** It initializes all member variables. <br> Inicializuje vnitøní promìnné */
void FuzzyIIORules::init() 
{
//  operation.reserve(2);
//  outWV.reserve();
  operation = NULL;
  outWV = NULL;
  out.reserve(1);
  in.reserve(2);
  inputs = outputs = rules = 0;
}


/**
 * It alocates a memory space for operation and outWV arrays. A alocated array size is
 * a product of numbers of word values of all input variables.<br>
 * Alokuje prostor pro pole operation a outWV. Velikost alokovaného pole je souèinem 
 * poètù slovních hodnot v¹ech jednotlivých vstupních promìnných.
 */
void FuzzyIIORules::createVectors()
{
  unsigned length = 1;
  for (int i = 0; i < MAX_INPUTS; i++) 
  {
    length *= in[i]->count();
  }
  operation = new Operations[length];
  outWV = new int[length];
}

/** It tests if all arrays are allocated.<br>Testuje, jestli u¾ jsou alokována pole. */
bool FuzzyIIORules::isAllCreated()
{
  return (operation != NULL) && (outWV != NULL);
}

/** It adds next input variable.<br> Vlo¾í dal¹í vstupní promìnnou. */
void FuzzyIIORules::addFuzzyInput(FuzzyInput *in)
{
  if (inputs < MAX_INPUTS) { inputs++; FuzzyInferenceRules::addFuzzyInput(in); }
  else { SIMLIB_error("FuzzyIIORules: can not store more than 2 fuzzy inputs."); }
  if (isComplete() && !isAllCreated()) createVectors();
}

/** It adds output variable.<br> Vlo¾í výstupní promìnnou. */
void FuzzyIIORules::addFuzzyOutput(FuzzyOutput *out)
{
  if (outputs < MAX_OUTPUTS) { outputs++; FuzzyInferenceRules::addFuzzyOutput(out); }
  else { SIMLIB_error("FuzzyIIORules: can not store more than 1 fuzzy output."); }
  if (isComplete() && !isAllCreated()) createVectors();
}

/**
 * It tests if all variables are assigned.<br>
 * Testuje, jestli u¾ jsou pøiøazeny v¹echny promìnné.
 * @see addFuzzyInput(FuzzyInput *in)
 * @see addFuzzyOutput(FuzzyOutput *out)
 */
bool FuzzyIIORules::isComplete()
{
   return (inputs == MAX_INPUTS) && (outputs == MAX_OUTPUTS);
}

/**
 * Adds a rule into the list.<br>
 * Pøídá inferenèní pravidlo do seznamu.
 * @param operation   operation which is used in rule<br>operace, která se pou¾ije v pravidle
 * @param in1WVIndex  index of first fuzzy input variable<br>index slovní hodnoty první vstupní promìnné
 * @param in2WVIndex  index of second fuzzy input variable<br>index slovní hodnoty druhé vstupní promìnné
 * @param outWVIndex  index of output fuzzy variable<br>index slovní hodnoty výstupní promìnné
 */
void FuzzyIIORules::add (const FuzzyInferenceRules::Operations operation, 
                         int in1WVIndex, 
                         int in2WVIndex, 
                         int outWVIndex)
{
//  if (rules >= in[2]->count*in[1]->count()) // chyba
  rules++;
  unsigned index = in2WVIndex*in[1]->count()+in1WVIndex;
  this->operation[index] = operation;
  this->outWV[index] = outWVIndex;
//  printf("add (in1WVIndex=%d, in2WVIndex=%d, outWVIndex=%d) at index %d\n", in1WVIndex, in2WVIndex, outWVIndex, index);
}

/**
 * Adds a rule into the list.<br>
 * Pøídá inferenèní pravidlo do seznamu.
 * @param operation     operation which is used in rule<br>operace, která se pou¾ije v pravidle
 * @param in1WordValue  word value of first fuzzy input variable<br>slovní hodnota první vstupní fuzzy promìnné
 * @param in2WordValue  word value of second fuzzy input variable<br>slovní hodnota druhé vstupní fuzzy promìnné
 * @param outWordValue  word value of output fuzzy variable<br>slovní hodnota výstupní fuzzy promìnné
 */
void FuzzyIIORules::add (const FuzzyInferenceRules::Operations operation, 
                         const char *in1WordValue, 
                         const char *in2WordValue, 
                         const char *outWordValue)
{
  add(operation, in[0]->search(in1WordValue), in[1]->search(in2WordValue), out[0]->search(outWordValue));
}

/**
 * Adds a rule into the list.<br>
 * Pøídá inferenèní pravidlo do seznamu.
 * @param operation     operation which is used in rule<br>operace, která se pou¾ije v pravidle
 * @param in1WVIndex    index of first fuzzy input variable<br>index slovní hodnoty první vstupní promìnné
 * @param in2WVIndex    index of second fuzzy input variable<br>index slovní hodnoty druhé vstupní promìnné
 * @param outWordValue  word value of output fuzzy variable<br>slovní hodnota výstupní fuzzy promìnné
 */
void FuzzyIIORules::add (const FuzzyInferenceRules::Operations operation, 
                         int in1WVIndex, 
                         int in2WVIndex, 
                         const char *outWordValue)
{
  add(operation, in1WVIndex, in2WVIndex, out[0]->search(outWordValue));
}

/* pomocné funkce */
inline double rmin(double x, double y) { return (x < y) ? x : y; }
inline double rmax(double x, double y) { return (x > y) ? x : y; }

/**
 * It evaluates the rules.<br>
 * Vyhodnotí pravidla.
 */
void FuzzyIIORules::evaluate()
{
//  if (rules >= in[2]->count*in[1]->count()) // chyba
  for (int i = 0; i < rules; i++)
  {
    unsigned outindex = outWV[i];            // index slovní hodnoty výstupu
    unsigned in0index = i % in[0]->count();  // index slovní hodnoty první vstupní promìnné
    unsigned in1index = i/in[0]->count();    // index slovní hodnoty druhé vstupní promìnné
//    printf("i: %d, outindex: %d, in0index: %d, in1index: %d\n", i, outindex, in0index, in1index);
    switch (operation[i])
    {
      case opAND:// if (in0 == "something" && in1 == "someother") { out = "some"; }
      // (*out)[i] - zpøístupní polo¾ku pole mval u FuzzyOutput
      // (*in[])[i] - vrátí hodnotu z mval, kde je aktuální fuzzifikovaná hodnota
      // = pro inty není pøetí¾eno, proto pouze pøiøadí výsledek
        (*out[0])[outindex] = rmax((*out[0])[outindex], rmin((*in[0])[in0index], (*in[1])[in1index])); 
        break;
      case opOR:// if (in0 == "something" || in1 == "someother") { out = "some"; }
        (*out[0])[outindex] = rmax((*out[0])[outindex], rmax((*in[0])[in0index], (*in[1])[in1index]));
        break;
      case opNAND:// if (!(in0 == "something" && in1 == "someother")) { out = "some"; }
        (*out[0])[outindex] = rmax((*out[0])[outindex], 1-rmin((*in[0])[in0index], (*in[1])[in1index]));
        break;
      case opNOR:// if (!(in0 == "something" || in1 == "someother")) { out = "some"; }
        (*out[0])[outindex] = rmax((*out[0])[outindex], 1-rmax((*in[0])[in0index], (*in[1])[in1index]));
        break;
      default:
        break;
    }
  }
}
/////////////////////////////////////////////////////////////////////////////////////////
// FuzzyGeneralRules
/////////////////////////////////////////////////////////////////////////////////////////
/**
 * It returns true when all variables are assigned.<br>
 * Testuje, jestli u¾ jsou pøiøazeny v¹echny promìnné.
 * @see addFuzzyInput(FuzzyInput *in)
 * @see addFuzzyOutput(FuzzyOutput *out)
 */
bool FuzzyGeneralRules::isComplete()
{
  return (in.size() > 0) && (out.size() > 0);
}

/**
 * It adds next rule into list. When it is too much rules here, error is indicated.<br>
 * 
 * Pøidá dal¹í pravidlo do seznamu. Pokud u¾ je definováno pøíli¹ pravidel, nastane chyba.
 * @param rule Inference rule who is represented by tree structure.
 *             Inferenèní pravidlo reprezentované stromovou strukturou.
 * @param release It has no meaning here.<br>Zde nemá význam.
 */
void FuzzyGeneralRules::add(FuzzyRule *rule, bool release/*=true*/)
{
  rules.push_back(rule);
}

/**
 * It evaluates all rules. This method is defaultly called from a method Behavior in class
 * FuzzyRSBlock.<br>
 * Vyhodnotí pravidla. Tato funkce je standardnì volána z funkce Behavior tøídy 
 * FuzzyRSBlock.
 */
void FuzzyGeneralRules::evaluate()
{
  for (unsigned int i = 0; i < rules.size(); i++)
  {
    (*rules[i]).evaluate();
  }
}

} // namespace

