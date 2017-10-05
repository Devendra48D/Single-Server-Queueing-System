/////////////////////////////////////////////////////////////////////////////
//! \file run.cc   Simulation control algorithm
//
// Copyright (c) 1991-2008 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
// description: control of simulation
//


////////////////////////////////////////////////////////////////////////////
// interface
////////////////////////////////////////////////////////////////////////////

#include "simlib.h"
#include "internal.h"
#include <cstdlib> // exit()


////////////////////////////////////////////////////////////////////////////
// implementation
////////////////////////////////////////////////////////////////////////////

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

////////////////////////////////////////////////////////////////////////////
//  SIMLIB global variables  ### move to extra module!!!
//
//  SIMLIB_*  --- internal variables
//  [A-Z]*    --- user-level variables
//


// time-related variables
double SIMLIB_StartTime;       // time of simulation start
double SIMLIB_Time;            // simulation time
double SIMLIB_NextTime;        // next-event time
double SIMLIB_EndTime;         // time of simulation end

// read-only references to time variables
// ASSERTION: StartTime <= Time <= NextTime <= EndTime
const double & StartTime = SIMLIB_StartTime;    // time of simulation start
const double & Time      = SIMLIB_Time;         // simulation time
const double & NextTime  = SIMLIB_NextTime;     // next-event time
const double & EndTime   = SIMLIB_EndTime;      // time of simulation end

// current entity pointer
Entity *SIMLIB_Current = NULL;
Entity *const &Current = SIMLIB_Current;        // read-only reference

// phase of simulation experiment
SIMLIB_Phase_t SIMLIB_Phase = START;
const SIMLIB_Phase_t & Phase = SIMLIB_Phase;    // read-only reference

// experiment counter
unsigned long SIMLIB_experiment_no = 0;

////////////////////////////////////////////////////////////////////////////
/// internal statistical information
SIMLIB_statistics_t::SIMLIB_statistics_t() {
    Init();
}
void SIMLIB_statistics_t::Init() {
    StepCount = 0;
    MinStep = -1;
    MaxStep = -1;
    EventCount = 0;
    StartTime = -1;
    EndTime = -1;
}

static SIMLIB_statistics_t SIMLIB_run_statistics;
const SIMLIB_statistics_t &SIMLIB_statistics = SIMLIB_run_statistics;

////////////////////////////////////////////////////////////////////////////
// private module variables

static bool StopFlag = false;           // if set, stop simulation run

////////////////////////////////////////////////////////////////////////////
// support for Delay blocks (internal)
//
DEFINE_HOOK(Delay);     // called in Run() and SampleDelays()
DEFINE_HOOK(DelayInit); // called in Init()

// for explicit sampling
void SampleDelays() { // used at step-wise changes !!!###
  CALL_HOOK(Delay);
}

// ZDelays:
DEFINE_HOOK(ZDelayTimerInit); // called in Run()

////////////////////////////////////////////////////////////////////////////
// support for simulation interrupt (user-level)
//
DEFINE_HOOK(Break);     // called in Run()

void InstallBreak(void (*f)()) { // for user interface (in simlib.h)
    INSTALL_HOOK( Break, f );
}

////////////////////////////////////////////////////////////////////////////
// support for Samplers (internal)
//
DEFINE_HOOK(SamplerAct);        // called in Run()
DEFINE_HOOK(SamplerInit);       // called in Init()


////////////////////////////////////////////////////////////////////////////
// WUclear hook --- if non-empty WUlist, call init function
//
DEFINE_HOOK(WUclear); // should be in run.cc

////////////////////////////////////////////////////////////////////////////
// SIMLIB_WUClear --- remove all items in WaitUntil list
//
void SIMLIB_WUClear() // should be removed -- ise CALL_HOOK instead
{
    CALL_HOOK(WUclear);
}

