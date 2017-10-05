/////////////////////////////////////////////////////////////////////////////
//! \file atexit.cc  Implementation of global objects for cleanup
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//

#include "simlib.h"
#include "internal.h"

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

static const int MAX_ATEXIT = 10; // for internal use it is enough
static int counter = 0; // internal module counter
static SIMLIB_atexit_function_t atexit_array[MAX_ATEXIT] = { 0, };

// used in SIMLIB
void SIMLIB_atexit(SIMLIB_atexit_function_t p) {
    DEBUG(DBG_ATEXIT,("SIMLIB_atexit(%p)", p ));
    int i;
    for(i=0; i<MAX_ATEXIT; i++) {
       if(atexit_array[i]==0) break;
    }
    if(i<MAX_ATEXIT)
       atexit_array[i]=p;
    else
       SIMLIB_internal_error();
}

// used here
static void SIMLIB_atexit_call() {
    DEBUG(DBG_ATEXIT,("ATEXIT:"));
    for(int i=0; i<MAX_ATEXIT; i++)
       if(atexit_array[i]) {
           DEBUG(DBG_ATEXIT,("ATEXIT_CALL#%d: %p ", i, atexit_array[i]));
           atexit_array[i]();
    }
}

// constructor --- count module and initialize
SIMLIB_module::SIMLIB_module():
    string(0) {
    counter++;
    DEBUG(DBG_MODULE,("MODULE#%d init",counter));
}

// set module id string
int SIMLIB_module::Init(const char *s) {
    string = s;
    return counter;
}

// destructor --- last one calls atexit functions
SIMLIB_module::~SIMLIB_module() {
    DEBUG(DBG_MODULE,("MODULE#%d %s",counter, string?string:""));
    if(--counter == 0)
        SIMLIB_atexit_call();
}


}

// end

