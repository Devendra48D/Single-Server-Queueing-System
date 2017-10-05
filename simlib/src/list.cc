/////////////////////////////////////////////////////////////////////////////
//! \file  list.cc  internal double-linked list implementation
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  implementation of double-linked list
//  base for Queue abstraction (and simple Calendar)
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
//  constructors
//
List::List() : Link(this,this,this), n(0)
{
  Dprintf(("List::List()"));
}

List::List(const char *name) : Link(this,this,this), n(0)
{
  Dprintf(("List::List(\"%s\")",name));
  SetName(name);
}

////////////////////////////////////////////////////////////////////////////
//  destructor
//
List::~List() {
  Dprintf(("List::~List() // \"%s\" ",Name()));
  clear();
  head = 0; // this is important due to check in ~Link
}

////////////////////////////////////////////////////////////////////////////
// InsFirst --- insert first
//
void List::InsFirst(Link *ent)
{
  if(ent->head!=0)
     SIMLIB_internal_error();
  ent->pred = this;
  ent->succ = Link::succ;
  Link::succ->pred = ent;
  Link::succ = ent;
  ent->head = this;
  n++; // # of list items
}

////////////////////////////////////////////////////////////////////////////
//  InsLast - insert last
//
void List::InsLast(Link *ent)
{
  if(ent->head!=0)
     SIMLIB_internal_error();
  ent->pred = Link::pred;
  ent->succ = this;
  Link::pred->succ = ent;
  Link::pred = ent;
  ent->head = this;
  n++; // # of list items
}

////////////////////////////////////////////////////////////////////////////
//  PostIns - insert after
//
void List::PostIns(Link *ent, iterator pos)
{
    if(pos==end())
        SIMLIB_error(ListActivityError); /// change !!!!
    Link *act = *pos;
    if(act->head != this) // logic-error
        SIMLIB_error(ListActivityError); /// change !!!!
    if(ent->head!=0)
       SIMLIB_internal_error();
    ent->pred = act;
    ent->succ = act->succ;
    act->succ->pred = ent;
    act->succ = ent;
    ent->head = this;
    n++; // # of list items
}

////////////////////////////////////////////////////////////////////////////
// PredIns --- insert before
//
void List::PredIns(Link *ent, iterator pos)
{
  if(ent->head!=0)
     SIMLIB_internal_error();
  Link *act = *pos;
  if(act->head != this)
     SIMLIB_error(ListActivityError); /// change !!!!
  ent->pred = act->pred;
  ent->succ = act;
  act->pred->succ = ent;
  act->pred = ent;
  ent->head = this;
  n++; // # of list items
}

////////////////////////////////////////////////////////////////////////////
// GetFirst --- remove first item from list
//
Link *List::GetFirst()
{
  return Get(begin());
}

////////////////////////////////////////////////////////////////////////////
// GetLast --- remove last item from list
//
Link *List::GetLast()
{
  return Get(--end());
}

////////////////////////////////////////////////////////////////////////////
// Get --- remove selected item from list (virtual)
//
Link *List::Get(iterator pos)
{
  if(empty())
      SIMLIB_error(ListEmptyError);
  if(pos==end())
      SIMLIB_error(ListActivityError); // change !!
  Link *x = *pos;
  if (x->head != this)
    SIMLIB_error(LinkOutError); // can't remove if not in list

  (x->pred->succ = x->succ)->pred = x->pred;
  x->head = 0;      // ---- not in list
  n--;              // # of items in list
  return (x);
}

////////////////////////////////////////////////////////////////////////////
// clear --- list initialization (remove all items)
//
void List::clear()
{
  while(!empty()) {
    Link *e = GetFirst();         // remove entity from list
    if(e->isAllocated()) delete e;   // free memory ?????????????###
  }
}

}
// end

