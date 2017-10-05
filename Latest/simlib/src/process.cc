/////////////////////////////////////////////////////////////////////////////
//! \file process.cc  Implementation of quasiparallel processes
//!
//! This module contains implementation of cooperative multitasking processes
//! for discrete simulation

//
// Copyright (c) 1991-2007 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  Implementation of interruptable functions (non-preemptive threads/coroutines)
//
//  This module is the only nonportable module in SIMLIB
//  We need code to save/restore process stack contents and working setjmp/longjmp
//  This approach has advantage in small memory requirements.
//
//  Supported CPU architectures: i386+, x86_64
//  NOTE: on Sparc it does not work (for now)
//
//  WARNING: dirty hack inside
//

// TODO: add implementation with stack switching (not copying)
//       as compile-time option
//       mark SP, alloc stack [size=runtime-option], change SP, call global function without
//       params/locals, call Current->Behavior (uses new stack for this)
//       return: set SP back, ...
//       Process destructor: free stack
// TODO: add implementation using preemptive threads (Boost)


////////////////////////////////////////////////////////////////////////////
// interface
//

#include "simlib.h"
#include "internal.h"

#include <csetjmp>
#include <cstring>

// basic operating system test
#if !(defined(__MSDOS__)||defined(__linux__)|| \
      defined(__WIN32__)||defined(__FreeBSD__))
# error "module process.cc is not implemented on this operating system"
#endif

// basic CPU test
#if !(defined(__i386__)||defined(__x86_64__))
# error "module process.cc is not ported to this processor architecture"
#endif


////////////////////////////////////////////////////////////////////////////
// implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

////////////////////////////////////////////////////////////////////////////
// Machine dependent macros for direct stack pointer manipulation:
//
// _GET_STACK_PTR(v)   v = <stack pointer>
// _SET_STACK_PTR(v)   <stack pointer> = v
//
/////////////////////////////////////////////////////////////////////////////
#if defined(__BCPLUSPLUS__) || defined(__TURBOC__)  // Borland compilers
/////////////////////////////////////////////////////////////////////////////

// this works on i386+ platforms
# if defined(__WIN32__)         // 32 bit WINDOWS
#   define _GET_STACK_PTR(v)    { v = (char*)_ESP; }
#   define _SET_STACK_PTR(v)    { _ESP = v; }
# else                          // 16 bit MSDOS (obsolete, not supported)
#   if defined(__LARGE__) || defined(__COMPACT__) || defined(__HUGE__)
#     define _GET_STACK_PTR(v)  { v = (char far *)MK_FP(_SS,_SP); }
#   else
#     define _GET_STACK_PTR(v)  { v = (char*)_SP; }
#   endif
#   define _SET_STACK_PTR(v)  { _SP = v; }
# endif

/////////////////////////////////////////////////////////////////////////////
#elif defined(__GNUC__)         // GNU C++ compiler
/////////////////////////////////////////////////////////////////////////////

# if defined(__sparc__)         // Sparc processor

#   error "this code for SPARC doesn't work yet"
#   define _GET_STACK_PTR(v)    { asm("st %%sp, [ %0 ]" : "=m" (v)); }
#   define _SET_STACK_PTR(v)    { asm("ld [ %0 ], %%sp" : "=r" (v)); }

# elif defined(__i386__)        // 32bit: i386+ expected...

#   define _GET_STACK_PTR(v)    { asm("movl %%esp,%0":"=r" (v)); }
#   define _SET_STACK_PTR(v)    { asm("movl %0,%%eax": : "m" (v)); \
                                  asm("movl %eax,%esp");                     \
                                }
# elif defined(__x86_64__)      // 64bit: Athlon64, ...

#   define _GET_STACK_PTR(gvar) { asm("movq %%rsp,%0": "=r" (gvar)); }
#   define _SET_STACK_PTR(gvar) { asm("movq %0,%%rsp": : "m" (gvar)); }

