/////////////////////////////////////////////////////////////////////////////
//! \file  print.cc  Printing - like printf
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
// implementation of direct output to user
//

#include "simlib.h"
#include "internal.h"

#include <cstdio>
#include <cstdarg>

////////////////////////////////////////////////////////////////////////////
// implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;


////////////////////////////////////////////////////////////////////////////
//  OutFile
//

// This singleton solves module initialization order problem
class _FileWrap {
    static FILE *OutFile;
    static FILE *get() {
        if(!OutFile)
            OutFile = stdout;
        return OutFile;
    }
  public:
    operator FILE *()           { return get(); }
    void operator = (FILE *f)   { OutFile=f; }
} OutFile;

FILE *_FileWrap::OutFile = 0;

////////////////////////////////////////////////////////////////////////////
//  SetOutput
//
void SetOutput(const char *name)
{
  if(OutFile!=stdout)
    fclose(OutFile);            // close non-default

  if (*name != '\0') {
    OutFile = fopen(name,"wt");
    if(!OutFile)                // can not be open
      OutFile = stdout;         // use default
  } else {
    OutFile = stdout;           // empty name "" means stdout
  }
}

////////////////////////////////////////////////////////////////////////////
//  _Print
//
int _Print(const char *fmt, ...)
{
   va_list argptr;
   int cnt;

   va_start(argptr, fmt);
   cnt = vfprintf(OutFile, fmt, argptr);
   va_end(argptr);

   fflush(OutFile);

   if (OutFile!=stdout) {
       // copy the same output to stderr
       va_start(argptr, fmt);
       cnt = vfprintf(stderr, fmt, argptr);
       va_end(argptr);
   }

   return(cnt);
}

////////////////////////////////////////////////////////////////////////////
//  Print
//
int Print(const char *fmt, ...)
{
   va_list argptr;
   int cnt;

   va_start(argptr, fmt);
   cnt = vfprintf(OutFile, fmt, argptr);
   fflush(OutFile);
   va_end(argptr);

   return(cnt);
}

int Print(const double x)
{
  return Print(" %g ", x);
}

int Print(const double x, const double y)
{
  return Print(" %g %g ", x, y);
}

int Print(const double x, const double y, const double z)
{
  return Print(" %g %g %g ", x, y, z);
}

////////////////////////////////////////////////////////////////////////////
//  Error - print message & end of program
//
void Error(const char *fmt, ...)
{
  va_list argptr;

  va_start(argptr, fmt);
  vfprintf(OutFile, fmt, argptr);
  fflush(OutFile);
  if (OutFile!=stdout)
    vfprintf(stderr, fmt, argptr);
  va_end(argptr);

  _Print("\n");
  SIMLIB_error(UserError);
}

} // end

