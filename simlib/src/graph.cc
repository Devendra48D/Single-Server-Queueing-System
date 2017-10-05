/////////////////////////////////////////////////////////////////////////////
// graph.cc
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
// description:
//   Graph object has a name (= output file name)
//                may send given values to output
//                (is an output port of simulation model)
//
// NEW! output ports ('measuring device') + output stream
//        + assign(port, stream)
//
//   X parameters - inputs (from zero to Nmax)
//                - name of graph file (and graph itself)
//                - period of automatic sampling (can be changed)
//
//----
// Init initialization of stream
// Run initialize samplers
//----
//

// ZMENA! pouzit jen pro pojmenovani a registraci vystupu!
// spec proces bude vzorkovat? (Sampler) + output stream


////////////////////////////////////////////////////////////////////////////
// interface
//

#include "simlib.h"
#include "internal.h"

#ifdef hdsfjfhsaj
#include <cstdio>

////////////////////////////////////////////////////////////////////////////
// implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

////////////////////////////////////////////////////////////////////////////
// Sample - output event
//
void Graph::Sample()
{
  // prozatimni
  double value = in.Value();
  Print("Graph'%s': %g \n", Name(), value);
}


////////////////////////////////////////////////////////////////////////////
//  graph constructors
//
Graph *Graph::First = 0;

void Graph::CtrInit(char *name) // common initialization
{
  Next = First;
  First = this;
  SetName(name);
}

////////////////////////////////////////////////////////////////////////////
//  constructor
//
Graph::Graph(char *_name, Input i, double dTime) : in(i), TimeStep(dTime)
{
  CtrInit(_name);
  StartSampling(); // default
  Dprintf(("Graph::Graph(\"%s\")",_name));
}

////////////////////////////////////////////////////////////////////////////
// destructor
//
Graph::~Graph()
{
  Dprintf(("Graph::~Graph() // \"%s\" ", Name()));
  if(this==First)
    First = Next;
  else
  {
    Graph *i;
    for(i=First; i && i->Next!=this; i=i->Next);
    if(i) i->Next = Next;
  }
}

////////////////////////////////////////////////////////////////////////////
//  Graph::Behavior
//
void Graph::Behavior()
{
  Sample();

  if(TimeStep<=0)
    TimeStep = (double(EndTime)-double(StartTime))/100;

  Activate(double(Time)+double(TimeStep));
}


////////////////////////////////////////////////////////////////////////////
//  Graph::StartSampling
//
void Graph::StartSampling()
{
  if( Phase!=SIMULATION && Phase!=INITIALIZATION ) return;
  //Behavior()
  Sample();

  if(TimeStep<=0)
    TimeStep = (double(EndTime)-double(StartTime))/100;

  Activate(double(Time)+double(TimeStep));
}

////////////////////////////////////////////////////////////////////////////
//  Graph::StopSampling
//
void Graph::StopSampling()
{
  Sample();
  Passivate();
}


////////////////////////////////////////////////////////////////////////////
//  Graph::Initialize
//
void Graph::Initialize()
{
  Graph *p;
  for(p=First; p; p=p->Next)
    p->StartSampling();
}


void _GraphInit()
{
  Graph::Initialize();
}

}
#endif

////////////////////////////////////////////////////////////////////////////
// end of GRAPH.CPP
////////////////////////////////////////////////////////////////////////////