/////////////////////////////////////////////////////////////////////////////
# else                          // other...
#   error "process.cc: unsupported CPU"
# endif

/////////////////////////////////////////////////////////////////////////////
#else                           // other compilers not supported
/////////////////////////////////////////////////////////////////////////////

# error "process.cc: unsupported compiler"

/////////////////////////////////////////////////////////////////////////////
#endif
/////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
// machine independent code:
////////////////////////////////////////////////////////////////////////////

/**
 * internal structure for storing of process context
 * @ingroup process
 */
struct P_Context_t {
    jmp_buf status;     // stored SP, IP, and other registers
    size_t size;        // size of following array (allocated on heap)
    char stack[1];      // stack contents saved
};

////////////////////////////////////////////////////////////////////////////
// global variables (should be volatile)
static jmp_buf _ProcessDispatcherStatusBuffer;  // stack state before dispatch
static char *volatile _StackBase = 0;    // start of stack data area
static char *volatile _StackBase2 = 0;   // CHECKING start of stack data area

static P_Context_t *volatile P_Context = 0;      // temporary - process state
static volatile size_t _StackSize = 0;   // temporary - global stack size

////////////////////////////////////////////////////////////////////////////
// special THREADS implementation:
////////////////////////////////////////////////////////////////////////////

static void THREAD_INTERRUPT_f();       // special function

/// interrupt process behavior execution, continue after return
#define THREAD_INTERRUPT()                                              \
{ /* This should be MACRO */                                            \
  /* if(!isCurrent())  SIMLIB_error("Can't interrupt..."); */           \
  this->_status = _INTERRUPTED;                                         \
  THREAD_INTERRUPT_f();                                                 \
  this->_status = _RUNNING;                                             \
  this->_context = 0;                                                   \
}

/// does not save context
#define THREAD_EXIT() \
    longjmp(_ProcessDispatcherStatusBuffer, 2)  // jump to dispatcher

/// allocate memory for process context, sz = size of stack area to save
#define ALLOC_CONTEXT(sz)                                                 \
    P_Context = (P_Context_t *) new char[sizeof(P_Context_t) + sz];       \
    P_Context->size = sz;

/// free memory of process context
#define FREE_CONTEXT()          \
    delete[] P_Context;         \
    P_Context = 0;

////////////////////////////////////////////////////////////////////////////
/// constructor sets state to PREPARED
Process::Process(Priority_t p) : Entity(p) {
  Dprintf(("Process::Process(%d)", p));
  _wait_until = false;
  _context = 0;                 // pointer to process context
  _status = _PREPARED;          // prepared for running
}

////////////////////////////////////////////////////////////////////////////
/// destructor
/// sets status to TERMINATED
/// and removes process from queue/calendar/waituntil list
Process::~Process()
{
    Dprintf(("Process::~Process()"));

    // TODO: what if Current?   (== delete this in Behavior())
    //if(this==Current) SIMLIB_warning("Currently running process (self-)destructed");

    // destroy context data
    delete [] (char*)_context;
    _context = 0;

    _status = _TERMINATED;

    if (_wait_until) {
        //SIMLIB_warning("Process in Wait-Until destructed");
        _WaitUntilRemove();     // Remove from wait-until list
    }

    if (Where() != 0) {         // if waiting in queue
        //SIMLIB_warning("Process waiting in queue destructed");
        Out();                  // remove from queue, no warning
    }
    if (!Idle()) {              // if process is scheduled
        //SIMLIB_warning("Active process destructed");
        SQS::Get(this);         // remove from calendar
    }
}

////////////////////////////////////////////////////////////////////////////
/// Name of the process
/// warning: uses static buffer for generic names
const char *Process::Name() const
{
    const char *name = SimObject::Name();
    if (*name)
        return name;            // has explicit name
    else
        return SIMLIB_create_tmp_name("Process#%lu", _Ident);       // generic
}

