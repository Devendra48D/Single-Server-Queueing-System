/////////////////////////////////////////////////////////////////////////////
//! \file internal.h  Internal header file for SIMLIB/C++
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

/**
 * @mainpage SIMLIB
 * @author Petr Peringer
 * @author David Martinek (fuzzy subsystem)
 * @author David Le¹ka (numerical integration subsystem)
 *
 * SIMLIB/C++ is the SIMulation LIBrary for C++ programming language
 *
 * \section intro Introduction
 *
 * This documentation is experimental for now.
 * It is made by program doxygen and is not complete.
 * Fuzzy extension documentation is not generated.
 *
 */

//! \example  _test_.cc
//! This is simple example for basic testing of SIMLIB/C++.
//! <br> Usage: make test


//
// SIMLIB internal declarations
//

#ifndef __SIMLIB__INTERNAL_H__
#define __SIMLIB__INTERNAL_H__

#ifndef __SIMLIB__
#  error "simlib.h should be included first"
#endif

namespace simlib3 {

////////////////////////////////////////////////////////////////////////////
//! Class for internal registration of modules.
class SIMLIB_module { // internal class for SIMLIB cleanup (atexit)
  const char *string; // module identification string
  SIMLIB_module(const SIMLIB_module&); // disable copy-ctr
  SIMLIB_module &operator= (const SIMLIB_module&); // disable =
 public:
  SIMLIB_module();
  int Init(const char *s);
  ~SIMLIB_module(); // last can do ATEXIT ?
};

////////////////////////////////////////////////////////////////////////////
//! Create single global registration object in each SIMLIB module.
static SIMLIB_module SIMLIB_module_id;

//! values indicate current phase of experiment
enum SIMLIB_Phase_t {
    START,          // before first Init() call
    INITIALIZATION, // after Init() call
    SIMULATION,     // inside Run() call
    TERMINATION,    // after Run() call
    ERROREXIT       // fatal error handling phase
};
//! This variable contains the current phase of experiment
//! (used for internal checking)
extern const SIMLIB_Phase_t &Phase;

////////////////////////////////////////////////////////////////////////////
// debugging ...
//TODO: minimize
//#define NDEBUG   // uncomment if you don't want to compile debug info

#ifdef NDEBUG
#   define Dprintf(s)
#   define DEBUG(c,s)
#   define DEBUG_INFO
#else
    extern double SIMLIB_Time;            // simulation time
#   define DEBUG_INFO "/debug"
    extern unsigned long SIMLIB_debug_flag; // debugging flags
#   define Dprintf(f) \
        do { if( SIMLIB_debug_flag ) \
                 { _Print("DEBUG: T=%-10g ", SIMLIB_Time); \
                   _Print f; _Print("\n"); \
        } }while(0)
#   define DEBUG(c,f) \
    do{ if( SIMLIB_debug_flag & (c) ) \
            { _Print("DEBUG: T=%-10g ", SIMLIB_Time); \
              _Print f; _Print("\n"); \
    } }while(0)
    // classification of DEBUG messages
#   define DBG_ALL         ~0UL          // print all debugging info
#   define DBG_NEW         (1UL)         // new/delete (memory allocation)
#   define DBG_CTR         (1UL<<1)      // ctr/dtr (creation/destruction)
#   define DBG_INIT        (1UL<<2)      // initialization
#   define DBG_CHG         (1UL<<3)      // object changes
#   define DBG_STEP        (1UL<<4)      // continuous step control
#   define DBG_CONTI       (1UL<<5)      // continuous blocks
#   define DBG_FACSTO      (1UL<<6)      // Enter,Leave,Seize,Release
#   define DBG_PROCESS     (1UL<<7)      // Process
#   define DBG_QUEUE       (1UL<<8)      // Queue operations
#   define DBG_CALENDAR    (1UL<<9)      // Scheduling
#   define DBG_SIMULATOR   (1UL<<10)     // Simulator actions - control
#   define DBG_BEXPR       (1UL<<11)     // Block expressions
#   define DBG_WU          (1UL<<12)     // WaitUntil
#   define DBG_NUMINT      (1UL<<13)     // numerical integration methods
#   define DBG_THREAD      (1UL<<14)     // Process-switching
#   define DBG_MODULE      (1UL<<15)     // Module cleanup: id_string,...
#   define DBG_ATEXIT      (1UL<<16)     // SIMLIB_atexit
#endif // NDEBUG


////////////////////////////////////////////////////////////////////////////
/// \def SIMLIB_IMPLEMENTATION
/// internal module identification
#define SIMLIB_IMPLEMENTATION                            \
   _Pragma("GCC diagnostic push")                        \
   _Pragma("GCC diagnostic ignored \"-Wunused-variable\"") \
   static int SIMLIB_module_num =                        \
      SIMLIB_module_id.Init(                             \
        "(" __FILE__                                     \
        ", SIMLIB-" SIMLIB_VERSION  DEBUG_INFO           \
        ", " SIMLIB_SYSTEM                               \
        "/" SIMLIB_COMPILER                              \
        ", " __DATE__ " " __TIME__                       \
        ")"                                              \
      );                                                 \
   _Pragma("GCC diagnostic pop") 

// SIMLIB atexit function (for internal use only)
typedef void (*SIMLIB_atexit_function_t)();
void SIMLIB_atexit(SIMLIB_atexit_function_t p);

////////////////////////////////////////////////////////////////////////////
// error handling functions
//

#include "errors.h" // error messages (generated by program generr)

/// print specific error messages
void SIMLIB_error(const enum _ErrEnum ErrMsgNum);

/// print formatted error messages
void SIMLIB_error(const char *fmt, ... );

/// print internal error messages
void SIMLIB_error(const char *file, const int line);
#define SIMLIB_internal_error() SIMLIB_error(__FILE__, __LINE__)

/// print warnings
void SIMLIB_warning(const enum _ErrEnum ErrMsgNum);
/// print warnings
void SIMLIB_warning(const char *fmt, ... );

////////////////////////////////////////////////////////////////////////////
// types:
//
typedef void (*VoidFunctionPtr)(); // ptr to void function


////////////////////////////////////////////////////////////////////////////
// internal variables:
//

extern bool SIMLIB_DynamicFlag;             // in dynamic section
extern bool SIMLIB_ResetStatus;             // restart flag

extern SIMLIB_Phase_t SIMLIB_Phase;         // phase of simulation experiment

extern Entity *SIMLIB_Current;              // currently active entity

extern int SIMLIB_ERRNO;                    // error number

extern bool SIMLIB_ConditionFlag;           // change of condition vector
extern bool SIMLIB_ContractStepFlag;        // requests shorter step
extern double SIMLIB_ContractStep;          // requested step size

extern double SIMLIB_StepStartTime;         // last step time
extern double SIMLIB_DeltaTime;             // Time-s_StepStartTime

extern double SIMLIB_OptStep;               // optimal step
extern double SIMLIB_MinStep;               // minimal step
extern double SIMLIB_MaxStep;               // max. step
extern double SIMLIB_StepSize;              // actual step

extern double SIMLIB_AbsoluteError;         // absolute error tolerance
extern double SIMLIB_RelativeError;         // relative error

extern double SIMLIB_StartTime;       // time of simulation start
extern double SIMLIB_Time;            // simulation time
extern double SIMLIB_NextTime;        // next-event time
extern double SIMLIB_EndTime;         // time of simulation end

// TODO: move to context (public methods with prefix calendar::?)

//! Special namespace for calendar implementation.
//!
//! This is for internal use only.
namespace SQS {
    void ScheduleAt(Entity *e, double t);// time t
    Entity *GetFirst();                  // remove first item
    void Get(Entity *e);                 // remove entity e
    bool Empty();                        // ?empty calendar
    void Clear();                        // remove all items
    int debug_print();
};

/// macro for simple assignement to internal time variables
#define _SetTime(t,x) (SIMLIB_##t = x)

void SIMLIB_Dynamic();               // optimize!
void SIMLIB_DoActions();             // dispatch events and processes
void SIMLIB_ContinueInit();          // initialize variables
void SIMLIB_DoConditions();          // perform state events
void SIMLIB_WUClear();               // clear WUList


//////////////////////////////////////////////////////////////////////////
// MACROS --- Hooks into simulation control algorithm
//
// we use static pointers to void function()
// function can be installed by calling INSTALL_HOOK(hook_name,function)
// used mainly in run.cc

// definition of hook-pointer-variable name and hook-install-function name
// the names are internal and _can_ be changed here:
#define HOOK_PTR_NAME(id)  SIMLIB_Hook_Ptr_##id
#define HOOK_INST_NAME(id)  SIMLIB_Install_hook_##id

/////////////////////////////////////////////////////////////////////////////
// INSTALL_HOOK --- macro for hook function installation
// parameters:
//      name of hook
//      function to install as hook -- prototype should be: void f();
//
#define INSTALL_HOOK(name,function) do {\
        void HOOK_INST_NAME(name) (void (*f)()); /* prototype */ \
        HOOK_INST_NAME(name)(function);  /* call of installer */ \
        }while (0)


