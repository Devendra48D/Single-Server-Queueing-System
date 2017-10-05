//
// errors.cc
//
//
//
//
/* Generated from file 'errors.txt' by program GENERR */

#include "simlib.h"
namespace simlib3 {
#include "errors.h"

static char _Errors[] = {
/* 0 */ "SIMLIB/C++ Simulation Library, " SIMLIB_COPYRIGHT "\0"
/* 1 */ "Undocumented error\0"
/* 2 */ "Internal error\0"
/* 3 */ "No memory\0"
/* 4 */ "Bad Init() arguments\0"
/* 5 */ "Init() called twice before Run()\0"
/* 6 */ "Init() can not be called during simulation run\0"
/* 7 */ "Run() should be called after Init()\0"
/* 8 */ "Bad SetStep(min,max) arguments\0"
/* 9 */ "Requested integration step is too small\0"
/* 10 */ "SetStep: Too big difference of min/max step\0"
/* 11 */ "SetAccuracy: Too small relative accuracy requested\0"
/* 12 */ "Special function called and simulation is not running\0"
/* 13 */ "Numerical integration error greater than requested\0"
/* 14 */ "Bad reference to list item\0"
/* 15 */ "Deleted item is linked in some list\0"
/* 16 */ "Removed item not in list\0"
/* 17 */ "Calendar should be singleton\0"
/* 18 */ "Deleting active item in calendar\0"
/* 19 */ "Scheduling before current Time\0"
/* 20 */ "Calendar is empty\0"
/* 21 */ "Procesis is not initialized\0"
/* 22 */ "Bad histogram step (step<=0)\0"
/* 23 */ "Bad histogram interval count (max=10000)\0"
/* 24 */ "List does not have active item\0"
/* 25 */ "Empty list\0"
/* 26 */ "Bad queue reference\0"
/* 27 */ "Empty WaitUntilList - can't Get() (internal error)\0"
/* 28 */ "Bad entity reference\0"
/* 29 */ "Entity not scheduled\0"
/* 30 */ "Time statistic not initialized\0"
/* 31 */ "Can't create new integrator in dynamic section\0"
/* 32 */ "Can't destroy integrator in dynamic section\0"
/* 33 */ "Can't create new status variable in dynamic section\0"
/* 34 */ "Can't destroy status variable in dynamic section\0"
/* 35 */ "Seize(): Can't interrupt facility service\0"
/* 36 */ "Release(): Facility is released by other than currently serviced process\0"
/* 37 */ "Release(): Can't release empty facility\0"
/* 38 */ "Enter() request exceeded the store capacity\0"
/* 39 */ "Leave() leaves more than currently used\0"
/* 40 */ "SetCapacity(): can't reduce store capacity\0"
/* 41 */ "SetQueue(): deleted (old) queue is not empty\0"
/* 42 */ "Weibul(): lambda<=0.0 or alfa<=1.0\0"
/* 43 */ "Erlang(): beta<1\0"
/* 44 */ "NegBin(): q<=0 or k<=0\0"
/* 45 */ "NegBinM(): m<=0\0"
/* 46 */ "NegBinM(): p not in range 0..1\0"
/* 47 */ "Poisson(lambda): lambda<=0\0"
/* 48 */ "Geom(): q<=0\0"
/* 49 */ "HyperGeom(): m<=0\0"
/* 50 */ "HyperGeom(): p not in range 0..1\0"
/* 51 */ "Can't write output file\0"
/* 52 */ "Output file can't be open between Init() and Run()\0"
/* 53 */ "Can't open output file\0"
/* 54 */ "Can't close output file\0"
/* 55 */ "Algebraic loop detected\0"
/* 56 */ "Parameter low>=high\0"
/* 57 */ "Parameter of quantizer <= 0\0"
/* 58 */ "Library and header (simlib.h) version mismatch \0"
/* 59 */ "Semaphore::V() -- bad call\0"
/* 60 */ "Uniform(l,h) -- bad arguments\0"
/* 61 */ "Stat::MeanValue()  No record in statistics\0"
/* 62 */ "Stat::Disp()  Can't compute (n<2)\0"
/* 63 */ "AlgLoop: t_min>=t_max\0"
/* 64 */ "AlgLoop: t0 not in  <t_min,t_max>\0"
/* 65 */ "AlgLoop: method not convergent\0"
/* 66 */ "AlgLoop: iteration limit exceeded\0"
/* 67 */ "AlgLoop: iterative block is not in loop\0"
/* 68 */ "Unknown integration method\0"
/* 69 */ "Integration method name not unique\0"
/* 70 */ "Integration step <=0\0"
/* 71 */ "Start-method is not single-step\0"
/* 72 */ "Method is not multi-step\0"
/* 73 */ "Can't switch methods in dynamic section\0"
/* 74 */ "Can't switch start-methods in dynamic section\0"
/* 75 */ "Rline: argument n<2\0"
/* 76 */ "Rline: array is not sorted\0"
/* 77 */ "Library compiled without debugging support\0"
/* 78 */ "Dealy is too small (<=MaxStep)\0"
/* 79 */ "Parameter can not be changed during simulation run\0"
/* 80 */ "General error\0"
};

char *_ErrMsg(enum _ErrEnum N)
{
  char *p = _Errors;
  int i = N;
  while( i-- > 0 )
    while( *p++ != '\0' ) { /*empty*/ }
  return p;
};
} // namespace simlib3

