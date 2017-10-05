/////////////////////////////////////////////////////////////////////////////
//! \file link.cc list item implementation
//
// Copyright (c) 1991-2016 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
// Link --- base class for doubly linked list items
//

////////////////////////////////////////////////////////////////////////////
// interface
//

#include "simlib.h"
#include "internal.h"


////////////////////////////////////////////////////////////////////////////
// implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

////////////////////////////////////////////////////////////////////////////
//  Link constructors
//
Link::Link() :
  pred(0), succ(0), head(0)
{
}

Link::Link(Link *p, Link *s, List *h) :
  pred(p), succ(s), head(h)
{
}

////////////////////////////////////////////////////////////////////////////
//  Link destructor
//
Link::~Link() {
  if (head)
    SIMLIB_error(LinkDelError);   // remove non-linked item
}

////////////////////////////////////////////////////////////////////////////
//  Into - inserts item to the list end
//
void Link::Into(List *l)
{
  if (head)
    Out();                   // if in list then remove
  l->InsLast(this);          // insert at end of list
}

////////////////////////////////////////////////////////////////////////////
//  Out - takes item from list
//
void Link::Out()
{
  if (head)
    head->Get(this);
  else
    SIMLIB_error(LinkOutError);      // not in list
}

#if 0
////////////////////////////////////////////////////////////////////////////
//
//
void Link::Follow(Link *li)       // insert after li
{
  if (!li) SIMLIB_error(LinkRefError);
  if (head) Out();                // remove if already inserted
  if (li->head) li->head->PostIns(li);
  else          SIMLIB_error();
}

////////////////////////////////////////////////////////////////////////////
//
//
void Link::Precede(Link *li)      // insert before li
{
  if (!li) SIMLIB_error(LinkRefError);
  if (head) Out();                // remove if already inserted
  if (li->head) li->head->PredIns(li);
  else          SIMLIB_error();
}
#endif

}

