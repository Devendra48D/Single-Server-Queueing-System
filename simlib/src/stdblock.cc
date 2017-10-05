/////////////////////////////////////////////////////////////////////////////
//! \file stdblock.cc    Non-linear blocks
//
// Copyright (c) 1991-2007 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
// standard non-linear continuous blocks
//
//  class Lim   -- limitation
//  class Insv  -- dead space
//  class Qntzr -- quantizer
//  class Frict -- friction
//  class Hyst  -- hysteresis
//  class Relay -- relay
//  class Blash -- backlash
//  class Rline -- table-defined function (lin. interpolation)
//
// modification history:
//  930825 added Rline
//  2007-12-11 add Name() methods -- good for debugging


////////////////////////////////////////////////////////////////////////////
// interface
//

#include "simlib.h"
#include "internal.h"

#include <cmath>
#include <cstring>


////////////////////////////////////////////////////////////////////////////
// implementation
//

namespace simlib3 {

SIMLIB_IMPLEMENTATION;
////////////////////////////////////////////////////////////////////////////
/// constructor of limitation block
//
Lim::Lim(Input in, double l, double h, double tga)
  : aContiBlock1(in), low(l), high(h), tgalpha(tga)
{
    if (l >= h)
        SIMLIB_error(LowGreaterHigh);
    Dprintf(("Lim::Lim(in,l=%g,h=%g,tga=%g)", l, h, tga));
}

void Lim::_Eval()
{
};

double Lim::Value()
{
    AlgLoopDetector _(this);
    double x = InputValue();
    if (x > high)
        return high * tgalpha;
    if (x < low)
        return low * tgalpha;
    return x * tgalpha;
}


const char *Lim::Name() const
{
    if (HasName())
        return _name;
    else
        return SIMLIB_create_tmp_name("Lim{%p}", this);
}

////////////////////////////////////////////////////////////////////////////
/// dead space
Insv::Insv(Input in, double l, double h, double tga, double tgb)
  : aContiBlock1(in), low(l), high(h), tgalpha(tga), tgbeta(tgb)
{
    if (l >= h)
        SIMLIB_error(LowGreaterHigh);
    Dprintf(("Insv::Insv(in,l=%g,h=%g,tga=%g,tgb=%g)", l, h, tga, tgb));
}

void Insv::_Eval()
{
};

double Insv::Value()
{
    AlgLoopDetector _(this);
    double x = InputValue();
    if (x > high)
        return ((x - high) * tgalpha);
    if (x < low)
        return ((x - low) * tgbeta);
    return (0);
}

const char *Insv::Name() const
{
    if (HasName())
        return _name;
    else
        return SIMLIB_create_tmp_name("Insv{%p}", this);
}

////////////////////////////////////////////////////////////////////////////
/// quantizer constructor
Qntzr::Qntzr(Input in, double p) // p = quantiz. step
   : aContiBlock1(in), step(p)
{
    if (p <= 0)
        SIMLIB_error(BadQntzrStep);
    Dprintf(("Qntzr::Qntzr(in,step=%g)", p));
}

void Qntzr::_Eval()
{
};

double Qntzr::Value()
{
    AlgLoopDetector _(this);
    double x = InputValue();
    double zn = (x > 0) ? 1 : ((x < 0) ? -1 : 0);
    double k = zn * floor((fabs(x) - step / 2) / step + 1);
    return (k * step);
}

const char *Qntzr::Name() const
{
    if (HasName())
        return _name;
    else
        return SIMLIB_create_tmp_name("Qntzr{%p}", this);
}

////////////////////////////////////////////////////////////////////////////
/// friction constructor
Frict::Frict(Input in, double l, double h, double tga)
  : aContiBlock1(in), low(l), high(h), tgalpha(tga)
{
    if (l >= h)
        SIMLIB_error(LowGreaterHigh);
    Dprintf(("Frict::Frict(in,l=%g,h=%g,tga=%g)", l, h, tga));
}

void Frict::_Eval()
{
};

double Frict::Value()
{
    AlgLoopDetector _(this);
    double x = InputValue();
    if (x < 0)
        return (low + x * tgalpha);
    if (x > 0)
        return (high + x * tgalpha);
    return (0);
}

const char *Frict::Name() const
{
    if (HasName())
        return _name;
    else
        return SIMLIB_create_tmp_name("Frict{%p}", this);
}

////////////////////////////////////////////////////////////////////////////
/// hysteresis constructor
Hyst::Hyst(Input i, double _p1, double _p2, double _y1, double _y2, double _tga)
  : Status(i), p1(_p1), p2(_p2), y1(_y1), y2(_y2), tga(_tga)
{
    Dprintf(("Hyst::Hyst(in,%g,%g,%g,%g,tga=%g)", p1, p2, y1, y2, tga));
}

void Hyst::Eval()
{
    double x = InputValue();
    double ys = stl;
    ys = max(ys, y1);           // in the case of bad initialization (out of range)
    ys = min(ys, y2);           // can be leaved out if correct init
    double yn2 = (x - p2) * tga;
    double yn1 = (x - p1) * tga;
    double y = ys;
    if (ys <= yn2)
        y = min(yn2, y2);
    if (ys >= yn1)
        y = max(yn1, y1);
    st = y;                     // new status
    ValueOK = true;
}


const char *Hyst::Name() const
{
    if (HasName())
        return _name;
    else
        return SIMLIB_create_tmp_name("Hyst{%p}", this);
}

////////////////////////////////////////////////////////////////////////////
/// relay constructor
Relay::Relay(Input i, double _p1, double _p2, double _p3, double _p4, double _y1, double _y2)
  : Status(i), p1(_p1), p2(_p2), p3(_p3), p4(_p4), y1(_y1), y2(_y2)
{
    // TODO: check
    Dprintf(("Relay::Relay(in,%g,%g,%g,%g,y1=%g,y2=%g)",
                              p1, p2, p3, p4, y1, y2));
}

void Relay::Eval()
{
    double x = InputValue();
    double y;

    if (x < p1)
        y = y1;
    else if (x < p2)
        y = stl;
    else if (x < p3)
        y = 0;
    else if (x < p4)
        y = stl;
    else
        y = y2;
    if (y != stl)
        ContractStep();         // forces step shortening
    st = y;
    ValueOK = true;
}

const char *Relay::Name() const
{
    if (HasName())
        return _name;
    else
        return SIMLIB_create_tmp_name("Relay{%p}", this);
}

////////////////////////////////////////////////////////////////////////////
/// blash constructor
Blash::Blash(Input i, double _p1, double _p2, double _tga)
  : Status(i), p1(_p1), p2(_p2), tga(_tga)
{
    Dprintf(("Blash::Blash(in,%g,%g,tga=%g)", p1, p2, tga));
}

void Blash::Eval()
{
    // TODO: CHECK !!!
    double x = InputValue();
    double ys = stl;
    double yn1 = (x - p1) * tga;
    double yn2 = (x - p2) * tga;
    double y = max(yn2, ys);
    y = min(yn1, y);
    st = y;
    ValueOK = true;
}

const char *Blash::Name() const
{
    if (HasName())
        return _name;
    else
        return SIMLIB_create_tmp_name("Blash{%p}", this);
}

////////////////////////////////////////////////////////////////////////////
/// constructor of class Rline
/// implements table-defined function (linear interpolation)
// TODO: variants
//       - use data directly? (no new and copy),
//       - read from input file
Rline::Rline(Input in, int num, double *X, double *Y)
  : aContiBlock1(in), n(num)
{
    Dprintf(("Rline::Rline(in,%i,X[],Y[])", n));
    if (n < 2)
        SIMLIB_error(RlineErr1);        // Rline: n<2
    //
    // TODO: use vectors of (x,y) pairs
    //
    tableX = new double[n];
//    if (!tableX)
//        SIMLIB_error(MemoryError);
    tableY = new double[n];
//    if (!tableY)
//        SIMLIB_error(MemoryError);
    memcpy(tableX, X, n * sizeof(double));
    memcpy(tableY, Y, n * sizeof(double));
    for (int i = 1; i < n; i++)
        if (tableX[i] < tableX[i - 1])
            SIMLIB_error(RlineErr2);    // Rline: tableX not ordered
    //
    // OPTIMIZE:
    //  count (tableY[i]-tableY[i-1]) / (tableX[i]-tableX[i-1])
    //  save: division, 4 -, 3 index; price: array[n]
}

Rline::~Rline()
{
    Dprintf(("Rline::~Rline()", n));
    delete tableX;
    delete tableY;
}

void Rline::_Eval()
{
}

double Rline::Value()
{
    AlgLoopDetector _(this);
    // TODO: add step change detection => Status!
    double x = InputValue();
    int i;
    if (x >= tableX[n - 1])
        return tableY[n - 1];
    if (x <= tableX[0])
        return tableY[0];
    for (i = 1; x > tableX[i]; i++) { /*empty*/ } // linear search
    return (tableY[i] - tableY[i - 1]) / (tableX[i] - tableX[i - 1])
        * (x - tableX[i - 1]) + tableY[i - 1];
}

const char *Rline::Name() const
{
    if (HasName())
        return _name;
    else
        return SIMLIB_create_tmp_name("Rline{%p}", this);
}

} // namespace
