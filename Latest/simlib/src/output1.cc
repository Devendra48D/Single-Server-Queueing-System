/////////////////////////////////////////////////////////////////////////////
//! \file output1.cc    Output subsystem implementation
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  Output methods for all continuous-part classes
//#error Split into parts!!!!
//

////////////////////////////////////////////////////////////////////////////
//  interface
//

#include "simlib.h"
#include "internal.h"

#include <cstdio>    // sprintf()


////////////////////////////////////////////////////////////////////////////
//  implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

////////////////////////////////////////////////////////////////////////////
//  Event::Output
//
void Event::Output() const
{
    Print(" EVENT %-48s \n", Name());
}


////////////////////////////////////////////////////////////////////////////
//  List::Output
//
void List::Output() const
{
    Print("+----------------------------------------------------------+\n");
    Print("| LIST %-51s |\n", Name());
    Print("+----------------------------------------------------------+\n");
#ifdef XXX_PRINT_LIST_
    {
        Print("Contents:\n");
        unsigned u;
        iterator p;
        for (p = begin(), u = 1; p != end(); ++p, u++) {
            Print("  [%u]:\t", u);
            (*p)->Output();
        }
        Print("End of list.\n");
    }
#endif
}

void SIMLIB_statistics_t::Output() const
{
    Print("#\n");
    Print("# SIMLIB run-time statistics:\n");
    Print("#    StartTime  = %g\n", StartTime);
    Print("#    EndTime    = %g\n", EndTime);
    Print("#    EventCount = %ld\n", EventCount);
    Print("#    StepCount  = %ld\n", StepCount);
    if (StepCount>0) {
        Print("#    MinStep    = %g\n", MinStep);
        Print("#    MaxStep    = %g\n", MaxStep);
    }
    Print("#\n");
}

} // namespace