/////////////////////////////////////////////////////////////////////////////
// DEFINE_HOOK --- macro for hook definition
// parameter:
//      name of hook
// can be used at global scope
//
#define DEFINE_HOOK(name)  \
        static void (* HOOK_PTR_NAME(name) )() = 0; \
        void HOOK_INST_NAME(name)(void (*f)())  { HOOK_PTR_NAME(name) = f; }


/////////////////////////////////////////////////////////////////////////////
// CALL_HOOK --- macro for checked hook calling
// parameter:
//      name of hook
// can be used in the same module as DEFINE_HOOK (static)
//
#define CALL_HOOK(name)  \
        if( HOOK_PTR_NAME(name) )  HOOK_PTR_NAME(name) ()


////////////////////////////////////////////////////////////////////////////
//  auxiliary functions TODO: ### remove, use std version
//
inline double min(double a, double b) { return ((a)>(b)?(b):(a)); }
inline double max(double a, double b) { return ((a)>(b)?(a):(b)); }


////////////////////////////////////////////////////////////////////////////
//! internal method for flag manipulation
/// is used for:
/// - algebraic loop checking in continuous blocks
///
inline bool SimObject::TestAndSetFlag(bool new_value, unsigned n) {
    bool old_value = (_flags >> n) & 1U;
    if(new_value)
        _flags |=  (1U << n);
    else
        _flags &= ~(1U << n);
    return old_value;
}

////////////////////////////////////////////////////////////////////////////
//! class for algebraic loop detection
/// AlgLoopDetector object should be used in Value() method only
/// it checks for recursion in continuous block expression evaluation <br>
/// User should remove algebraic loop manualy using special block (see AlgLoop solvers)
class AlgLoopDetector {
    aContiBlock *block;
    /// begin checking for recursion in Value() calling
    void lock() {
        if(block->TestAndSetFlag(true,SimObject::_EVAL_FLAG))  // was evaluated
          SIMLIB_error(AlgLoopDetected);           // recursive call detected
    }
    /// end checking for recursion
    void unlock() {
        block->TestAndSetFlag(false,SimObject::_EVAL_FLAG);
    }
  public:
    AlgLoopDetector(aContiBlock *x): block(x)  { lock(); }
    ~AlgLoopDetector() { unlock(); }
};


////////////////////////////////////////////////////////////////////////////
// NAME subsystem - experimental
const char *SIMLIB_create_tmp_name(const char *fmt, ...);

} // namespace

#endif //__SIMLIB__INTERNAL_H__

// end

