/////////////////////////////////////////////////////////////////////////////
//! \file intg.cc    Integrator block implementation
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  description: continuous system - integrator & status variables
//
//  clas Integrator and Status implementation
//


////////////////////////////////////////////////////////////////////////////
// interface

#include "simlib.h"
#include "internal.h"

#include <cmath>


////////////////////////////////////////////////////////////////////////////
// implementation

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

int SIMLIB_ERRNO=0;

double SIMLIB_StepStartTime;         //!< last step time
double SIMLIB_DeltaTime;             //!< Time-SIMLIB_StepStartTime

double SIMLIB_OptStep;               //!< optimal step
double SIMLIB_MinStep=1e-10;         //!< minimal step
double SIMLIB_MaxStep=1;             //!< max. step
double SIMLIB_StepSize;              //!< actual step

double SIMLIB_AbsoluteError=0;       //!< absolute error
double SIMLIB_RelativeError=0.001;   //!< relative error

// step limits
const double &MinStep=SIMLIB_MinStep;        //!< minimal integration step
const double &MaxStep=SIMLIB_MaxStep;        //!< maximal integration step
const double &StepSize=SIMLIB_StepSize;      //!< actual integration step
const double &OptStep=SIMLIB_OptStep;        //!< optimal integration step
//const double &StepStartTime=SIMLIB_StepStartTime; // start of step

// error params
const double &AbsoluteError=SIMLIB_AbsoluteError; //!< max. abs. error of integration
const double &RelativeError=SIMLIB_RelativeError; //!< max. rel. error

bool SIMLIB_DynamicFlag = false;          //!< in dynamic section

bool SIMLIB_ContractStepFlag = false;     //!< requests shorter step
double  SIMLIB_ContractStep = SIMLIB_MAXTIME;    //!< requested step size


////////////////////////////////////////////////////////////////////////////
/// contract step of integration
void ContractStep() {
  SIMLIB_ContractStepFlag = true;  // contract to default value (usually 1/2)
}


////////////////////////////////////////////////////////////////////////////
/// contract step of integration to end step time
/// @param time expected end of step time
void ContractStep(double time)
{ // parameter is required end step time
  SIMLIB_ContractStepFlag = true;  // flag
  double newCS = time - SIMLIB_StepStartTime;
  if (newCS<SIMLIB_ContractStep)
    SIMLIB_ContractStep = newCS;                // can be only less
  if (newCS<SIMLIB_MinStep)
    SIMLIB_ContractStep = SIMLIB_MinStep;       // minimum
}


////////////////////////////////////////////////////////////////////////////
/// \var bool SIMLIB_ResetStatus
/// flag set if there is a need for integration method restart
bool SIMLIB_ResetStatus = false;


////////////////////////////////////////////////////////////////////////////
// SetStep -- set range of step
//
void SetStep(double _dtmin, double _dtmax)
{
  SIMLIB_MinStep = _dtmin;
  SIMLIB_MaxStep = _dtmax;
  if (SIMLIB_MinStep>SIMLIB_MaxStep) SIMLIB_error(SetStepError);
//  if (SIMLIB_MinStep/SIMLIB_MaxStep < 1e-12) SIMLIB_error(SetStepError2);
//  if(MinStep/tend<1e-15) SIMLIB_error(InitMinStepError); // moznost chyby ???
  Dprintf(("SetStep: StepSize = %g .. %g ",SIMLIB_MinStep,SIMLIB_MaxStep));
}


////////////////////////////////////////////////////////////////////////////
//  SetAccuracy -- set allowed error of integration method
//
void SetAccuracy(double _abserr, double _relerr)
{
  SIMLIB_AbsoluteError = _abserr;
  if(_relerr>1) _relerr=1;   // 100% error is maximum
  SIMLIB_RelativeError = _relerr;
  if(SIMLIB_RelativeError<1e-14) SIMLIB_error(SetAccuracyError);
  Dprintf(("SetAccuracy: maxerror = %g + %g * X ",
            SIMLIB_AbsoluteError,SIMLIB_RelativeError));
}

void SetAccuracy(double relerr)
{
  SetAccuracy(0,relerr);
}


////////////////////////////////////////////////////////////////////////////
//  SIMLIB_ContinueInit -- initialize continuous subsystem
//
void SIMLIB_ContinueInit()
{
  SIMLIB_OptStep = SIMLIB_MaxStep;        // initial step size
  SIMLIB_StepStartTime = SIMLIB_Time;
  SIMLIB_DeltaTime = 0.0;
  if (IntegratorContainer::isAny()
      || StatusContainer::isAny()
      || Condition::isAny())
  { // ------------------- initialize status --------------------
    IntegratorContainer::InitAll();
    StatusContainer::InitAll();
    Condition::InitAll();       // really needed ???
    SIMLIB_Dynamic();           // initial state evaluation
    SIMLIB_DynamicFlag = true;
    Condition::TestAll();       // evaluate conditions
    SIMLIB_DynamicFlag = false;
    Condition::SetAll();        // set state
  }
}


