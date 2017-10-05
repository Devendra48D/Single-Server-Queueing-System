/////////////////////////////////////////////////////////////////////////////
//! \file debug.cc    Some internal debugging tools
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
// debugging facilities
//


////////////////////////////////////////////////////////////////////////////
//
// interface
//

#include "simlib.h"
#include "internal.h"

namespace simlib3 {

////////////////////////////////////////////////////////////////////////////
//
// implementation
//

SIMLIB_IMPLEMENTATION;

#ifndef NDEBUG
unsigned long SIMLIB_debug_flag = 0UL; // default = no debugging
#endif

// start debugging
void DebugON()
{
    Debug(~0UL);
}

// stop debugging
void DebugOFF()
{
    Debug(0UL);
}

// set debug mode flag
unsigned long Debug(unsigned long mode)
{
#ifdef NDEBUG
    SIMLIB_warning(NoDebugErr);
    return 0;
#else
    unsigned long tmp = SIMLIB_debug_flag;
    SIMLIB_debug_flag = mode;
    return tmp;
#endif
}

}
// end

