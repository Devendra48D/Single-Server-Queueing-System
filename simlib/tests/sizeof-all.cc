// sizeof-all.cc
// -------------------------------------------------------------------

#include "simlib.h"
#include <iostream>
using namespace std;

int main() {

  cout << "Simlib internals" << endl ;
  cout << "----------------" << endl << endl ;

  cout << "SIMLIB Version  = " << SIMLIB_version_string() << endl ;
  cout << "SIMLIB System   = " << SIMLIB_SYSTEM << endl ;
  cout << "SIMLIB Compiler = " << SIMLIB_COMPILER << endl ;

  cout << "SIMLIB MinTime = " << SIMLIB_MINTIME << endl ;
  cout << "SIMLIB MaxTime = " << SIMLIB_MAXTIME << endl ;

  cout << "SIMLIB default MinStep = " << MinStep << endl ;
  cout << "SIMLIB default MaxStep = " << MaxStep << endl ;

  cout << "SIMLIB default AbsoluteError = " << AbsoluteError << endl ;
  cout << "SIMLIB default RelativeError = " << RelativeError << endl ;

  cout << "SIMLIB priority = " << LOWEST_PRIORITY <<
          " .. " << HIGHEST_PRIORITY <<
          ", (default = " << DEFAULT_PRIORITY << ")" << endl ;

#define PRINT_SIZE(x) cout << "\n  sizeof(" #x ") = " << sizeof(x)

  PRINT_SIZE(void*);
  PRINT_SIZE(SimObject);
// TODO: generate from simlib.h
  PRINT_SIZE(Link) << ",  parent = SimObject" ;
  PRINT_SIZE(Entity) << ",  parent = Link" ;
  PRINT_SIZE(Process) << ",  parent = Entity" ;
  PRINT_SIZE(Event) << ",  parent = Entity" ;
  PRINT_SIZE(Sampler) << ",  parent = Event" ;
  PRINT_SIZE(Stat) << ",  parent = SimObject" ;
  PRINT_SIZE(TStat) << ",  parent = SimObject" ;
  PRINT_SIZE(List) << ",  parent = SimObject" ;
  PRINT_SIZE(Queue) << ",  parent = List" ;
  PRINT_SIZE(Histogram) << ",  parent = SimObject" ;
  PRINT_SIZE(Facility) << ",  parent = SimObject" ;
  PRINT_SIZE(Store) << ",  parent = SimObject" ;
  PRINT_SIZE(aBlock) << ",  parent = SimObject" ;
  PRINT_SIZE(aContiBlock) << ",  parent = aBlock" ;
  PRINT_SIZE(Constant) << ",  parent = aContiBlock" ;
  PRINT_SIZE(Variable) << ",  parent = aContiBlock" ;
  PRINT_SIZE(Parameter) << ",  parent = aContiBlock" ;
  PRINT_SIZE(Input) ;
  PRINT_SIZE(aContiBlock1) << ",  parent = aContiBlock" ;
  PRINT_SIZE(Expression) << ",  parent = aContiBlock1" ;
  PRINT_SIZE(aContiBlock2) << ",  parent = aContiBlock" ;
  PRINT_SIZE(aContiBlock3) << ",  parent = aContiBlock" ;
  PRINT_SIZE(IntegrationMethod) ;
  PRINT_SIZE(SingleStepMethod) << ",  parent = IntegrationMethod" ;
  PRINT_SIZE(MultiStepMethod) << ",  parent = IntegrationMethod" ;
  PRINT_SIZE(StatusMethod) << ",  parent = SingleStepMethod" ;
  PRINT_SIZE(Integrator) << ",  parent = aContiBlock" ;
  PRINT_SIZE(Status) << ",  parent = aContiBlock1" ;   // state-variables
  PRINT_SIZE(Hyst) << ",  parent = Status" ;
  PRINT_SIZE(Blash) << ",  parent = Status" ;
  PRINT_SIZE(Relay) << ",  parent = Status" ;
  PRINT_SIZE(Function1) << ",  parent = aContiBlock1" ;
  PRINT_SIZE(Function2) << ",  parent = aContiBlock2" ;
  PRINT_SIZE(aCondition) << ",  parent = aBlock" ;
  PRINT_SIZE(Condition) << ",  parent = aCondition" ; // state condition
  PRINT_SIZE(ConditionUp) << ",  parent = Condition" ;
  PRINT_SIZE(ConditionDown) << ",  parent = Condition" ;
  PRINT_SIZE(Lim) << ",  parent = aContiBlock1" ;
  PRINT_SIZE(Insv) << ",  parent = aContiBlock1" ;
  PRINT_SIZE(Qntzr) << ",  parent = aContiBlock1" ;
  PRINT_SIZE(Frict) << ",  parent = aContiBlock1" ;
  PRINT_SIZE(Rline) << ",  parent = aContiBlock1" ;
  PRINT_SIZE(AlgLoop) << ",  parent = aContiBlock1" ;
  PRINT_SIZE(Iterations) << ",  parent = AlgLoop" ;
  PRINT_SIZE(Bisect) << ",  parent = AlgLoop" ;
  PRINT_SIZE(RegulaFalsi) << ",  parent = AlgLoop" ;
  PRINT_SIZE(Newton) << ",  parent = AlgLoop" ;
  PRINT_SIZE(Semaphore) << ",  parent = SimObject" ;
  PRINT_SIZE(Barrier) << ",  parent = SimObject" ;

  cout << endl ;
}

// end of SIMLIB.H