////////////////////////////////////////////////////////////////////////////
// special WaitUntil processing
DEFINE_HOOK(WUget_next);
////////////////////////////////////////////////////////////////////////////
// SIMLIB_DoActions --- central calling of interruptable procedures
// WARNING: SIMLIB_Current->_Run() should be called from this place only!
//
void SIMLIB_DoActions()
{
  do {
    SIMLIB_Current->_Run(); // perform event-dispatch
    SIMLIB_Current = 0;
    CALL_HOOK(WUget_next);  // check and activate next in WUlist
  }while( SIMLIB_Current != 0 );
}

////////////////////////////////////////////////////////////////////////////
//  Stop --- break the simulation run, can continue with another run
//
void Stop()
{
    Dprintf(("\n ********************* STOP *********************\n"));
    if( SIMLIB_Phase != SIMULATION ) // if runs a simulation experiment
        SIMLIB_error(SFunctionUseError);
    StopFlag = true;
}


////////////////////////////////////////////////////////////////////////////
//  Abort --- end of simulation program
//
void Abort()
{
    Dprintf(("\n ********************* ABORT *********************\n"));
    SIMLIB_Phase = ERROREXIT;
    std::exit(1); // end of program, errorcode 1
}


////////////////////////////////////////////////////////////////////////////
//  SIMLIB_Init --- initialization of simulation system and some parts of
//                  model; some checks
//  initialize:
//   - Time, StartTime, EndTime
//   - calendar (SQS)
//   - WaitUntilList
//   - existing continuous blocks, ...
//   - Delay blocks
//
void SIMLIB_Init(double T0, double T1, unsigned version)
{
  ++SIMLIB_experiment_no;
  Dprintf(("\n\t ************************* Init(%g,%g) #%lu \n",
           T0, T1, SIMLIB_experiment_no));
  // first some checks
  if(version != SIMLIB_version) {  // check versions
    Dprintf(("\n SIMLIB library version %x.%02x ",
              SIMLIB_version >> 8, SIMLIB_version & 0xFF));
    Dprintf((" SIMLIB header version %x.%02x \n",
              version >> 8, version & 0xFF));
    SIMLIB_error(InconsistentHeader);  // exit program
  }
  if( SIMLIB_Phase == INITIALIZATION ) SIMLIB_error(TwiceInitError);
  if( SIMLIB_Phase == SIMULATION ) SIMLIB_error(InitInRunError);
  SIMLIB_Phase = INITIALIZATION;
  /////////////////////////////////////////////////////////////////
  if( T0 < SIMLIB_MINTIME ) SIMLIB_error(InitError);
  if( T1 > SIMLIB_MAXTIME ) SIMLIB_error(InitError);
  if( T0 >= T1 )            SIMLIB_error(InitError);
  /////////////////////////////////////////////////////////////////
  // set simulation parameters
  _SetTime(StartTime,T0);
  _SetTime(Time,T0);
  _SetTime(EndTime,T1);

// set reasonable defaults for Step limits ???
// if not set by user first ### add flag to SetStep
//  MaxStep = tend/100;
//  MinStep = MaxStep/1000;

  // TODO: add SIMLIB_RunNumber++
  // TODO: add real-time stderr/cerr output message for long computations?
  //       on-demand print of "RunNumber:Time   ETArun:minutes" each minute?

  // ADD list of functions to call: (dynamically!, priority)
  // something like atexit(): atInit, atRunStart, atRunEnd

  SQS::Clear();                 // initialize calendar
  SIMLIB_WUClear();             // initialize WaitUntilList
  SIMLIB_ContinueInit();        // initialize status variables 1 ###

  CALL_HOOK(SamplerInit);       // initialize all Samplers
  CALL_HOOK(DelayInit);         // initialize all Delays

}


