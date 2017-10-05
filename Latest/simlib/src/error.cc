/////////////////////////////////////////////////////////////////////////////
//! \file  error.cc  error messages processing
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//  internal error messages

////////////////////////////////////////////////////////////////////////////
// interface
//

#include "simlib.h"
#include "internal.h"

#include <cstdlib>
#include <cstdio>
#include <cstdarg>


////////////////////////////////////////////////////////////////////////////
// implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

#define _ERR_TXT     "\nERROR, Time=%g : %s \n"
#define _INT_ERR_TXT "\nERROR, Time=%g : %s (file:%s line:%d)\n"
#define _ABORT_TXT   "\n ========== Simulation aborted ========== \n"
#define _WARNING_TXT "\nWARNING, Time=%g : %s \n"

////////////////////////////////////////////////////////////////////////////
/// print error message and abort program
void SIMLIB_error(const enum _ErrEnum N)
{
  _Print(_ERR_TXT, (double)Time, _ErrMsg(N));
  _Print(_ABORT_TXT);
  SIMLIB_Phase = ERROREXIT;
  SIMLIB_DynamicFlag = false;
  exit(3);
}

/// print error message and abort program
void SIMLIB_error(const char *fmt, ... )
{
  char s[100];
  va_list argptr;
  va_start(argptr, fmt);
  vsnprintf(s, sizeof(s), fmt, argptr);
  va_end(argptr);
  _Print(_ERR_TXT, (double)Time, s);
  _Print(_ABORT_TXT);
  exit(1);
}

/// print error message and abort program
void SIMLIB_error(const char*filename, const int linenum)
{
  _Print(_INT_ERR_TXT, (double)Time,
                       _ErrMsg(InternalError),
                       filename, linenum);
  _Print(_ABORT_TXT);
  SIMLIB_Phase = ERROREXIT;
  SIMLIB_DynamicFlag = false; // if in continuous simulation algorithm
  exit(3);
}

////////////////////////////////////////////////////////////////////////////
/// print warning message and continue
void SIMLIB_warning( const enum _ErrEnum N )
{
  _Print(_WARNING_TXT, (double)Time, _ErrMsg(N));
}

/// print warning message and continue
void SIMLIB_warning(const char *fmt, ... )
{
  char s[100];
  va_list argptr;
  va_start(argptr, fmt);
  vsnprintf(s, sizeof(s), fmt, argptr);
  _Print(_ERR_TXT, (double)Time, s);
  va_end(argptr);
}

} // namespace