////////////////////////////////////////////////////////////////////////////
/// Interrupt process behavior - this ensures WaitUntil tests
/// WARNING: use with care - it can run higher (or equal) priority processes
///          before continuing
void Process::Interrupt()
{
    Dprintf(("%s.Interrupt()", Name()));
    if (!isCurrent())
        return;                 // TODO: Error?
    // continue after other processes WaitUntil checks
    // TODO: use >highest priority to eliminate problem
    Entity::Activate(); // schedule now - can run higher priority
                        //                processes before this one
    THREAD_INTERRUPT();
    // TODO: return to previous priority
}

////////////////////////////////////////////////////////////////////////////
/// Activate process at time t
void Process::Activate(double t)
{
    Entity::Activate(t);
    if (!isCurrent())
        return;
    // SIMLIB_warning("Process can not activate itself - use Wait");
    THREAD_INTERRUPT();
}

////////////////////////////////////////////////////////////////////////////
/// Wait for dtime
void Process::Wait(double dtime)
{
    Dprintf(("%s.Wait(%g)", Name(), dtime));
    Entity::Activate(double (Time) + dtime);    // scheduling
    if (!isCurrent())
        return;
    THREAD_INTERRUPT();
}

////////////////////////////////////////////////////////////////////////////
/// Seize facility f with optional priority of service sp
/// possibly waiting in input queue, if it is busy
void Process::Seize(Facility & f, ServicePriority_t sp /* = 0 */ )
{
    f.Seize(this, sp);          // polymorphic interface
}

////////////////////////////////////////////////////////////////////////////
/// Release facility f
/// possibly activate first waiting entity in queue
void Process::Release(Facility & f)
{
    f.Release(this);            // polymorphic interface
}

////////////////////////////////////////////////////////////////////////////
/// Enter - use cap capacity of store s
/// possibly waiting in input queue, if not enough free capacity
void Process::Enter(Store & s, unsigned long cap)
{
    s.Enter(this, cap);         // polymorphic interface
}

////////////////////////////////////////////////////////////////////////////
/// Leave - return cap capacity of store s
/// and enter first waiting entity from queue, which can use free capacity
//TODO: should be parametrized: use first-only, first-good, all-good
void Process::Leave(Store & s, unsigned long cap)
{
    s.Leave(cap);               // polymorphic interface
}

////////////////////////////////////////////////////////////////////////////
/// insert current process into queue
/// The process can be at most in single queue.
//
void Process::Into(Queue & q)
{
    if (Where() != 0) {
        SIMLIB_warning("Process already in (other) queue");
        Out();          // if already in queue then remove
    }
    q.Insert(this);     // polymorphic interface
}

////////////////////////////////////////////////////////////////////////////
/// Process deactivation
/// To continue the behavior it should be activated again
/// Warning: memory leak if not activated/deleted
void Process::Passivate()
{
    Entity::Passivate();
    if (!isCurrent())
        return;         // passivated by other process
    THREAD_INTERRUPT();
}

////////////////////////////////////////////////////////////////////////////
/// Terminate the process
//
void Process::Terminate()
{
    Dprintf(("%s.Terminate()", Name()));

    // remove from all queues  TODO: write special method for this
    if (Where() != 0) {         // Entity linked in queue
        Out();                  // remove from queue, no warning
    }
    if (!Idle())
        SQS::Get(this);         // remove from calendar

    // End of thread
    if (isCurrent()) {          // if currently running process
        _status = _TERMINATED;
        THREAD_EXIT();          // Jump back to dispatcher
    }
    else {
        _status = _TERMINATED;
        if (isAllocated())
            delete this;        // Remove passive process
    }
}

////////////////////////////////////////////////////////////////////////////
#define CANARY1 (reinterpret_cast<long>(this)+1) // unaligned value is better