////////////////////////////////////////////////////////////////////////////
// Run --- main simulation control
//
void Run() {
  Dprintf(("\n\t ********** Run() --- START \n"));

  // first some checks
  if( SIMLIB_Phase != INITIALIZATION )
      SIMLIB_error(RunUseError); // bad use of Run()
  if( NextTime < StartTime )
      SIMLIB_internal_error();   // never reached

  // welcome to the SIMLIB simulation control algorithm :-)

  // initialize variables
  SIMLIB_Phase = SIMULATION;
  StopFlag = false;               // flag for stop simulation

  SIMLIB_run_statistics.Init();       // initialize internal statistics
  SIMLIB_run_statistics.StartTime = Time;

  // call init functions
  SIMLIB_ContinueInit();          // initialize status variables 2 ###

  CALL_HOOK(ZDelayTimerInit);     // activate all ZDelayTimers
  CALL_HOOK(SamplerAct);          // activate all Samplers
  CALL_HOOK(Break);               // user can stop simulation by any key?

// TODO: try using special lowest priority end-event to stop simulation
//       It should be simpler

  // main loop
  while( Time < EndTime && !StopFlag )  {
      int endFlag = NextTime > EndTime; // if no event at end time
      if( endFlag )
          _SetTime( NextTime, EndTime ); // limit NextTime to EndTime

      if( Time < NextTime )  {  // no event at current Time
          if( IntegratorContainer::isAny() || StatusContainer::isAny() ) {
              // there are integrators or status variables, so we enter
              // -------------- CONTINUOUS SIMULATION ---------------
              SIMLIB_ResetStatus = true;   // don't use previous step buffers
                                           // TODO: is it really needed always?
              CALL_HOOK(Delay);            // DELAY: sample input
              while( Time < NextTime )  {  // do continuous steps
                                           // until scheduled event or end ...
                  IntegrationMethod::StepSim(); // *** continuous step ***

                  SIMLIB_run_statistics.StepCount++; // some runtime statistics
                  if(SIMLIB_run_statistics.MinStep<0) {
                      SIMLIB_run_statistics.MinStep = StepSize;
                      SIMLIB_run_statistics.MaxStep = StepSize;
                  } else if(SIMLIB_run_statistics.MinStep>StepSize)
                      SIMLIB_run_statistics.MinStep = StepSize;
                  else if(SIMLIB_run_statistics.MaxStep<StepSize)
                      SIMLIB_run_statistics.MaxStep = StepSize;

                  SIMLIB_DoConditions();   // perform state events
                  CALL_HOOK(Delay);        // DELAY: sample input at each step
                  CALL_HOOK(Break); // user can stop simulation by any key?
                                    // TODO: use signal handler, ^C=SIGINT
                  if(StopFlag)
                      break;        // end of simulation run was required
                                    // by a state-event
              } // while continuous steps
              // _SetTime( Time, NextTime ); // set next event activation time
              // ^^^^^^^^^^^^^^^^^^^^^^^^^^^ should be in StepSim()
          } else { // no integrators, status blocks, ...
              _SetTime( Time, NextTime ); // set next event activation time
          }
      } // if (NextTime>Time)

      // assert(is event in Time OR end of simulation)
      if( endFlag )  break; // end of simulation if no event at endtime
      ///////////// (TODO: ###BUG? state-conditions can schedule!)

      while( Time >= NextTime && !StopFlag && !SQS::Empty() ) {
          // there are events scheduled at current Time
          // >= because of rounding errors
          SIMLIB_Current = SQS::GetFirst(); // get first record from calendar
          SIMLIB_DoActions();  // perform actions (see waitunti.cc)
          SIMLIB_run_statistics.EventCount++;   // internal statistics
          // assert: SIMLIB_Current is NULL
          CALL_HOOK(Break); // Callback: user can stop simulation by key or GUI
        }
  } // main loop
  IntegrationMethod::IntegrationDone(); // terminate integration run
  SIMLIB_Phase = TERMINATION;
  SIMLIB_run_statistics.EndTime = Time;
  Dprintf(("\n\t ********** Run() --- END \n"));
}

} // namespace

