/////////////////////////////////////////////////////////////////////////////
//! \file algloop.cc  Algebraic loop solver blocks
//
// Copyright (c) 1997 David Le¹ka
// Copyright (c) 1998-2016 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  Methods for solving algebraic loops
//

#include "simlib.h"
#include "internal.h"

#include <cmath>

//#define LOOP_DEBUG

#ifdef LOOP_DEBUG
#include <cstdio>
#define dbgprnt(x) { printf x; }
#else
#define dbgprnt(x)
#endif

////////////////////////////////////////////////////////////////////////////
// implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;

////////////////////////////////////////////////////////////////////////////
// AlgLoop  --  constructor
//
AlgLoop::AlgLoop(Input i, double eps, unsigned long max_it,
                 double t_min, double t_max, double t0) :
  aContiBlock1(i),
  Eps(eps),
  MaxIt(max_it),
  TA(t_min),
  TB(t_max),
  T0(t0),
  was_cycle(false),
  phase(0),     // automaton state
  root(0)
{
  if(t_min>=t_max) { // check boundary points
    SIMLIB_error(AL_BadBounds);
  }
  if(t0>t_max || t0<t_min) { // is initial value between t_min,t_max?
    SIMLIB_error(AL_BadInitVal);
  }
}; // AlgLoop::AlgLoop


////////////////////////////////////////////////////////////////////////////
// AlgLoop  --  set parameters
//
void AlgLoop::Set(double eps, unsigned long max_it,
                  double t_min, double t_max, double t0)
{
  if(t_min>=t_max) { // check boundary points
    SIMLIB_error(AL_BadBounds);
  }
  if(t0>t_max || t0<t_min) { // is initial value betveen t_min,t_max?
    SIMLIB_error(AL_BadInitVal);
  }
  Eps=eps;
  MaxIt=max_it;
  TA=t_min;
  TB=t_max;
  T0=t0;
}; // AlgLoop::Set1


////////////////////////////////////////////////////////////////////////////
// AlgLoop  --  set parameters, for methods without initial value
//
void AlgLoop::Set(double eps, unsigned long max_it,
                  double t_min, double t_max)
{
  if(t_min>=t_max) { // check boundary points
    SIMLIB_error(AL_BadBounds);
  }
  Eps=eps;
  MaxIt=max_it;
  TA=t_min;
  TB=t_max;
  T0=t_min;
}; // AlgLoop::Set2

/// get name of object
const char *AlgLoop::Name() const
{
    if (HasName())
        return _name;
    else
        return SIMLIB_create_tmp_name("AlgLoop{%p}", this);
}

////////////////////////////////////////////////////////////////////////////
// Iterations  --  returned value
//
/* Formula:

   f(t)=t ... t[n+1]=f(t[n])
*/
double Iterations::Value()
{
  unsigned long count=0; // counter of iterations
  double prev_root; // root from previous iteration step

  /* take fresh initial value */

  if(phase==0) {
    root=T0;
    phase=1;
  }

  /* iterate */

  do {
    prev_root=root;
    /* compute t=f(t) */
    if(!was_cycle) {
      was_cycle=true;
      root=InputValue(); // go through loop
      if(was_cycle) { // test if block is in loop
        SIMLIB_error(AL_NotInLoop);
      }
    } else {
      was_cycle=false;
      return root; // end of walk
    }
    /* check for number of loops */
    if(count>=MaxIt) {
      SIMLIB_warning(AL_MaxCount);
      break;
    }
    /* convergency test */
    if(root<TA || root>TB) {
      SIMLIB_warning(AL_Diverg);
      break;
    }
    count++;
  } while(fabs(root-prev_root)>Eps); // accuracy has been achieved

  /* restore values */

  dbgprnt(("Iterations-count: %lu\n",count));
  was_cycle=false;
  phase=0;
  return root;
} // Iterations::Value