/*************************************************/
/*****  Outline members of class Integrator  *****/
/*************************************************/


////////////////////////////////////////////////////////////////////////////
// Integrator::CtrInit -- integrator initialization
//
void Integrator::CtrInit() {
  if(SIMLIB_DynamicFlag) {
    SIMLIB_error(CantCreateIntg);  // can't in 'dynamic section' !!!
  }
  // put integrator into list & retain position of it
  it_list=IntegratorContainer::Insert(this);
  // Dprintf(("constructor: Integrator[%p]  #%d", this, Number));
  SIMLIB_ResetStatus = true; //???????????????????????????????
}


////////////////////////////////////////////////////////////////////////////
// Integrator::Integrator -- integrator creation
//

/// dummy input with zero value
Constant SIMLIB_Integrator_0input(0);

/// constructor for special cases (arrays of integrators)
/// Input can be set using method Integrator::Set
Integrator::Integrator() : input(SIMLIB_Integrator_0input)
{
  Dprintf(("Integrator[%p]::Integrator()  #%d",
           this, IntegratorContainer::Size()+1));
  CtrInit();
  initval = 0.0;
}

/// constructor
/// @param i input block expression
/// @param initvalue initial value (optional, default=0)
Integrator::Integrator(Input i, double initvalue) : input(i)
{
  Dprintf(("Integrator[%p]::Integrator(Input,%g)  #%d",
           this, initvalue, IntegratorContainer::Size()+1));
  CtrInit();
  initval = initvalue;
}


////////////////////////////////////////////////////////////////////////////
/// special copy constructor needed for i1(i2) connection
Integrator::Integrator(Integrator &i, double initvalue) :
  aContiBlock(),
  input(i)
{
  Dprintf(("Integrator[%p]::Integrator(Integrator[%p],%g) #%d",
           this, &i, initvalue, IntegratorContainer::Size()+1));
  CtrInit();
  initval = initvalue;
}


////////////////////////////////////////////////////////////////////////////
/// destructor removes integrator from list
Integrator::~Integrator()
{
  Dprintf(("destructor: Integrator[%p]  #%d",
           this, IntegratorContainer::Size()));
  if(SIMLIB_DynamicFlag) {
    SIMLIB_error(CantDestroyIntg);  // can't in 'dynamic section' !!!
  }
  IntegratorContainer::Erase(it_list);  // remove integrator from list
}


////////////////////////////////////////////////////////////////////////////
/// set initial value of integrator
void Integrator::Init(double initvalue) {
  ss = initval = initvalue;
  SIMLIB_ResetStatus = true; // if in simulation
}


////////////////////////////////////////////////////////////////////////////
/// set the integrator status value (step change)
void Integrator::Set(double value)
{
  ss = value;
  SIMLIB_ResetStatus = true;  // always
}


////////////////////////////////////////////////////////////////////////////
/// evaluate integrator input
void Integrator::Eval()
{
//  Dprintf(("START: Integrator[%p]::Eval()", this));
  dd = InputValue();
//  Dprintf(("STOP: Integrator[%p]::Eval() %g ", this, dd));
}


////////////////////////////////////////////////////////////////////////////
/// get integrator status (output value)
double Integrator::Value()
{
//  Dprintf(("Integrator[%p]::Value() = %g ", this, ss));
  return ss;
}


const char *Integrator::Name() const
{
    if (HasName())
        return _name;
    else
        return SIMLIB_create_tmp_name("Integrator{%p}", this);
}

/**********************************************************/
/*****  Outline members of class IntegratorContainer  *****/
/**********************************************************/

/// list of integrators
std::list<Integrator*>* IntegratorContainer::ListPtr=NULL;

////////////////////////////////////////////////////////////////////////////
//  IntegratorContainer::Instance
//  return pointer to list, also create list if it is not created
//
std::list<Integrator*>* IntegratorContainer::Instance(void)
{
  Dprintf(("IntegratorContainer::Instance()(%p)",ListPtr));
  if(ListPtr==NULL) {  // list is not created
    ListPtr = new std::list<Integrator*>;  // create it
    Dprintf(("created: %p", ListPtr));
  }
  return ListPtr;
} // Instance