/**
 * process dispatch method
 *
 * The dispatcher starts/reactivates process behavior
 *
 * IMPORTANT notes:
 * - Function contains some non-portable code and should be called
 *   from single place in simulation-control algorithm,
 *   because it is sensitive to stack-frame position
 *   (it saves/restores the stack contents).
 *
 * This function:
 *  1) marks current position on stack
 *  2) marks current CPU context
 *  3) calls Behavior()
 *  4) after interruption of Behavior() returns
 *  5) if called next, do 1), 2) and move stack pointer (SP)
 *  6) copy saved stack content back
 *  7) do longjmp to restore Behavior() execution
 *
 * @ingroup process
 */
void Process::_Run() throw() // no exceptions
{
    // WARNING: all local variables should be volatile (see setjmp manual)
    static const char * status_strings[] = {
        "unknown", "PREPARED", "RUNNING", "INTERRUPTED", "TERMINATED"
    };
    Dprintf(("%016p===%s._Run() status=%s", this, Name(), status_strings[_status]));

    if (_status != _INTERRUPTED && _status != _PREPARED)
        SIMLIB_error(ProcessNotInitialized);

    // Mark the stack base address
    volatile long mylocal = CANARY1;     // should be automatic = on stack
    // Warning: DO NOT USE ANY OTHER LOCAL VARIABLES in this function!
    _StackBase = (char*)(&mylocal + 1);

//
// STACK layout (stack grows down):
//
//                |         |
//                |         |
// _StackBase +-> +---------+
//            |   | mylocal |
//            |   +---------+
//            |   |         |
//            |    .........
//     _size  |    arguments, return addresses, locals, etc
//            |    .........
//            |   |         |
//            |   +---------+
//            |   | mylocal2|
//            +-> +---------+
//
#define STACK_RESERVED 0x200 // reserve for "red zone" ?
//
//            |   |         |
//            |   |         |
// _StackBase2 -> +---------+


#if 0
    DEBUG(DBG_THREAD,("THREAD_STACK_BASE=%016p", _StackBase));
    // CHECK if the _StackBase position is constant in each call
    static char *_StackBase0=0;
    if(_StackBase0==0)
        _StackBase0=_StackBase;
    else if (_StackBase!=_StackBase0)
        SIMLIB_error("Internal error: _StackBase not constant");
#endif

    //  2) mark current CPU context (part of context)
    if (!setjmp(_ProcessDispatcherStatusBuffer))
    {
        // setjmp returned after saving current status

        _status = _RUNNING;
        if (_context == 0) {    // process start
            DEBUG(DBG_THREAD, (" --- Thread START "));
            Behavior();         // run behavior description
            DEBUG(DBG_THREAD, (" --- Thread END "));
            _status = _TERMINATED;
            if(mylocal != CANARY1)
                SIMLIB_error("Process canary1 died after Behavior() return");
            // Remove from any queue
            if (Where() != 0) {         // Entity linked in queue
                Out();                  // Remove from queue, no warning
            }
            if (!Idle())
                SQS::Get(this);         // Remove from calendar
            //if(in any facility) error
        } else { // process was interrupted and has saved context
            DEBUG(DBG_THREAD, (" --- Thread CONTINUE "));
            mylocal = 0; // for checking only - previous value should be saved and later restored
            // RESTORE_CONTEXT
            // A. Save local variables to global
            // This is important because of following stack manipulations.
            P_Context = (P_Context_t*) this->_context;
            _StackSize = P_Context->size;

            // B. Shift stack pointer under the currently restored stack area
            // This is very important for next memcpy and longjmp
            // (stack grows down), we reserve some more space
            _StackBase2 = _StackBase - _StackSize - STACK_RESERVED;

            // DEBUG(DBG_THREAD,("THREAD_SHIFT_SP(to=%p)",_StackBase2));

            // ==========================================================
            _SET_STACK_PTR(_StackBase2); // === BEGIN inconsistent state!
            // Warning: We can not use any local variables here!

            // C. Copy saved stack contents back to stack
            memcpy((void *) (_StackBase - _StackSize), P_Context->stack, _StackSize);

            // DEBUG(DBG_THREAD, ("THREAD - before longjmp %p",P_Context->state));

            // 4. Restore proces status (SP,IP,...)
            longjmp(P_Context->status, 1); // === END inconsistent state!
            // ===========================================================
            // never reach this point - longjmp never returns

        }
    }
    else
    {   // setjmp: back from Behavior() - interrupted or terminated

        if(mylocal != CANARY1)
            SIMLIB_error("Process implementation canary1 died");

        if(!isTerminated()) {
            // interrupted process
            // We store content in global variables back to attributes
            P_Context->size = _StackSize;
            this->_context = P_Context;
            DEBUG(DBG_THREAD,(" --- Thread INTERRUPTED %p.context=%p, size=%d", \
                                                this, P_Context, _StackSize));
            P_Context = 0; // cleaning
        }
    }

    Dprintf(("%016p===%s._Run() RETURN status=%s", this, Name(), status_strings[_status]));

    //TODO: MOVE to simulation control loop
    if (isTerminated() && isAllocated()) {
        // terminated process on heap
        DEBUG(DBG_THREAD,("Process %p ends and is deallocated now",this));
        delete this;    // destroy process
    }
    // return to simulation control
}


