/////////////////////////////////////////////////////////////////////////////
//! \file random2.cc  Random number generators - transformations
//
// Copyright (c) 1991-2004 Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//

//
//  random number generators (dependent on Random())
//

////////////////////////////////////////////////////////////////////////////
// interface
//

#include "simlib.h"
#include "internal.h"

#include <cmath>  // exp() floor() log() pow() sqrt()


////////////////////////////////////////////////////////////////////////////
// implementation
//


namespace simlib3 {

SIMLIB_IMPLEMENTATION;

////////////////////////////////////////////////////////////////////////////
//  _gam
//
static double _gam(double AK)
{
  int K,i;
  double FK,PROD,DG,G,W;

  FK = K = (int) floor(AK);
  G = 0.0;
  if (K>0)
  {
    PROD = 1.0;
    for(i=1; i<=K; i++)  PROD *= Random();
    G = -log(PROD);
  }

  DG = AK-FK;
  if (DG <= 0.015)  return (G);
  if (DG >=0.935)
    W=1.0;
  else
  {
    double A,B,X,Y;
    A = 1.0/DG;
    B = 1.0/(1.0-DG);
    do{
      X = pow(Random(),A);
      Y = pow(Random(),B+X);
    }while (Y>1.0);
    W = X/Y;
  }
  G += W*(-log(Random()));
  return (G);
}

////////////////////////////////////////////////////////////////////////////
//  Uniform - uniform random number generator
//
double Uniform(double l, double h)
{
  if( l >= h ) SIMLIB_error(BadUniformParam);
  return(l+(h-l)*Random());
}

////////////////////////////////////////////////////////////////////////////
//  Normal(mi,sigma)
//  mi    = mean value
//  sigma = std deviation? (smerodatna odchylka) ###
//
double Normal(double mi, double sigma)
{
  int i;
  double SUM = 0.0;
  for (i=0; i<12; i++)  SUM += Random();
  return (SUM-6.0)*sigma + mi;
}

////////////////////////////////////////////////////////////////////////////
//  Weibul
//
// TODO: check
double Weibul(double lambda, double alfa)
{
  double R,W;

  if (lambda<=0.0 || alfa<=1.0)  SIMLIB_error(WeibullError);

  while ((R=Random()) == 0 || R == 1) { /*empty*/ }
  W= pow (-1.0/lambda*log(1.0-R), 1.0/alfa);
  return (W);
}

////////////////////////////////////////////////////////////////////////////
//  Erlang
//
// TODO: check
double Erlang(double alfa, int beta)
{
  double ER = 1.0;
  int i;
  if (beta<1)  SIMLIB_error(ErlangError);
  for (i=0; i<beta; i++)  ER *= Random();
  return -alfa*log(ER);
}

////////////////////////////////////////////////////////////////////////////
//  NegBin
//
int NegBin(double q, int k)
{
  double IS,XLOGQ,R;
  int i;

  if (k<=0 || q<=0)  SIMLIB_error(NegBinError);

  IS = 0;
  XLOGQ = log(q);
  for (i=1; i<=k; i++)
  {
    while ((R=Random()) == 0) { /*empty*/ }
    IS += log(R)/XLOGQ;
  }
  return int(IS);
}

////////////////////////////////////////////////////////////////////////////
//  Gamma
//
double Gamma(double alfa, double beta)
{
  double G;

  G = _gam(alfa)*beta;
  return (G);
}

////////////////////////////////////////////////////////////////////////////
//  Exponential(mv)
//
double Exponential(double mv)
{
  double exp = -mv * std::log(Random());
//  _Print("Exponential(%g),%g = %g\n", mv, r, exp);
  return exp;
}



////////////////////////////////////////////////////////////////////////////
//
//
int NegBinM(double p, int m)
{
  int i,ix;

  if (m<=0)        SIMLIB_error(NegBinMError1);
  if (p<0 || p>1)  SIMLIB_error(NegBinMError2);
  ix = i = 0;
  do{
    if (Random() <= p)  ix++;
    i++;
  }while (i <= m);
  return (ix);
}

////////////////////////////////////////////////////////////////////////////
//  Beta
//
double Beta(double th, double fi, double min, double max)
{
  double X;
  X = _gam(th);
  X = X/(X+_gam(fi));
  X = X*(max-min)+min;
  return (X);
}

////////////////////////////////////////////////////////////////////////////
//  Triag(mod,min,max)
//
double Triag(double mod, double min, double max)
{
  double RN,BMA,CMA,TR;

  RN=Random();
  BMA=mod-min;
  CMA=max-min;
  if  (RN<BMA/CMA)
    TR=min+sqrt(BMA*CMA*RN);
  else
    TR=max-sqrt(CMA*(1.0-RN)*(max-mod));
  return (TR);
}

////////////////////////////////////////////////////////////////////////////
//  Log
//
double Logar(double mi, double delta)
{
  double VA = Normal(mi, delta);
  return exp(VA);
}

////////////////////////////////////////////////////////////////////////////
//  Rayle(delta)
//
double Rayle(double delta)
{
  double R;
  while ((R=Random()) == 0) { /*empty*/ }
  return  delta * sqrt(-log(R));
}

////////////////////////////////////////////////////////////////////////////
//  Poisson(double lambda)
//
int Poisson(double lambda)
{
  double Y,X;
  int PSSN = 0;
  if (lambda<=0) SIMLIB_error(PoissonError);
  if (lambda <= 9.0)
  {
    Y=exp(-lambda);
    X=1.0;
    do{
      X *= Random();
      if (X<Y) break;
      PSSN++;
    }while(1);
  }
  else
  {
    double sl=sqrt(lambda);
    do
      PSSN = (int) (Normal(lambda, sl) + 0.5);  /* round ???### */
    while (PSSN<0);
  }
  return PSSN;
}

////////////////////////////////////////////////////////////////////////////
//  Geom(q)
//
int Geom(double q)
{
  double X,R;

  if (q<=0)  SIMLIB_error(GeomError);
  while ((R=Random()) == 0) { /*empty*/ }
  X = log(R)/log(q);
  return int(X);
}

////////////////////////////////////////////////////////////////////////////
//  HyperGeometric
//
int HyperGeom(double p, int n, int m)
{
  int IX,i;
  if (m <= 0)           SIMLIB_error(HyperGeomError1);
  if (p<0 || p>1)       SIMLIB_error(HyperGeomError2);
  IX=0;
  for (i=1; i<=n; i++)
  {
    if (Random() > p)
      p =m*p/(m-1);
    else
    {
      IX++;
      p=(m*p-1.0)/(m-1);
    }
    m--;
  }
  return (IX);
}

////////////////////////////////////////////////////////////////////////////
//  Binom(n,theta)
//  n     = # of experiments (pocet pokusu)
//  theta = probability
//
/*
int Binom(unsigned n, double theta)
{
  int BNM;
  double SUM,R,TT,x;
  if (theta<0 || theta>1) SIMLIB_error(BinomError);
  SUM = R = pow(1-theta,n);
  TT = theta/(1-theta);
  BNM = 0;
  x = Random();
  while(x-SUM > 0)
  {
    R *= (n-BNM)/(BNM+1)*TT;
    SUM += R;
    BNM++;
  }
  return (BNM);
}
*/

} // end