////////////////////////////////////////////////////////////////////////////
//  IntegratorContainer::Insert
//  insert element into random (any) position in the container
//
IntegratorContainer::iterator IntegratorContainer::Insert(Integrator* ptr)
{
  Dprintf(("IntegratorContainer::Insert(%p)",ptr));
  (void)Instance();  // create list if it is not created
  return ListPtr->insert(ListPtr->end(),ptr);  // insert element
} // Insert


////////////////////////////////////////////////////////////////////////////
//  IntegratorContainer::Erase - exclude element from container
//
void IntegratorContainer::Erase(IntegratorContainer::iterator it)
{
  Dprintf(("IntegratorContainer::Erase(...)"));
  if(ListPtr!=NULL) {  // list is created
    ListPtr->erase(it);  // exclude element
  }
} // Erase


////////////////////////////////////////////////////////////////////////////
//  IntegratorContainer::NtoL
//  save statuses of blocks -- Now to Last
//
void IntegratorContainer::NtoL()
{
  Dprintf(("IntegratorContainer::NtoL()"));
  if(ListPtr!=NULL) {  // list is created
    iterator end_it=ListPtr->end();
    for(iterator ip=ListPtr->begin(); ip!=end_it; ip++) {
      (*ip)->Save();
    }
  }
} // NtoL


////////////////////////////////////////////////////////////////////////////
//  IntegratorContainer::LtoN -- restore saved statuses
//
void IntegratorContainer::LtoN()
{
  Dprintf(("IntegratorContainer::LtoN)"));
  if(ListPtr!=NULL) {  // list is created
    iterator end_it=ListPtr->end();
    for(iterator ip=ListPtr->begin(); ip!=end_it; ip++) {
      (*ip)->Restore();
    }
  }
} // LtoN


////////////////////////////////////////////////////////////////////////////
//  IntegratorContainer::InitAll() -- initialize all integrators
//
void IntegratorContainer::InitAll()
{
  Dprintf(("IntegratorContainer::InitAll)"));
  if(ListPtr!=NULL) {  // list is created
    iterator end_it=ListPtr->end();
    for(iterator ip=ListPtr->begin(); ip!=end_it; ip++) {
      (*ip)->SetState(0.0);  // zero values
      (*ip)->SetDiff(0.0);
      (*ip)->Init();
    }
  }
} // InitAll


////////////////////////////////////////////////////////////////////////////
// static IntegratorContainer::EvaluateAll -- without loop detection
//
void IntegratorContainer::EvaluateAll()
{
  Dprintf(("IntegratorContainer::EvaluateAll)"));
  if(ListPtr!=NULL) {  // list is created
    iterator end_it=ListPtr->end();
    for(iterator ip=ListPtr->begin(); ip!=end_it; ip++) {
      (*ip)->Eval();  // evaluate inputs ...
    }
  }
} // EvaluateAll


/*********************************************/
/*****  Outline members of class Status  *****/
/*********************************************/


////////////////////////////////////////////////////////////////////////////
//  Status::CtrInit -- status variable initialization
//
void Status::CtrInit() {
  if(SIMLIB_DynamicFlag) {
    SIMLIB_error(CantCreateStatus); // can't in 'dynamic section' !!!
  }
  // put status into list & retain position of it
  it_list=StatusContainer::Insert(this);
  ValueOK = false;              // important !!! ###
  Dprintf(("constructor: Status[%p]   #%d", this, StatusContainer::Size()));
  SIMLIB_ResetStatus = true;    // ??????????
}


////////////////////////////////////////////////////////////////////////////
//  Status::Status -- initialize status variable
//
Status::Status(Input i, double initvalue): aContiBlock1(i)
{
  CtrInit();
  initval = initvalue;
}


////////////////////////////////////////////////////////////////////////////
//  Status::~Status -- remove status block from list
//
Status::~Status() {
  Dprintf(("destructor: Status[%p]   #%d", this, StatusContainer::Size()));
  if(SIMLIB_DynamicFlag) {
    SIMLIB_error(CantDestroyStatus);
  }
  StatusContainer::Erase(it_list);  // remove status variable from list
}


////////////////////////////////////////////////////////////////////////////
/// set initial value
void Status::Init(double initvalue)
{
  st = initval = initvalue;
  ValueOK = false;              // important !!! ??? ###
}


////////////////////////////////////////////////////////////////////////////
/// set state variable value (step change)
//
void Status::Set(double value)
{
  st = value;
  ValueOK = false;              // important !!!
}