////////////////////////////////////////////////////////////////////////////
// Bisect  --  returned value
//
/* Formula:

   t:=(ta+tb)/2
   ft:=f(t)
   if (ft*fb<0) -- select interval for next step
   then
     ta:=t;
     fa:=ft;
   else
     tb:=t;
     fb:=ft;
   end if;
*/
double Bisect::Value()
{
  unsigned long count=0; // counter of iterations
  //TODO this code needs checking
  double fa = 0; // start function value
  double fb = 0; // end function value
  double ft = 0; // function value in middle of the interval
  double ta = 0; // start point
  double tb = 0; // end point

  /* now compute fa=f(ta) */

  // TODO automaton state named "phase" ? This code needs cleaning!
  // TODO use switch(state)
  if(phase==0) {
    if(!was_cycle) {    // this means object.Value() call level 0
      was_cycle=true;
      ta=TA;
      fa=ta-InputValue(); // go through loop
      if(was_cycle) { // test if block is in loop
        SIMLIB_error(AL_NotInLoop);
      }
    } else {
      was_cycle=false;
      return TA; // end of walk
    }
    dbgprnt(("ta: %g\n",ta));
    dbgprnt(("fa: %g\n",fa));
    was_cycle=false;
    phase=1;
  } // if phase0

  /* compute fb=f(tb) */

  if(phase==1) {
    if(!was_cycle) {
      was_cycle=true;
      tb=TB;
      fb=tb-InputValue(); // go through loop
    } else {
      was_cycle=false;
      return TB; // end of walk
    }
    dbgprnt(("tb: %g\n",tb));
    dbgprnt(("fb: %g\n",fb));
    was_cycle=false;
    phase=2;
  } // phase1

  /* iterate */

  if(phase==2) {
    do {
      /* compute ft=f(t) */
      if(!was_cycle) {
        was_cycle=true;
        root=0.5*(ta+tb);
        dbgprnt(("root: %g\n",root));
        ft=root-InputValue(); // go through loop
        dbgprnt(("ft: %g\n",ft));
      } else {
        was_cycle=false;
        return root; // end of walk
      }
      /* check for number of loops */
      if(count>=MaxIt) {
        SIMLIB_warning(AL_MaxCount);
        break;
      }
      /* select interval for next step */
      if(ft*fb<0) { // in which half is root?
        ta=root;
        fa=ft;
      } else {
        tb=root;
        fb=ft;
      }
      count++;
    } while(fabs(ft)>Eps && 0.5*(tb-ta)>Eps); // accuracy has been achieved
  } // phase2

  /* restore values */

  dbgprnt(("Bisect-count: %lu\n",count));
  was_cycle=false;
  phase=0;
  return root;
} // Bisect::Value


////////////////////////////////////////////////////////////////////////////
// RegulaFalsi  --  returned value
//
/* Formula:

   t:=(ta*fb-tb*fa)/(fb-fa)
   ft:=f(t)
   if (ft*fb<0) -- select interval for next step
   then
     ta:=t;
     fa:=ft;
   else
     tb:=t;
     fb:=ft;
   end if;
*/
double RegulaFalsi::Value()
{
  unsigned long count=0; // counter of iterations
  //## repair
  double feps = 0; // function value for force precision test
  double prev_root = 0; // root from previous iteration step
  double fa = 0; // function value in boundary point
  double fb = 0; //          - "" -
  double ft = 0; // in middle of interval
  double ta = 0; // boundary point
  double tb = 0; //    - "" -

  /* compute fa=f(ta) */

  if(phase==0) {
    if(!was_cycle) {
      was_cycle=true;
      root=ta=TA;
      fa=ta-InputValue(); // go through loop
      if(was_cycle) { // test if block is in loop
        SIMLIB_error(AL_NotInLoop);
      }
    } else {
      was_cycle=false;
      return TA; // end of walk
    }
    dbgprnt(("ta: %g\n",ta));
    dbgprnt(("fa: %g\n",fa));
    was_cycle=false;
    phase=1;
  } // phase0

  /* compute fb=f(tb) */

  if(phase==1) {
    if(!was_cycle) {
      was_cycle=true;
      tb=TB;
      fb=tb-InputValue(); // go through loop
    } else {
      was_cycle=false;
      return TB; // end of walk
    }
    dbgprnt(("tb: %g\n",tb));
    dbgprnt(("fb: %g\n",fb));
    was_cycle=false;
    phase=2;
  } // phase1

  /* iterate */

  if(phase>=2) {
    do {
      if(phase==2) {
        /* compute ft=f(t) */
        if(!was_cycle) {
          was_cycle=true;
          prev_root=root;
          root=(ta*fb-tb*fa)/(fb-fa);
          ft=root-InputValue(); // go through loop
          dbgprnt(("ft: %12.9g\n",ft));
        } else {
          was_cycle=false;
          dbgprnt(("root: %12.9g\n",root));
          return root; // end of walk
        }
        /* check for number of loops */
        if(count>=MaxIt) {
          SIMLIB_warning(AL_MaxCount);
          break;
        }
        /* select interval for next step */
        if(ft*fb<0) { // in which part is root?
          ta=root;
          fa=ft;
        } else {
          tb=root;
          fb=ft;
        }
        phase=3;
      } // phase2
      if(phase==3) {
        /* force accuracy test */
        if(!was_cycle) {
          was_cycle=true;
          eps_root=((prev_root<root) ? (root+Eps) : (root-Eps));
          feps=eps_root-InputValue(); // go through loop
          dbgprnt(("feps: %12.9g\n",feps));
        } else {
          was_cycle=false;
          dbgprnt(("eps_root: %12.9g\n",eps_root));
          return eps_root; // end of walk
        }
        phase=2;
      } // phase3
      count++;
    // has been achieved required accuracy?
    } while((fabs(ft)>Eps && fabs(root-prev_root)>Eps) || feps*ft>0);
  } // phase2,3

  /* restore values */

  dbgprnt(("RegulaFalsi-count: %lu\n",count));
  was_cycle=false;
  phase=0;
  return root;
} // RegulaFalsi::Value