////////////////////////////////////////////////////////////////////////////
#define CANARY2 0xDEADBEEFUL
/**
 * Special function called from Process::Behavior() directly or indirectly.
 *
 * This function:
 *
 *  1) computes stack content size,
 *  2) allocates memory for stack contents,
 *  3) saves stack contents to allocated memory,
 *  4) saves CPU context using setjmp(), and
 *  5) interrupts execution of current function using longjmp()
 *     to process dispatcher code,
 *  == (now run dispatcher and other code)
 *  6) continues execution after longjmp from dispatcher.
 *  7) frees memory allocated at 2)
 *
 * Warning: This function is critical to process switching code and
 *          should not be inlined! It is never called directly by SIMLIB user.
 *
 * @ingroup process
 */
static void THREAD_INTERRUPT_f()
{
    // DEBUG(DBG_THREAD,("THREAD_INTERRUPT ***** begin *****"));

    ////////////////////////////////////////////////////////////////////////
    // SAVE THE STACK STATE of the thread
    ////////////////////////////////////////////////////////////////////////

    // 1) compute stack context size  (from _StackBase to local variable)
    volatile unsigned mylocal2 = CANARY2;       // the only on-stack variable
    _StackSize = (size_t) (_StackBase - (char *) (&mylocal2));

    // 2) allocate memory for stack contents
    ALLOC_CONTEXT(_StackSize);

    // 3) save stack data (stack grows DOWN)
    memcpy(P_Context->stack, (_StackBase - _StackSize), _StackSize);

    mylocal2 = 0;       // previous value should be saved now and restored later

    ////////////////////////////////////////////////////////////////////////
    // STACK CONTENTS SAVED
    ////////////////////////////////////////////////////////////////////////

    // 4) mark CPU context
    //DEBUG(DBG_THREAD,("THREAD_SAVE_STACK: before setjmp(%p)", P_Context->status));
    if (!setjmp(P_Context->status)) {
        //DEBUG(DBG_THREAD,("THREAD_SAVE_STACK: after setjmp()"));
        ////////////////////////////////////////////////////////////////////
        // 5) Interrupt the execution of Behavior()
        longjmp(_ProcessDispatcherStatusBuffer, 1);     // --> longjmp back to dispatcher
        // never return here
    }

    //DEBUG(DBG_THREAD,("THREAD_RESTORE_STACK: after longjmp(%p) back", P_Context->status));
    ////////////////////////////////////////////////////////////////////////
    // 6) Continue execution after longjmp from dispatcher
    // Data were restored on stack, longjmp restores SP

    // 7) free memory of stack contents
    FREE_CONTEXT();

    // DEBUG(DBG_THREAD,("THREAD_INTERRUPT ***** end *****"));
    if (mylocal2 != CANARY2)
        SIMLIB_error("Process switching canary2 died.");
    // return and continue in Process::Behavior() execution
}


} // namespace

