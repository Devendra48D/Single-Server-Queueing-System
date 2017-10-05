/////////////////////////////////////////////////////////////////////////////
//! \file output2.cc    Output subsystem implementation
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  Output methods for all discrete classes
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
//  Facility::Output
//
void Facility::Output() const
{
  char s[100];
  Print("+----------------------------------------------------------+\n");
  Print("| FACILITY %-47s |\n",Name());
  Print("+----------------------------------------------------------+\n");
  sprintf(s," Status = %s ", (Busy()) ? "BUSY" : "not BUSY");
  Print("| %-56s |\n",s);
  if (tstat.Number()>0)
  {
    sprintf(s," Time interval = %g - %g ",tstat.StartTime(), (double)Time);
    Print(  "| %-56s |\n", s);
    Print(  "|  Number of requests = %-28ld       |\n", tstat.Number());
    if (Time>tstat.StartTime())
      Print("|  Average utilization = %-27g       |\n", tstat.MeanValue());
  }
  Print("+----------------------------------------------------------+\n");

  if (OwnQueue())
  {
    if (Q1->StatN.Number()>0) // used
    {
      Print("  Input queue '%s.Q1'\n", Name());
      Q1->Output();
    }
  }
  else
    Print("  External input queue '%s'\n",Q1->Name());

  if (Q2->StatN.Number()>0) // used
  {
    Print("  Interrupted services queue '%s.Q2'\n", Name());
    Q2->Output();
  }

  Print("\n");
}

////////////////////////////////////////////////////////////////////////////
//  Histogram::Output
//
void Histogram::Output() const
{
  Print("+----------------------------------------------------------+\n");
  Print("| HISTOGRAM %-46s |\n",Name());
  //Print("+----------------------------------------------------------+\n");
  stat.Output();
  long sum = 0;
  unsigned i;
  for (i=0; i<=count+1; i++)
    sum += dptr[i];
  if (sum==0) return;
  //Print("+------------+------------+----------+----------+----------+\n");
  Print("|    from    |     to     |     n    |   rel    |   sum    |\n");
  Print("+------------+------------+----------+----------+----------+\n");
  double from,to;
  long s;
  from = low;
  s = dptr[0];
  for (i=1; i<=count; i++)
  {
    unsigned x = dptr[i];
    s += x;
    to = from+step;
    Print("| %10.3f | %10.3f | %8u | %8.6f | %8.6f |\n",
            from, to, x, (double)x/sum, (double)s/sum);
    from = to;
  }
  Print("+------------+------------+----------+----------+----------+\n");
  Print("\n");
}

////////////////////////////////////////////////////////////////////////////
//  Process::Output
//
void Process::Output() const
{
  Print(" PROCESS %-38s %10s \n", Name(), isCurrent()?"Current":" ");
}

////////////////////////////////////////////////////////////////////////////
//  Queue::Output
//
void Queue::Output() const
{
  char s[100];
  Print("+----------------------------------------------------------+\n");
  Print("| QUEUE %-39s %10s |\n", Name(), StatN.Number()?"":"not used");
  if (StatN.Number() > 0)
  {
    Print("+----------------------------------------------------------+\n");
    sprintf(s," Time interval = %g - %g ",StatN.StartTime(), (double)Time);
    Print(  "| %-56s |\n", s);
    Print(  "|  Incoming  %-26ld                    |\n", StatN.Number());
    Print(  "|  Outcoming  %-26ld                   |\n", StatDT.Number());
    Print(  "|  Current length = %-26lu             |\n", size());
    Print(  "|  Maximal length = %-25g              |\n", StatN.Max());
    double dt = double(Time) - StatN.StartTime();
    if(dt>0)
    {
      double mv = StatN.MeanValue();
      Print(  "|  Average length = %-25g              |\n",mv);
    }
    if (StatDT.Number()>0)
    {
      Print(  "|  Minimal time = %-25g                |\n", StatDT.Min());
      Print(  "|  Maximal time = %-25g                |\n", StatDT.Max());
      double mv = StatDT.MeanValue();
      Print(  "|  Average time = %-25g                |\n", mv);
      if (StatDT.Number()>99)
        Print("|  Standard deviation = %-25g          |\n",
               StatDT.StdDev());
    }
  }
  Print("+----------------------------------------------------------+\n");
#ifdef XXX_PRINT_QUEUE_
  // only for debug
  {
    Print("Queue contents:\n");
    unsigned u;
    iterator p;
    for(p=begin(), u=1; p!=end(); ++p, u++) {
      Print("  [%u]:\t",u);
      (*p)->Output();
    }
    Print("End of queue.\n");
  }
#endif
}

////////////////////////////////////////////////////////////////////////////
//  Stat::Output
//
void Stat::Output() const
{
  Print("+----------------------------------------------------------+\n");
  Print("| STATISTIC %-46s |\n",Name());
  Print("+----------------------------------------------------------+\n");
  if (n==0)
    Print("|  no record                                               |\n");
  else
  {
    Print(  "|  Min = %-15g         Max = %-15g     |\n", min, max);
    Print(  "|  Number of records = %-26ld          |\n", n);
    Print(  "|  Average value = %-25g               |\n", MeanValue());
    if (n>99)
      Print("|  Standard deviation = %-25g          |\n", StdDev());
  }
  Print("+----------------------------------------------------------+\n");
}

////////////////////////////////////////////////////////////////////////////
//  Store::Output
//
void Store::Output() const
{
  char s[100];
  Print("+----------------------------------------------------------+\n");
  Print("| STORE %-50s |\n",Name());
  Print("+----------------------------------------------------------+\n");
  sprintf(s," Capacity = %lu  (%lu used, %lu free) ",
               Capacity(), used, Free());
  Print("| %-56s |\n",s);
  if (tstat.n>0)
  {
    sprintf(s," Time interval = %g - %g ",tstat.StartTime(), (double)Time);
    Print(  "| %-56s |\n", s);
    Print(  "|  Number of Enter operations = %-24ld   |\n", tstat.Number());
    Print(  "|  Minimal used capacity = %-30g  |\n", tstat.Min());
    Print(  "|  Maximal used capacity = %-30g  |\n", tstat.Max());
    if (Time>tstat.StartTime())
      Print("|  Average used capacity = %-30g  |\n", tstat.MeanValue());
  }
  Print("+----------------------------------------------------------+\n");
  if (OwnQueue())
  {
    if (Q->StatN.Number()>0) // byla pouzita
    {
      Print("  Input queue '%s.Q'\n", Name());
      Q->Output();
    }
  }
  else
    Print("  External input queue '%s'\n",Q->Name());
  Print("\n");
}

////////////////////////////////////////////////////////////////////////////
//  TStat::Output
//
void TStat::Output() const
{
  Print("+----------------------------------------------------------+\n");
  Print("| STATISTIC %-46s |\n", Name());
  Print("+----------------------------------------------------------+\n");
  if (n==0)
    Print("|  no record                                               |\n");
  else
  {
    char s[100];
    Print(  "|  Min = %-15g         Max = %-15g     |\n", min, max);
    sprintf(s," Time = %g - %g ", t0, (double)Time);
    Print(  "| %-56s |\n", s);
    Print(  "|  Number of records = %-26ld          |\n", n);
    if (Time>t0)
      Print("|  Average value = %-25g               |\n", MeanValue());
  }
  Print("+----------------------------------------------------------+\n");
}

}
// end of output2.cc