////////////////////////////////////////////////////////////////////////////
// Newton  --  returned value
//
/* Formula:

   t[n+1]:=(t[n-1]*f[t]-t[n]*f[n-1])/(f[n]-f[n-1])
   f[n+1]:=f(t[n+1])
*/
double Newton::Value()
{
  unsigned long count=0; // counter of iterations
  double trat=1.0+(TB-TA)*1.0e-3; // small step ratio
  double feps = 0; // function value for force precision test
  double ft = 0; // function value in root
  double prev_ft = 0; // function value in previous root
  double aux_root = 0; // auxiliary variables
  double aux_ft = 0; // to store values

  /* compute ft=f(root) */

  if(phase==0) {
    if(!was_cycle) {
      was_cycle=true;
      root=T0;
      ft=root-InputValue(); // go through loop
      if(was_cycle) { // test if block is in loop
        SIMLIB_error(AL_NotInLoop);
      }
      dbgprnt(("f[0]: %g\n",ft));
    } else {
      was_cycle=false;
      dbgprnt(("root[0]: %g\n",root));
      return root; // end of walk
    }
    was_cycle=false;
    phase=1;
  } // phase0

  /* compute prev_ft=f(prev_root) */

  if(phase==1) {
    if(!was_cycle) {
      was_cycle=true;
      prev_root=T0*trat;
      prev_ft=prev_root-InputValue(); // go through loop
      dbgprnt(("f[-1]: %g\n",prev_ft));
    } else {
      was_cycle=false;
      dbgprnt(("root[-1]: %g\n",prev_root));
      return prev_root; // end of walk
    }
    was_cycle=false;
    phase=2;
  } // phase1

  /* iterate */

  if(phase>=2) {
    do {
      if(phase==2) {
        /* compute ft=f(t) */
        if(!was_cycle) {
          was_cycle=true;
          aux_root=root;
          aux_ft=ft;
          root=(prev_root*ft - root*prev_ft) / (ft - prev_ft);
          ft=root-InputValue(); // go through loop
          prev_root=aux_root;
          prev_ft=aux_ft;
          dbgprnt(("ft: %g\n",ft));
        } else {
          was_cycle=false;
          dbgprnt(("root: %g\n",root));
          return root; // end of walk
        }
        /* check for number of loops */
        if(count>=MaxIt) {
          SIMLIB_warning(AL_MaxCount);
          break;
        }
        /* convergency test */
        if(root<TA || root>TB) {
          SIMLIB_warning(AL_Diverg);
          break;
        }
        phase=3;
      } // phase2
      if(phase==3) {
        /* force accuracy test */
        if(!was_cycle) {
          was_cycle=true;
          eps_root=((prev_root<root) ? (root+Eps) : (root-Eps));
          feps=eps_root-InputValue(); // go through loop
          dbgprnt(("feps: %g\n",feps));
        } else {
          was_cycle=false;
          dbgprnt(("eps_root: %g\n",eps_root));
          return eps_root; // end of walk
        }
        phase=2;
      } // phase3
      count++;
    // has been achieved required accuracy?
    } while((fabs(ft)>Eps && fabs(root-prev_root)>Eps) || feps*ft>0);
  } // phase2,3

  /* restore values */

  dbgprnt(("Newton-count: %lu\n",count));
  was_cycle=false;
  phase=0;
  return root;
} // Newton::Value

}

// end