////////////////////////////////////////////////////////////////////////////
//  Restore -- restore saved status
//
void Status::Restore()
{
  st = stl;
  ValueOK = false;  // important !!!
}


////////////////////////////////////////////////////////////////////////////
/// value of state variable
double Status::Value()
{
  if(!ValueOK)
    _Eval();                    // if not, evaluate
  return st;                    // return status
}


const char *Status::Name() const
{
    if (HasName())
        return _name;
    else
        return SIMLIB_create_tmp_name("Status{%p}", this);
}

////////////////////////////////////////////////////////////////////////////
// virtual void Status::Eval() -- evaluate without loop detection ###
//
void Status::Eval()
{
  st = InputValue();            // input ---> output
  ValueOK = true;
}


/******************************************************/
/*****  Outline members of class StatusContainer  *****/
/******************************************************/

/// list of status variables
std::list<Status*>* StatusContainer::ListPtr=NULL;


////////////////////////////////////////////////////////////////////////////
//  StatusContainer::Instance
//  return pointer to list, also create list if it is not created
//
std::list<Status*>* StatusContainer::Instance(void)
{
  Dprintf(("StatusContainer::Instance()(%p)",ListPtr));
  if(ListPtr==NULL) {  // list is not created
    ListPtr = new std::list<Status*>;  // create it
    Dprintf(("created: %p", ListPtr));
  }
  return ListPtr;
} // Instance


////////////////////////////////////////////////////////////////////////////
//  StatusContainer::Insert
//  insert element into random (any) position in the container
//
StatusContainer::iterator StatusContainer::Insert(Status* ptr)
{
  Dprintf(("StatusContainer::Insert(%p)",ptr));
  (void)Instance();  // create list if it is not created
  return ListPtr->insert(ListPtr->end(),ptr);  // insert element
} // Insert


////////////////////////////////////////////////////////////////////////////
//  StatusContainer::Erase - exclude element from container
//
void StatusContainer::Erase(StatusContainer::iterator it)
{
  Dprintf(("StatusContainer::Erase(...)"));
  if(ListPtr!=NULL) {  // list is created
    ListPtr->erase(it);  // exclude element
  }
} // Erase


////////////////////////////////////////////////////////////////////////////
//  StatusContainer::NtoL
//  save statuses of blocks -- Now to Last
//
void StatusContainer::NtoL()
{
  Dprintf(("StatusContainer::NtoL()"));
  if(ListPtr!=NULL) {  // list is created
    iterator end_it=ListPtr->end();
    for(iterator sp=ListPtr->begin(); sp!=end_it; sp++) {
      (*sp)->Save();
    }
  }
} // NtoL


////////////////////////////////////////////////////////////////////////////
//  StatusContainer::LtoN -- restore saved statuses
//
void StatusContainer::LtoN()
{
  Dprintf(("StatusContainer::LtoN)"));
  if(ListPtr!=NULL) {  // list is created
    iterator end_it=ListPtr->end();
    for(iterator sp=ListPtr->begin(); sp!=end_it; sp++) {
      (*sp)->Restore();
    }
  }
} // LtoN


////////////////////////////////////////////////////////////////////////////
//  StatusContainer::InitAll() -- initialize all status variables
//
void StatusContainer::InitAll()
{
  Dprintf(("StatusContainer::InitAll)"));
  if(ListPtr!=NULL) {  // list is created
    iterator end_it=ListPtr->end();
    for(iterator sp=ListPtr->begin(); sp!=end_it; sp++) {
      (*sp)->SetState(0.0);  // zero values
      (*sp)->Init();
    }
  }
} // InitAll


////////////////////////////////////////////////////////////////////////////
// static StatusContainer::EvaluateAll -- with loop detection
//
void StatusContainer::EvaluateAll()
{
  Dprintf(("StatusContainer::EvaluateAll)"));
  if(ListPtr!=NULL) {  // list is created
    iterator end_it=ListPtr->end();
    for(iterator sp=ListPtr->begin(); sp!=end_it; sp++) {
      (*sp)->_Eval();  // evaluate inputs with loop detection
    }
  }
} // EvaluateAll


////////////////////////////////////////////////////////////////////////////
//  StatusContainer::ClearAllValueOK
//  invalidate values of all status variables
//
void StatusContainer::ClearAllValueOK()
{
  Dprintf(("StatusContainer::EvaluateAll)"));
  if(ListPtr!=NULL) {  // list is created
    iterator end_it=ListPtr->end();
    for(iterator sp=ListPtr->begin(); sp!=end_it; sp++) {
      (*sp)->SetValid(false);  // invalidate values
    }
  }
} // EvaluateAll


}
// end

