/////////////////////////////////////////////////////////////////////////////
//! \file cond.cc  State conditions
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
// description: continuous system - state conditions
//

////////////////////////////////////////////////////////////////////////////
// interface
//

#include "simlib.h"
#include "internal.h"

namespace simlib3 {


////////////////////////////////////////////////////////////////////////////
// implementation
//

SIMLIB_IMPLEMENTATION;


bool SIMLIB_ConditionFlag = false;       // condition vector changed
aCondition *aCondition::First = 0;       // condition list

////////////////////////////////////////////////////////////////////////////
// aCondition implementation
//
aCondition::aCondition() :
  Next(First)
{
  First = this;
}

aCondition::~aCondition() {
  if (this==First)
    First = Next;
  else
  {
    aCondition *i;
    for(i=First; i && i->Next!=this; i=i->Next) { /*empty*/ }
    if (i) i->Next = Next;
  }
}

////////////////////////////////////////////////////////////////////////////
// aCondition::InitAll -- initialize all conditions
//
void aCondition::InitAll() {
  SIMLIB_ConditionFlag = false;
  for(aCondition *i=First; i; i=i->Next)
    i->Init();
}

////////////////////////////////////////////////////////////////////////////
// aCondition::SetAll
//
void aCondition::SetAll() {
  for(aCondition *i=First; i; i=i->Next)
    i->SetNewStatus();
}

bool aCondition::isAny()  { return First!=0; }

////////////////////////////////////////////////////////////////////////////
// Condition implementation
//
Condition::Condition(Input i) :
    in(i),
    cc(0),
    ccl(0)
{
}

Condition::~Condition()
{
}

////////////////////////////////////////////////////////////////////////////
// Condition::Init -- initialize
//
void Condition::Init()
{
  cc = ccl = 0;
}

////////////////////////////////////////////////////////////////////////////
// Condition::SetNewStatus -- set new status of condition
//
void Condition::SetNewStatus()
{
  ccl = cc;
}


////////////////////////////////////////////////////////////////////////////
/// Condition::operator ()
//
bool Condition::Test() {
  bool x = (in.Value()>=0.0);
  if(SIMLIB_DynamicFlag)       // inside numerical integration step
  {
    cc = x;                    // new condition status
    if(Change()) {             // is change of status?
      SIMLIB_ConditionFlag = true;  // global change flag
      ContractStep();          // need to step contraction
    }
    return false;              // test only
  }
  return Change();             // do actions if changed
}

////////////////////////////////////////////////////////////////////////////
//  test or action
//
void aCondition::TestAll()
{
  SIMLIB_ConditionFlag = false;
  for(aCondition *i=aCondition::First; i ; i=i->Next)
    if(i->Test()) SIMLIB_ConditionFlag = true;
}

void aCondition::AllActions()
{
  for(aCondition *i=aCondition::First; i ; i=i->Next)
    if(i->Test()) i->Action(); /// Change???? ### !!!!
}

////////////////////////////////////////////////////////////////////////////
//  SIMLIB_DoConditions -- perform reactions to condition change
//
void SIMLIB_DoConditions()
{
  // precondition:  SIMLIB_DynamicFlag = false;
  if (SIMLIB_ConditionFlag)
  {
    void SampleDelays(); // needs something better ###
    SampleDelays();

    aCondition::AllActions();    // perform reactions to condition change
    aCondition::SetAll();        // register new status
    SIMLIB_ResetStatus = true;   // ??? always reset integration method ???
  }
}

////////////////////////////////////////////////////////////////////////////
/// get name of object
const char *Condition::Name() const {
  if(HasName()) return _name;
  else return SIMLIB_create_tmp_name("Condition{%p}", this);
}

////////////////////////////////////////////////////////////////////////////
/// get name of object
const char *ConditionUp::Name() const {
  if(HasName()) return _name;
  else return SIMLIB_create_tmp_name("ConditionUp{%p}", this);
}

////////////////////////////////////////////////////////////////////////////
/// get name of object
const char *ConditionDown::Name() const {
  if(HasName()) return _name;
  else return SIMLIB_create_tmp_name("ConditionDown{%p}", this);
}


} // namespace

