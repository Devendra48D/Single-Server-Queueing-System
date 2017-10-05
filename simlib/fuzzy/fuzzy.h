// cz - pøíli¹ ¾lu»ouèký kùò úpìl ïábelské ódy
/////////////////////////////////////////////////////////////////////////////
// fuzzy.h
//
// SIMLIB version: 2.16.4
// Date: 2001-04-04
// Copyright (c) 1999-2001 Dr. Ing. Petr Peringer, David Martinek 
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//
// Warning: this is EXPERIMENTAL code, interfaces can be changed
//
// Fuzzy subsystem for SIMLIB
// version 0.6 --- We apr 4 10:33:52 CEST 2001 
// 
// subsystem interfaces:
//  - membership functions
//  - fuzzy variable
//  - fuzzification and defuzzification methods
//  - fuzzy block 
//  - sampled fuzzy block
//  - interfaces for typing inference rules in
//


#ifndef FUZZY_H
#define FUZZY_H

#if 0
 #include <stdio.h>
 #define TRACE(x) do{ if(1) x; } while(0)
#else
 #define TRACE(x)
#endif

#include "simlib.h"
#include <vector>

namespace simlib3 {

/**
 * @mainpage FuzzySIMLIB 
 * @version 1.0
 * @author David Martinek
 * @author Dr. Ing. Petr Peringer
 * 
 * FuzzySIMLIB is part of SIMulation LIBrary including the fuzzy and xml module.
 * 
 * FuzzySIMLIB je èástí simulaèní knihovny SIMLIB a obsahuje fuzzy a xml modul.
 */
 
/**
 * @defgroup fuzzy Fuzzy modul
 * This module contains number of classes which is needed for fuzzy controllers modeling.
 *
 * Tento modul obsahuje soubor tøíd potøebných pro modelování fuzzy regulátorù.
 */

/**
 * @defgroup xml XML modul
 * General interface for loading of external data. Now only interface for reding fuzzy models
 * from XML is implemented here.
 *
 * Obecné rozhraní pro naèítání externì ulo¾ených dat. Prozatím je zde implementováno
 * konkrétní rozhraní pro naèítání fuzzy modelù z XML souborù.
 */


/**
 * @example auta.cc
 * This is complete general interface example for typing inference rules in. Time for start 
 * is a little slower but the run is quite fast. This interface is quite uncomfortable.
 * You can use rather interface which is described in FuzzyExpr and FuzzyGeneralRules classes.
 *
 * Toto je kompletní pøíklad pou¾ití obecného rozhraní pro zadávání inferenèních
 * pravidel. Start tohoto programu je trochu del¹í, ale samotný bìh modelu je
 * pomìrnì rychlý. Toto rozhraní je celkem nepohodlné. Radìji pou¾ívejte rozhraní popsané
 * ve tøídách FuzzyExpr a FuzzyGeneralRules.
 */
/**
 * @example auta2.cc
 * This is comfortable general interface example for typing inference rules in. This is the
 * slowest but the  most general solution.
 *
 * Pøíklad komfortního obecného rozhraní pro zápis inferenèních pravidel. Toto je nejpomalej¹í
 * ale nejobecnìj¹í øe¹ení.
 */
/**
 * @example autanottree.cc
 * This is complete specialized interface example for typing infrence rules in. This is the most 
 * faster solution from all these examples.
 * 
 * Toto je kompletní pøíklad pou¾ití specializovaného rozhraní pro zadávání inferenèních
 * pravidel. Pou¾itím tohoto rozhraní je inicializace modelu nejrychlej¹í ze v¹ech 
 * tìchto pøíkladù.
 */
/**
 * @example xml.cc 
 * This is complete example of use XML for model specification. The analyzis takes a quite
 * muxh time, but it has no influence on time for run. This solution is most flexible.
 *
 * Toto je kompletní pøíklad pou¾ití XML pøi specifikaci modelu. Analýza xml zpùsobuje
 * pomìrnì dlouhou inicialializaci, ale na bìh modelu nemá ¾ádný vliv. Toto øe¹ení
 * je nejvíce flexibilní.
 */
/**
 * @example ftycka.cc 
 * Next complete example of use XML for model specification.  The analyzis takes a quite
 * muxh time, but it has no influence on time for run. This solution is most flexible.
 *
 * Dal¹í kompletní pøíklad pou¾ití XML pøi specifikaci modelu. Analýza xml zpùsobuje
 * pomìrnì dlouhou inicialializaci, ale na bìh modelu nemá ¾ádný vliv. Toto øe¹ení
 * je nejvíce flexibilní.
 */


/////////////////////////////////////////////////////////////////////////////
// general membership function
/////////////////////////////////////////////////////////////////////////////
/**
 * Abstract baze class for membership functions.<br>
 * Abstraktní bázová tøída pro funkce pøíslu¹nosti.
 * @ingroup fuzzy
 */
class FuzzyMembershipFunction 
{
    char *Name;		/**< Word value.  */  // TODO: std::string
  protected:
    int defValues;      /**< Number of definition values. */
  public:
    /**
     * It assigns word value to the membership function. This does not copy poiner of parameter
     * name, but it cretes copy of memory.<br>
     * Pøiøadí slovní hodnotu funkci pøíslu¹nosti. Nekopíruje ukazatel parametru name, ale vytváøí
     * kopii pamìti.
     * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
     */
    // implemented in fuzzymf.cc
    FuzzyMembershipFunction(const char *name);
    /**
     * Copy constructor. It creates copy of all member variables. For poiners creates copy of 
     * memory block.<br>
     * Kopy konstruktor. Vytváøí kopii v¹ech èlenských promìnných. Pro ukazatele vytvoøí kopii
     * pamìti.
     * @param duplicate Object that will be duplicated.<br>Objekt, který bude duplikován.
     */
    // implemented in fuzzymf.cc
    FuzzyMembershipFunction(const FuzzyMembershipFunction &duplicate);
    /** Destructor */
    // implemented in fuzzymf.cc
    virtual ~FuzzyMembershipFunction();
    /** Gets word value of this function.<br>Vrací slovní hodnotu této funkce. */
    const char *wordValue() const { return Name; }
    /** It duplicates object.<br>Duplikuje objekt. */
    virtual FuzzyMembershipFunction *clone() const = 0; 
    /** It computes function value (membership).<br>Vypoète funkèní hodnotu (pøíslu¹nost). */
    virtual double Membership(double x) const = 0; 
    /** Center of this function.<br>Støed této funkce. */
    virtual double center() const = 0;
    /** First occurence of maximum in this function.<br>První výskyt maxima v této funkci. */
    virtual double min1() const = 0;
    /** Last occurence of maximum in this function.<br>Poslední výskyt maxima v této funkci. */
    virtual double max1() const = 0;
    /** Print of membership table.<br>Tisk tabulky pøíslu¹nosti. */
    // implemented in fuzzymf.cc
    void Print(double a, double b) const;
    /** Test if is definition of function complete.<br>Test jestli je definice funkce kompletní. */
    bool isComplete() { return defValues == getNumValues(); }
    /** This adds next definition value.<br>Pøidá dal¹í definièní hodnotu. */
    virtual void addDefValue(double value) = 0;
    /** It retuns number of definition values.<br>Vrátí poèet definièních hodnot. */
    virtual int getNumValues() = 0;
};//FuzzyMembershipFunction 

/////////////////////////////////////////////////////////////////////////////
// special membership functions:
/////////////////////////////////////////////////////////////////////////////
/**
 * Membership function of type impulse.<br>
 * Funkce pøíslu¹nosti typu impuls.
 * @ingroup fuzzy
 */
class FuzzySingleton: public FuzzyMembershipFunction
{
    double x0;
  public:
    /**
     * It assigns word value to the membership function and sets definition value.<br>
     * Pøiøadí slovní hodnotu funkci pøíslu¹nosti a nastaví definièní hodnotu.
     * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
     * @param a A definition value.<br>Definièní hodnota.
     */
    // implemented in fuzzymf.cc
    FuzzySingleton(const char *name, double a);
    /**
     * It assigns word value to the membership function.<br>
     * Pøiøadí slovní hodnotu funkci pøíslu¹nosti.
     * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
     */
    // implemented in fuzzymf.cc
    FuzzySingleton(const char *name);
    // implemented in fuzzymf.cc
    
    virtual ~FuzzySingleton() { TRACE(printf("~FuzzySingleton()\n")); }
    /** It duplicates object.<br>Duplikuje objekt. */
    // implemented in fuzzymf.cc
    virtual FuzzySingleton *clone() const;
    /** It computes function value (membership).<br>Vypoète funkèní hodnotu (pøíslu¹nost). */
    // implemented in fuzzymf.cc
    virtual double Membership(double x) const;
    /** Center of this function.<br>Støed této funkce. */
    virtual double center() const { return x0; } 
    /** First occurence of maximum in this function.<br>První výskyt maxima v této funkci. */
    virtual double min1() const { return x0; }
    /** Last occurence of maximum in this function.<br>Poslední výskyt maxima v této funkci. */
    virtual double max1() const { return x0; }
    /** This adds next definition value.<br>Pøidá dal¹í definièní hodnotu. */
    // implemented in fuzzymf.cc
    virtual void addDefValue(double value);
    /** It retuns number of definition values.<br>Vrátí poèet definièních hodnot. */
    virtual int getNumValues() { return 1; }
};//FuzzySingleton

/**
 * Triangulal membership function.<br>Trojúhelníková funkce pøíslu¹nosti.
 * @ingroup fuzzy
 */
class FuzzyTriangle: public FuzzyMembershipFunction 
{
    double x0, x1, x2;
  public:
    /**
     * It assigns word value to the membership function and sets definition values.<br>
     * Pøiøadí slovní hodnotu funkci pøíslu¹nosti a nastaví definièní hodnotu.
     * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
     * @param a A left vertex.<br>Levý vrchol.
     * @param b A center vertex.<br>Prostøední vrchol.
     * @param c A right vertex.<br>Pravý vrchol.
     */
    // implemented in fuzzymf.cc
    FuzzyTriangle(const char *name, double a, double b, double c);
    /**
     * It assigns word value to the membership function.<br>
     * Pøiøadí slovní hodnotu funkci pøíslu¹nosti.
     * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
     */
    // implemented in fuzzymf.cc
    FuzzyTriangle(const char* name);
    /** Destructor. */
    virtual ~FuzzyTriangle() { TRACE(printf("~FuzzyTriangle\n")); } 
    /** It duplicates object.<br>Duplikuje objekt. */
    // implemented in fuzzymf.cc
    virtual FuzzyTriangle *clone() const;
    /** It computes function value (membership).<br>Vypoète funkèní hodnotu (pøíslu¹nost). */
    // implemented in fuzzymf.cc
    virtual double Membership(double x) const;
    /** Center of this function.<br>Støed této funkce. */
    virtual double center() const { return (x0+x1+x2)/3; } 
    /** First occurence of maximum in this function.<br>První výskyt maxima v této funkci. */
    virtual double min1() const { return x1; }
    /** Last occurence of maximum in this function.<br>Poslední výskyt maxima v této funkci. */
    virtual double max1() const { return x1; }
    /** It retuns number of definition values.<br>Vrátí poèet definièních hodnot. */
    virtual int getNumValues() { return 3; } 
    /** This adds next definition value.<br>Pøidá dal¹í definièní hodnotu. */
    // implemented in fuzzymf.cc
    virtual void addDefValue(double value);
};//FuzzyTriangle

/////////////////////////////////////////////////////////////////////////////
// FuzzyTrapez ---
/**
 * Trapezoidal membership function.<br>
 * Lichobì¾níková funkce pøíslu¹nosti.
 * @ingroup fuzzy
 */
class FuzzyTrapez: public FuzzyMembershipFunction 
{
    double x0, x1, x2, x3;
  public:
    /**
     * It assigns word value to the membership function and sets definition values.<br>
     * Pøiøadí slovní hodnotu funkci pøíslu¹nosti a nastaví definièní hodnotu.
     * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
     * @param a A left down vertex.<br>Levý spodní vrchol.
     * @param b A left top vertex.<br>Levý horní vrchol.
     * @param c A right top vertex.<br>Pravý horní vrchol.
     * @param d A right down vertex.<br>Pravý spodní vrchol.
     */
    // implemented in fuzzymf.cc
    FuzzyTrapez(const char *name, double a, double b, double c, double d);
    /**
     * It assigns word value to the membership function.<br>
     * Pøiøadí slovní hodnotu funkci pøíslu¹nosti.
     * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
     */
    // implemented in fuzzymf.cc
    FuzzyTrapez(const char* name);
    /** Destructor. */
    virtual ~FuzzyTrapez() { TRACE(printf("~FuzzyTrapez()\n")); }
    /** It duplicates object.<br>Duplikuje objekt. */
    // implemented in fuzzymf.cc
    virtual FuzzyTrapez *clone() const;
    /** It computes function value (membership).<br>Vypoète funkèní hodnotu (pøíslu¹nost). */
    // implemented in fuzzymf.cc
    virtual double Membership(double x) const;
//    virtual double center() const { return (x1+x2)/2; } // bad### 
    /** Center of this function.<br>Støed této funkce. */
    virtual double center() const { return (x0+2*x1+2*x2+x3)/6; } 
    /** First occurence of maximum in this function.<br>První výskyt maxima v této funkci. */
    virtual double min1() const { return x1; }
    /** Last occurence of maximum in this function.<br>Poslední výskyt maxima v této funkci. */
    virtual double max1() const { return x2; }
    /** It retuns number of definition values.<br>Vrátí poèet definièních hodnot. */
    virtual int getNumValues() { return 4; } 
    /** This adds next definition value.<br>Pøidá dal¹í definièní hodnotu. */
    // implemented in fuzzymf.cc
    virtual void addDefValue(double value);
};// FuzzyTrapez

/////////////////////////////////////////////////////////////////////////////
// FuzzyGauss ---
/**
 * Gaussian membership function.<br>
 * Gaussovská funkce pøíslu¹nosti.
 * @ingroup fuzzy
 */
class FuzzyGauss: public FuzzyMembershipFunction 
{
    double sigma, c, twoSqrSigma;
  public:
    /**
     * It assigns word value to the membership function and sets definition values.<br>
     * Pøiøadí slovní hodnotu funkci pøíslu¹nosti a nastaví definièní hodnoty.
     * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
     * @param center A center of function.(mi) <br>Støed funkce. (mi)
     * @param radius A radius of function. (3*sigma)<br>Polomìr funkce. (3*sigma)
     */
    // implemented in fuzzymf.cc
    FuzzyGauss(const char *name, double center, double radius);
    /**
     * It assigns word value to the membership function.<br>
     * Pøiøadí slovní hodnotu funkci pøíslu¹nosti.
     * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
     */
    // implemented in fuzzymf.cc
    FuzzyGauss(const char* name);
    /** Destructor. */
    virtual ~FuzzyGauss() { TRACE(printf("~FuzzyGauss()\n")); }
    /** It duplicates object.<br>Duplikuje objekt. */
    // implemented in fuzzymf.cc
    virtual FuzzyGauss *clone() const;
    /** It computes function value (membership).<br>Vypoète funkèní hodnotu (pøíslu¹nost). */
    // implemented in fuzzymf.cc
    virtual double Membership(double x) const;
    /** Center of this function.<br>Støed této funkce. */
    virtual double center() const { return c; }
    /** First occurence of maximum in this function.<br>První výskyt maxima v této funkci. */
    virtual double min1() const { return c; }
    /** Last occurence of maximum in this function.<br>Poslední výskyt maxima v této funkci. */
    virtual double max1() const { return c; }
    /** It retuns number of definition values.<br>Vrátí poèet definièních hodnot. */
    int getNumValues() { return 2; } 
    /** This adds next definition value.<br>Pøidá dal¹í definièní hodnotu. */
    // implemented in fuzzymf.cc
    virtual void addDefValue(double value);
};// FuzzyGauss

/**
 * Nonsymetric Gaussian membership function.<br>
 * Nesymetrická Gaussovská funkce pøíslu¹nosti.
 * @ingroup fuzzy
 */
class FuzzyGauss2 : public FuzzyMembershipFunction 
{
  public:
    /**
     * It assigns word value to the membership function and sets definition values.<br>
     * Pøiøadí slovní hodnotu funkci pøíslu¹nosti a nastaví definièní hodnoty.
     * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
     * @param leftCenter A center of the left function.(mi) <br>Støed levé funkce. (mi)
     * @param leftRadius A radius of the left function. (3*sigma)<br>Polomìr levé funkce. (3*sigma)
     * @param rightCenter A center of the right function.(mi) <br>Støed pravé funkce. (mi)
     * @param rightRadius A radius of the right function. (3*sigma)<br>Polomìr pravé funkce. (3*sigma)
     */
    // implemented in fuzzymf.cc
    FuzzyGauss2(const char* name, 
                double leftCenter, double leftRadius, 
                double rightCenter, double rightRadius);
    /**
     * It assigns word value to the membership function.<br>
     * Pøiøadí slovní hodnotu funkci pøíslu¹nosti.
     * @param name Word value for this function.<br>Slovní hodnota pro tuto funkci
     */
    // implemented in fuzzymf.cc
    FuzzyGauss2(const char* name);
    /** Destructor. */
    virtual ~FuzzyGauss2() { TRACE(printf("~FuzzyGauss2()\n")); }
    /** It duplicates object.<br>Duplikuje objekt. */
    // implemented in fuzzymf.cc
    virtual FuzzyGauss2 *clone() const;
    /** It computes function value (membership).<br>Vypoète funkèní hodnotu (pøíslu¹nost). */
    // implemented in fuzzymf.cc
    virtual double Membership(double x) const;
    /** Center of this function.<br>Støed této funkce. */
    // implemented in fuzzymf.cc
    virtual double center() const; 
    /** First occurence of maximum in this function.<br>První výskyt maxima v této funkci. */
    // implemented in fuzzymf.cc
    virtual double min1() const;
    /** Last occurence of maximum in this function.<br>Poslední výskyt maxima v této funkci. */
    // implemented in fuzzymf.cc
    virtual double max1() const;
    /** It retuns number of definition values.<br>Vrátí poèet definièních hodnot. */
    int getNumValues() { return 4; } 
    /** This adds next definition value.<br>Pøidá dal¹í definièní hodnotu. */
    // implemented in fuzzymf.cc
    virtual void addDefValue(double value);
  protected:
    double leftSigma;     /**< A radius of the left function. */
    double leftCenter;    /**< A center of the left function. */
    double rightSigma;    /**< A radius of the right function. */
    double rightCenter;   /**< A center of the right function. */
    /** Position of vertex when leftCenter > rightCenter.<br>Pozice vrcholu, kdy¾ leftCenter > rightCenter. */
    // implemented in fuzzymf.cc
    double vertexPosition() const;
  private:
    double twoSqrSigmaL;  /**< 2*leftSigma*leftSigma */
    double twoSqrSigmaR;  /**< 2*rightSigma*rightSigma */
};

// TODO: add other mf

/////////////////////////////////////////////////////////////////////////////
// FuzzySet --- definition of all membership functions for fuzzy set
//
// TODO: make INTERNAL to fuzzy set, use reference counting???
//       add load/store 
//       dynamic change of parameters
//
/**
 * Implementation of a fuzzy set.<br>
 * Implementace fuzzy mno¾iny.
 * @ingroup fuzzy
 */
class FuzzySet 
{
  protected:
    unsigned n;         /**< actual number of elements */
    enum { MAX=10 };    /**< implementation limit */
    const FuzzyMembershipFunction *array[MAX]; /**< is owner of objects on pointers */
    double xmin, xmax;  /**< limits */
    char * Name;        /**< name of this set */
  public:
    /**
     * Creates fuzzy set.
     * @param min Minimal value of universum.<br>Spodní mez univerza.
     * @param max Maximal value of universum.<br>Horní mez univerza.
     */
    // implemented in fuzzy.cc
    FuzzySet(const char * name, double min, double max, 
     const FuzzyMembershipFunction &m1);
    /**
     * Creates fuzzy set.
     * @param min Minimal value of universum.<br>Spodní mez univerza.
     * @param max Maximal value of universum.<br>Horní mez univerza.
     */
    // implemented in fuzzy.cc
    FuzzySet(const char * name, double min, double max, 
     const FuzzyMembershipFunction &m1,
		 const FuzzyMembershipFunction &m2);
    /**
     * Creates fuzzy set.
     * @param min Minimal value of universum.<br>Spodní mez univerza.
     * @param max Maximal value of universum.<br>Horní mez univerza.
     */
    // implemented in fuzzy.cc 
    FuzzySet(const char * name, double min, double max, 
     const FuzzyMembershipFunction &m1,
		 const FuzzyMembershipFunction &m2,
		 const FuzzyMembershipFunction &m3);
    /**
     * Creates fuzzy set.
     * @param min Minimal value of universum.<br>Spodní mez univerza.
     * @param max Maximal value of universum.<br>Horní mez univerza.
     */
    // implemented in fuzzy.cc 
    FuzzySet(const char * name, double min, double max, 
     const FuzzyMembershipFunction &m1,
		 const FuzzyMembershipFunction &m2,
		 const FuzzyMembershipFunction &m3,
		 const FuzzyMembershipFunction &m4);
    /**
     * Creates fuzzy set.
     * @param min Minimal value of universum.<br>Spodní mez univerza.
     * @param max Maximal value of universum.<br>Horní mez univerza.
     */
    // implemented in fuzzy.cc 
    FuzzySet(const char * name, double min, double max,
     const FuzzyMembershipFunction &m1,
		 const FuzzyMembershipFunction &m2,
		 const FuzzyMembershipFunction &m3,
		 const FuzzyMembershipFunction &m4,
		 const FuzzyMembershipFunction &m5);
    /**
     * Creates fuzzy set.
     * @param min Minimal value of universum.<br>Spodní mez univerza.
     * @param max Maximal value of universum.<br>Horní mez univerza.
     */
    // implemented in fuzzy.cc 
    FuzzySet(const char * name, double min, double max); 
    /** Destructor removes all membership functions.<br>Destruktor uvolní v¹echny funkce pøíslu¹nosti. */
    // implemented in fuzzy.cc
    virtual ~FuzzySet();
    /** It duplicates object.<br>Duplikuje objekt. */
    // implemented in fuzzy.cc
    virtual FuzzySet *clone() const;
    /**
     * It adds next word value into the universum.<br>Pøidá dal¹í slovní hodnotu do univerza.
     * @param x Membership function representing word value.<br>Funkce pøíslu¹nosti reprezentující slovní hodnotu.
     */
    // implemented in fuzzy.cc
    void add(const FuzzyMembershipFunction &x); 
    /** Number of membership functions.<br>Poèet funkcí pøíslu¹nosti. */
    int count() const { return n; } 
    /** It selects i-th member function.<br>Vybere i-tou funkci pøíslu¹nosti. */
    // implemented in fuzzy.cc
    virtual const FuzzyMembershipFunction *operator[] (int i) const;
    /** It computes i-th function value (membership).<br>Vypoète i-tou funkèní hodnotu (pøíslu¹nost). */
    // implemented in fuzzy.cc
    double Membership(int i, double x) const;
    /** Minimal value of universum.<br>Spodní mez univerza. */
    double min() { return xmin; }
    /** Maximal value of universum.<br>Horní mez univerza. */
    double max() { return xmax; }
    /** Name of this fuzzy set.<br>Jméno této fuzzy mno¾iny. */
    const char * name() const { return Name; }
    /** Word value of i-th membership function.<br>Slovní hodnota i-té funkce pøíslu¹nosti. */
    const char * wordValue(int i) const { return array[i]->wordValue(); }
    /** Minimum of maxims.<br>Minimum z maxim. */
    // implemented in fuzzy.cc
    double min1(); // min of 1
    /** Maximum of maxims.<br>Maximum z maxim. */
    // implemented in fuzzy.cc
    double max1(); // max of 1
    
}; // FuzzySet


/////////////////////////////////////////////////////////////////////////////
// FuzzyVariable --- fuzzy set
//
//TODO: dynamic change of variable type
//
class FuzzyBlock; // forward
/**
 * General fuzzy variable. Baze class for FuzzyInput and FuzzyOutput.<br>
 * Obecná fuzzy promìnná. Bázová tøída pro FuzzyInput a FuzzyOutput.
 * @ingroup fuzzy
 */
class FuzzyVariable : public aContiBlock 
{ 
    FuzzyBlock *where;  /**< location */ 
//    const FuzzySet *m;	/**< pattern: n membership functions, parameters */ 
    FuzzySet *m;        /**< pattern: n membership functions, parameters */ 
    const unsigned n;   /**< number of values */ 
    double *mval;	      /**< membership values, dynamically, size given by m */ 
  public:
    /**
     * It connects fuzzy set with variable. If it is created inside FuzzyBlock then it is not need
     * to call registerOwner. Object FuzzySet is cloned and stored inside. Therefore you must release 
     * memory allocated by t in your way.<br>
     * Spojí fuzzy mno¾inu s fuzzy promìnnou. Pokud je promìnná vytváøena
     * uvnitø tøídy FuzzyBlock, není tøeba volat metodu registerOwner. Objekt FuzzySet je pøi pøedání
     * klonován a ulo¾en uvnitø objektu. Proto musíte uvolnit pamì» alokovanou pro t vy sami.
     * @param t Fuzzy set.<br>Fuzzy mno¾ina.
     */
    // implemented in fuzzy.cc
    FuzzyVariable(const FuzzySet &t);
    /** It releases memory allocated by FuzzySet.<br>Uvolní pamì» alokovanou pro FuzzySet. */
    virtual ~FuzzyVariable() 
    {
       TRACE(printf("~FuzzyVariable()\n")); 
       delete m;
       delete [] mval;
    }
      
    /**
     * It registers owner of this variable.<br>Zaregistruje vlastníka této promìnné.
     * @param owner Owner of this variable.<br>Vlastník této promìnné.
     */
    // implemented in fuzzy.cc
    void registerOwner(FuzzyBlock *owner);  // registration inside owner
    /** Number of members.<br>Poèet èlenù. */
    unsigned count() const { return n; } 
    /** I-th member function.<br>I-tá funkce pøíslu¹nosti. */
    // implemented in fuzzy.cc
    const FuzzyMembershipFunction *mf(int i) const;
    /** It gets center of i-th member function.<br>Vrací støed i-té funkce pøíslu¹nosti. */
    // implemented in fuzzy.cc
    double center(int i) const; 
    /** It gets i-th word value.<br>Vrací i-tou slovní hondotu. */
    // implemented in fuzzy.cc
    const char *wordValue(int i) const; // name of i-th membership function
    /** Get/set fuzzy value.<br>Vra»/nastav fuzzy hodnotu. */
    // implemented in fuzzy.cc
    double &operator[] (int i) const;
    /** Get/set fuzzy value.<br>Vra»/nastav fuzzy hodnotu. */
    double &operator[] (const char *s) const { return mval[search(s)]; }
    /** Search by member name.<br>Hledá podle jména. */
    // implemented in fuzzy.cc
    unsigned search(const char *s) const; 
    
    /** Fuzzify all membership functions.<br>Fuzzifikuje v¹echny funkce pøíslu¹nosti.*/
    // implemented in fuzzyio.cc
    void Fuzzify(double x);
    /**
     * Initializes all values for membership functions or fuzzify all value<br>
     * Inicializuje v¹echny hodnoty funkcí pøíslu¹nosti nebo provede fuzzifikaci.
     */
    virtual void Init() { for(unsigned i=0; i!=n; i++) mval[i]=0.0F; }
    virtual void Done() {}
    /** It gets owner.<br>Vrací vlastníka. */
    FuzzyBlock *Where()           // location
    { 
//      if(where==0) SIMLIB_error("Fuzzy set should be used inside FuzzyBlock only");
      return where; 
    }
    // implemented in fuzzy.cc
    void Print(); // print contents
  private:
    // implemented in fuzzy.cc
    double SetMembership(int i, double x);
}; // FuzzyVariable

/////////////////////////////////////////////////////////////////////////////
// FuzzyInput --- continuous input fuzzy variable
//
/**
 * Input varible. It fuzzifies inputs. I.e. it gets Input value and transforms this
 * sharp value into fuzzy reprezentation.<br>
 * Vstupní promìnná. Fuzzifikuje vstup. To znamená, ¾e vezme ostrou vstupní hodnotu 
 * (Input) a transformuje ji do fuzzy reprezentace.
 * @ingroup fuzzy
 */
class FuzzyInput : public FuzzyVariable {
    Input in; /**< continuous input */ 
  public:
    /**
     * It assignes continuous input and fuzzy set into this object.<br>
     * Pøiøadí spojitý vstup a fuzzy mno¾inu do tohoto objektu.
     * @param i Continuous input.<br>Spojitý vstup.
     * @param t Fuzzy set.<br>Fuzzy mno¾ina.
     */
    FuzzyInput(Input i, const FuzzySet &t) : FuzzyVariable(t), in(i) {}
    /**
     * It assignes only fuzzy set into this object. Continuous input must be 
     * added by setInput method. <br>
     * Pøiøadí tomuto objektu pouze fuzzy mno¾inu. Spojitý vstup musí být
     * pøidán pomocí metody setInput.
     * @param t Fuzzy set.<br>Fuzzy mno¾ina.
     */
    FuzzyInput(const FuzzySet &t) : FuzzyVariable(t), in(0.0) {}
    /** Destructor. */
    virtual ~FuzzyInput() { TRACE(printf("~FuzzyInput()\n")); }
    /**
     * It adds continuous input into this object.<br>
     * Pøidá spojitý vstup do tohoto objektu.
     * @param i Continuous input.<br>Spojitý vstup.
     */
    void setInput(Input i) { in = i; }
    /**
     * It returns sharp input value.<br>Vrací ostrou vstupní hodnotu.
     * @return It returns sharp input value.<br>Vrací ostrou vstupní hodnotu.
     */
    virtual double Value() { return in.Value(); }
    /**
     * It fuzzifies continuous input. This method is called every time before call 
     * to FuzzyBlock::Behavior().<br>
     * Fuzzifikuje spojitý vstup. Tato metoda je volána v¾dy pøed voláním 
     * FuzzyBlock::Behavior().
     */
    virtual void Init() { Fuzzify(in.Value()); }
//    virtual void Done() {}
};// FuzzyInput


/////////////////////////////////////////////////////////////////////////////
// FuzzyOutput --- continuous output (aggregation, defuzzification)
//
/**
 * Output variable. It defuzzifies fuzzy values, which is obtained by processing
 * inference rules.<br>
 * Výstupní promìnná. Defuzzifikuje fuzzy hodnoty, které jsou získány zpracováním
 * inferenèních pravidel.
 * @ingroup fuzzy
 */
class FuzzyOutput: public FuzzyVariable {
    double value; /**? value after defuzzification */ 
    double (*defuzzify)(const FuzzyVariable&); /**< defuzzification function */  // remove!!!!!!!####
  public:
    /**
     * It adds fuzzy set and defuzzification function.<br>
     * Pøidá fuzzy mno¾inu a defuzzifikaèní funkci.
     * @param t   Fuzzy set.<br>Fuzzy mno¾ina.
     * @param def Defuzzification function.<br>Defuzzifikaèní funkce.
     */
    // implemented in fuzzyrul.cc
    FuzzyOutput(const FuzzySet &t, double (*def)(const FuzzyVariable&)=0);
    /**
     * It assignes fuzzy value by word value.<br>Pøiøadí fuzzy hodnotu pomocí slovní hodnoty.
     */
    // implemented in fuzzyrul.cc
    const char *operator = (const char *val);
    /** 
     * It defuzzifies itself.<br> Defuzzifikuje se. 
     * @return It returns sharp value.<br>Vrátí osrou hodnotu.
     */
    // implemented in fuzzyrul.cc
    double Defuzzify();
    /** It adds defuzzification function.<br>Pøidá defuzzifikaèní funkci. */
    // implemented in fuzzyrul.cc
    void SetDefuzzifyMethod(double (*f)(const FuzzyVariable&));
    /**
     * Evaluates FuzzyBlock (variable where) and after defuzzification returns sharp value.<br>
     * Vyhodnotí FuzzyBlock (promìnnou whera) a po defuzzifikaci vrátí ostrou hodnotu.
     * @return Sharp value after defuzzification.<br>Ostrá hodnota po defuzzifikaci.
     */
    // implemented in fuzzyrul.cc
    double Value();
//inherited:  virtual void Init() { for(int i=0; i<n; i++) mval[i]=0.0F; }
    /** 
     * It defuzzifies itself.<br> Defuzzifikuje se. 
     */
    virtual void Done() { Defuzzify(); }
    // implemented in fuzzyrul.cc
    virtual ~FuzzyOutput();
}; // FuzzyOutput

/////////////////////////////////////////////////////////////////////////////
// defuzzification methods
// 

// TODO: function objects   X x(params)      a=x(fset)
// indexed-centre-of-gravity
//double i_g_centre(const FuzzyVariable &fs, double lim=0.0F);
//double centroid(const FuzzyVariable &fs);
// implemented in fuzzyio.cc
/**
 * @ingroup fuzzy
 * Defuzzification method "mean-of-maximum".<br>Defuzifikaèní metoda "støed maxim". 
 */
double defuzMeanOfMax(const FuzzyVariable &fs);
/**
 * @ingroup fuzzy
 * Defuzzification method "min-of-maximum".<br>Defuzifikaèní metoda "minimální maximum". 
 */
// implemented in fuzzyio.cc
double defuzMinOfMax(const FuzzyVariable &fs);  
/** 
 * @ingroup fuzzy
 * Defuzzification method "max-of-maximum".<br>Defuzifikaèní metoda "maximální maximum". 
 */
// implemented in fuzzyio.cc
double defuzMaxOfMax(const FuzzyVariable &fs); 
/**
 * @ingroup fuzzy
 * Defuzzification method "discrete-center-of-gravity".<br>Defuzifikaèní metoda "diskrétní tì¾i¹tì". 
 */
// implemented in fuzzyio.cc
double defuzDCOG(const FuzzyVariable &fs);

/////////////////////////////////////////////////////////////////////////////
// FuzzyBlock --- base class for inference blocks
// 
/**
 * Base class for inference blocks. Representation of fuzzy regulator. <br>
 * Bázová tøída pro inferenèní bloky. Reprezentace fuzzy regulátoru.
 * @ingroup fuzzy
 */
class FuzzyBlock 
{
  protected:
    FuzzyBlock *where;            /**< position in hierarchical structure */
    double lastTime;              /**< time of last evaluation */ 
    virtual void Behavior() = 0;  /**< user defined fuzzy rules */ 
    dlist<FuzzyVariable*> vlist;  /**< all fuzzy variables in the block */ 
  public:
    /**
     * If inference rules are specified by FuzzyExpr way then you must call 
     * EndConstructor method on the end of constructor.<br>
     * Jestli¾e jsou inferenèní pravidla specifikována pomocí FuzzyExpr, pak musíte
     * zavolat metodu EndConstructor na konci konstruktoru.
     */
    // implemented in fuzzyrul.cc
    FuzzyBlock();
    /**
     * It evaluates whole block. It calls method Behavior.<br>
     * Vyhodnotí celý blok. Volá metodu Behavior.
     */
    // implemented in fuzzyrul.cc
    virtual void Evaluate();              // calls Behavior()
    /**
     * If inference rules are specified by FuzzyExpr way then you must call this
     * method on the end of constructor.<br>
     * Jestli¾e jsou inferenèní pravidla specifikována pomocí FuzzyExpr, pak musíte
     * zavolat tuto metodu na konci konstruktoru.
     */
    // implemented in fuzzyrul.cc
    void EndConstructor();        // ### marks end, should be called in constructor
    /**
     * It registers fuzzy variable inside this object. If inference rules are NOT specified 
     * by FuzzyExpr way then you must call this method inside the constructor for all FuzzyInput
     * and FuzzyOutput variables.<br>
     * Registruje fuzzy promìnnou v tomto objektu. Jestli¾e NEjsou inferenèní pravidla 
     * specifikována pomocí FuzzyExpr, pak musíte zavolat tuto metodu v konstruktoru pro v¹echny
     * promìnné typu FuzzyInput a FuzzyOutput.
     */
    // implemented in fuzzyrul.cc
    void Register(FuzzyVariable *obj); // register variable in vlist
    virtual ~FuzzyBlock() { TRACE(printf("~FuzzyBlock()\n")); }
}; // FuzzyBlock


/////////////////////////////////////////////////////////////////////////////
// Author: David Martniek
/**
 * Base class for inference blocks with sampled input.<br>
 * Bázová tøída pro inferenèní bloky se vzorkovaným vstupem
 * @ingroup fuzzy
 */
class FuzzySampledBlock
: public FuzzyBlock
{
    /**
     * Inner class for sampling  
     * Vnitøní tøída pro vzorkování
     * @ingroup fuzzy
     */
    class FSampler 
    : public Event
    {
      
      double timeStep;
      FuzzySampledBlock *parent;
      public:
        /**
         * Creates sampler<br>Vytvoøí vzorkovaè.
         * @param parent owner of this class<br>vlastník této tøídy
         */
        FSampler(FuzzySampledBlock *parent) 
        { 
          this->parent = parent;
          timeStep = 0;
        }
        ~FSampler() { TRACE(printf("~FSampler()\n"));  parent = 0; }
        /** It sets the time step for sampling.<br> Nastaví èasový krok pro vzorkování. */
        void setTimeStep(double timeStep) { this->timeStep = timeStep; }
        /** It gets time step.<br>Vrátí èasový krok. */
        double getTimeStep() { return timeStep; }
        /**
         * Sampling - it evaluates the parent object and shedules next activacion.<br>
         * Vzorkování - vyhodnotí rodièovský objekt a naplánuje dal¹í spu¹tìní.
         */
        void Behavior()
        {
          parent->Evaluate();
          Activate(Time+timeStep);
        }
    };
  
    FSampler *sampler;

    /**
     * Will contain user defined inference rules.<br>
     * Bude obsahovat u¾ivatelem definovaná inferenèní pravidla.
     */
    virtual void Behavior() = 0;  
  public:
    /**
     * It creates and initializes the sampler object.
     * If inference rules are specified by FuzzyExpr way then you must call 
     * EndConstructor method on the end of constructor.<br>
     * Vytvoøí a inicalizuje vzorkovaè.
     * Jestli¾e jsou inferenèní pravidla specifikována pomocí FuzzyExpr, pak musíte
     * zavolat metodu EndConstructor na konci konstruktoru.
     */
    FuzzySampledBlock() { sampler = new FSampler(this); }
    virtual ~FuzzySampledBlock() { TRACE(printf("~FuzzySampledBlock")); }
    /**
     * It sets the time step for sampling.<br>
     * Nastaví èasový krok pro vzorkování.
     */
    void setTimeStep(double timeStep) { sampler->setTimeStep(timeStep); }
    /** It gets time step.<br> Vrátí èasový krok. */
    double getTimeStep() { return sampler->getTimeStep(); }
    /** It starts sampling.<br> Nastartuje vzorkování. */
    void Start() { sampler->Activate(); }
    /**
     * It evaluates fuzzy inference rules but only in sampled time steps.<br>
     * Vyhodnotí fuzzy inferenèní pravidla, ale jenom ve vzorkovaných èasových okam¾icích.
     */
    // implemented in fuzzyrul.cc
    virtual void Evaluate();
}; // FuzzySampledBlock

/////////////////////////////////////////////////////////////////////////////
// rules
// 
class FuzzyRule;
class FuzzyRuleFactory;

/**
 * Abstract class for representation and evaluation of inference rules.
 * It defines interface for generaly inputs, outputs and inference rules insertion.
 * It does not implement particular format form storing inference rules.<br>
 * 
 * Abstraktní tøída pro reprezentaci a vyhodnocení inferenèních pravidel. 
 * Definuje rozhraní pro obecné vkládání vstupù, výstupù a inferenèních pravidel.
 * Neimplementuje konkrétní formát ulo¾ení pravidel.
 *
 * @author David Martinek <a href="mailto://DavidMartinek@seznam.cz">DavidMartinek@seznam.cz</a>
 * @version 1.0
 * @ingroup fuzzy
 */
class FuzzyInferenceRules
{
  friend class FuzzyRuleFactory;
  public:
    /** 
     * Operations for use inside inference rules.<br>
     * Operace, které je mo¾no pou¾ít uvnitø inferenèních pravidel. 
     */
    enum Operations { opAND, opOR, opNOT, opNAND, opNOR };
    
    /**
     * Destructor - does NOT free memory allocated by FuzzyInput and FuzzyOutput!<br>
     *
     * Destruktor - NEuvolòuje pamì» zabranou promìnnými FuzzyInput a FuzzyOutput! 
     * Uvolnìní této pamìti se musí provést JINDE!
     */
    virtual ~FuzzyInferenceRules()
    {
      TRACE(printf("~FuzzyInferenceRules"));
      in.erase(in.begin(), in.end()); 
      out.erase(out.begin(), out.end());
    }
    
    /**
     * It adds next input variable. Before calling this method is recommended to call
     * isComplete.<br>
     * 
     * Vlo¾í dal¹í vstupní promìnnou. Pøed voláním je vhodné pou¾ít funkci isComplete.
     * @param in Ipnut fuzzy variable. It has not to be registered inside FuzzyBlock yet.
     *           Vstupní fuzzy promìnná. Zde je¹tì nemusí být registrovaná uvnitø FuzzyBlock.
     * 
     * @see isComplete()
     */
    virtual void addFuzzyInput(FuzzyInput *in)
    { this->in.push_back(in); }

    /**
     * It adds next output variable. Before calling this method is recommended to call
     * isComplete. <br>
     *
     * Vlo¾í výstupní promìnnou. Pøed voláním je vhodné pou¾ít funkci isComplete. 
     * @param out Output fuzzy variable. It has not to be registered inside FuzzyBlock yet.
     *            Výstupní fuzzy promìnná. Zde je¹tì nemusí být registrovaná uvnitø FuzzyBlock.
     * @see isComplete()
     */
    virtual void addFuzzyOutput(FuzzyOutput *out)
    { this->out.push_back(out); }
    
    /**
     * It safely creates FuzzyRuleFactory.<br>
     * Bezpeèným zpùsobem vytvoøí FuzzyRuleFactory.
     */
    //implemented in rules.cc
    virtual FuzzyRuleFactory * createRuleFactory();

    /**
     * It returns true when all variables are assigned.<br>
     * Testuje, jestli u¾ jsou pøiøazeny v¹echny promìnné.
     * @see addFuzzyInput(FuzzyInput *in)
     * @see addFuzzyOutput(FuzzyOutput *out)
     */
    virtual bool isComplete() = 0;
    
    /**
     * It adds next rule into list. When it is too much rules here, error is indicated.<br>
     * 
     * Pøidá dal¹í pravidlo do seznamu. Pokud u¾ je definováno pøíli¹ pravidel, nastane chyba.
     * @param rule Inference rule who is represented by tree structure.
     *             Inferenèní pravidlo reprezentované stromovou strukturou.
     * @param release Øíká, ¾e se po vlo¾ení uvolní pamì». Implicitnì nastaveno true.
     *                Memory will be released after rule storing. Default it is true.
     */
    virtual void add(FuzzyRule *rule, bool release=true) = 0;
    
    /**
     * It evaluates all rules. This method is defaultly called from a method Behavior in class
     * FuzzyRSBlock.<br>
     * Vyhodnotí pravidla. Tato funkce je standardnì volána z funkce Behavior tøídy 
     * FuzzyRSBlock.
     */
     virtual void evaluate() = 0;
  protected:
    /** Vector of input variables. <br> Vektor vstupních promìnných. */
    std::vector<FuzzyInput *>in;
    
    /** Vector of output variables. <br> Vektor výstupních promìnných. */
    std::vector<FuzzyOutput *>out;
  private:
}; // FuzzyInferenceRules

/**
 * This class is representing inference rules for classic fuzzy regulator with two inputs and
 * one output. It is possible to adds rules by new defined methods add(). These methods are
 * suitable for use during XML analysis.<br>
 * 
 * Tøída reprezentující inferenèní pravidla pro klasický fuzzy regulátor se dvìma vstupy a 
 * jedním výstupem. Pravidla je mo¾né vkládat pomocí nových funkcí add, které jsou vhodné
 * pro pou¾ití bìhem analýzy dat z XML souboru. Kromì toho je mo¾no pou¾ít zdìdìné rozhraní
 * FuzzyInferenceRules
 *
 * @author David Martinek <a href="mailto://DavidMartinek@seznam.cz">DavidMartinek@seznam.cz</a>
 * @version 1.0
 * @ingroup fuzzy
 */
class FuzzyIIORules 
: public FuzzyInferenceRules
{
  public:
/////////////////////////////////////////////////////////////////
// rozhraní FuzzyInferenceRules -- interface FuzzyInferenceRules
/////////////////////////////////////////////////////////////////
    /**
     * Parameterless constructor. Fuzzy variables must be added explicitly.<br>
     * Konstruktor bez parametru. Fuzzy promìnné je nutno pøiøadit pomocí dal¹ích funkcí explicitnì.
     */
    FuzzyIIORules();
    
    /**
     * It initializes all input and output variables.<br>
     * Inicializuje v¹echny vstupní a výstupní promìnné.
     */
    //implemented in rules.cc
    FuzzyIIORules(FuzzyInput *in1, FuzzyInput *in2, FuzzyOutput *out);
    
    /**
     * Destructor - does NOT free memory allocated by FuzzyInput and FuzzyOutput!<br>
     * Destruktor - NEuvolòuje pamì» zabranou promìnnými FuzzyInput a FuzzyOutput! 
     * Uvolnìní této pamìti se musí provést JINDE!
     */
    //implemented in rules.cc
    virtual ~FuzzyIIORules();
    
    /** It initializes all member variables. <br> Inicializuje vnitøní promìnné */
    //implemented in rules.cc
    void init();
     
    /** It adds next input variable.<br> Vlo¾í dal¹í vstupní promìnnou. */
    //implemented in rules.cc
    virtual void addFuzzyInput(FuzzyInput *in);

    /** It adds output variable.<br> Vlo¾í výstupní promìnnou. */
    //implemented in rules.cc
    virtual void addFuzzyOutput(FuzzyOutput *out);

    /**
     * It tests if all variables are assigned.<br>
     * Testuje, jestli u¾ jsou pøiøazeny v¹echny promìnné.
     * @see addFuzzyInput(FuzzyInput *in)
     * @see addFuzzyOutput(FuzzyOutput *out)
     */
     //implemented in rules.cc
    virtual bool isComplete();

    /**
     * It evaluates the rules.<br>
     * Vyhodnotí pravidla.
     */
     //implemented in rules.cc
    virtual void evaluate();

    /**
     * It adds next rule into list. If there is too much rules, an error is indicated.<br>
     * Pøidá dal¹í pravidlo do seznamu. Pokud u¾ je definováno pøíli¹ pravidel, nastane chyba.
     * @param rule Inference rule with tree structure.<br>Inferenèní pravidlo reprezentované stromovou strukturou.
     * @param release Memory will be released after rule storing. Default it is true.
     *                Øíká, ¾e se po vlo¾ení uvolní pamì». Implicitnì nastaveno true.
     */
     //implemented in rules.cc
    virtual void add(FuzzyRule *rule, bool release=true);
    
//////////////////////////////////////
// nové funkce -- new functions
//////////////////////////////////////
    /**
     * Adds a rule into the list.<br>
     * Pøídá inferenèní pravidlo do seznamu.
     * @param operation     operation which is used in rule<br>operace, která se pou¾ije v pravidle
     * @param in1WordValue  word value of first fuzzy input variable<br>slovní hodnota první vstupní fuzzy promìnné
     * @param in2WordValue  word value of second fuzzy input variable<br>slovní hodnota druhé vstupní fuzzy promìnné
     * @param outWordValue  word value of output fuzzy variable<br>slovní hodnota výstupní fuzzy promìnné
     */
     //implemented in rules.cc
    void add (const Operations operation, 
              const char *in1WordValue, 
              const char *in2WordValue, 
              const char *outWordValue);
    
    /**
     * Adds a rule into the list.<br>
     * Pøídá inferenèní pravidlo do seznamu.
     * @param operation   operation which is used in rule<br>operace, která se pou¾ije v pravidle
     * @param in1WVIndex  index of first fuzzy input variable<br>index slovní hodnoty první vstupní promìnné
     * @param in2WVIndex  index of second fuzzy input variable<br>index slovní hodnoty druhé vstupní promìnné
     * @param outWVIndex  index of output fuzzy variable<br>index slovní hodnoty výstupní promìnné
     */
     //implemented in rules.cc
    void add (const Operations operation, 
              int in1WVIndex, 
              int in2WVIndex, 
              int outWVIndex);
    /**
     * Adds a rule into the list.<br>
     * Pøídá inferenèní pravidlo do seznamu.
     * @param operation     operation which is used in rule<br>operace, která se pou¾ije v pravidle
     * @param in1WVIndex    index of first fuzzy input variable<br>index slovní hodnoty první vstupní promìnné
     * @param in2WVIndex    index of second fuzzy input variable<br>index slovní hodnoty druhé vstupní promìnné
     * @param outWordValue  word value of output fuzzy variable<br>slovní hodnota výstupní fuzzy promìnné
     */
     //implemented in rules.cc
    void add (const Operations operation, 
              int in1WVIndex, 
              int in2WVIndex, 
              const char *outWordValue);

  protected:
    /** Array of indexes into FuzzyOutput variable.<br> Pole indexù do promìnné FuzzyOutput. */
    int *outWV; 
    /** 
     * Array of corresponding operations between input variables.<br> Pole odpovídajících
     * operací mezi vstupními promìnnými.
     */
    Operations *operation;
    /** Number of variables and rules in object.<br> Poèet promìnných a pravidel v objektu */
    int inputs, outputs, rules;
    /** Maximum number of variables.<br> Maximální poèet promìnných. */
    static const int MAX_INPUTS = 2;
    static const int MAX_OUTPUTS = 1;
    
    /** It tests if all arrays are allocated.<br>Testuje, jestli u¾ jsou alokována pole. */
    //implemented in rules.cc
    bool isAllCreated();
  private:
    /**
     * It alocates a memory space for operation and outWV arrays. A alocated array size is
     * a product of numbers of word values of all input variables.<br>
     * Alokuje prostor pro pole operation a outWV. Velikost alokovaného pole je souèinem 
     * poètù slovních hodnot v¹ech jednotlivých vstupních promìnných.
     */
     //implemented in rules.cc
    void createVectors();
}; // FuzzyIIORules

/**
 * General form of inference rules.<br>Obecný tvar inferenèních pravidel.
 *
 * General representation of inference rules, which allows to create more complex fuzzy regulators.
 * Usualy is this class used indirectly over overloaded operators. For more details see the example
 * for class FuzzyExpr and documentation for class Rules.
 *
 * Obecná reprezentace inferenèních pravidel, která dovoluje vytváøet slo¾itìj¹í fuzzy regulátory.
 * Obvykle je tato tøída pou¾ívaná nepøímo pøes pøetí¾ené operátory. Dal¹í detaily viz pøíklad 
 * u tøídy FuzzyExpr a dokumentace pro tøídu Rules.
 */
class FuzzyGeneralRules
: public FuzzyInferenceRules
{
  public:
    /**
     * It destroys vector of rules
     */
    ~FuzzyGeneralRules()
    {
      TRACE(printf("~FuzzyGeneralRules\n"));
      rules.erase(rules.begin(), rules.end());
    }
    /**
     * It returns true when all variables are assigned.<br>
     * Testuje, jestli u¾ jsou pøiøazeny v¹echny promìnné.
     * @see addFuzzyInput(FuzzyInput *in)
     * @see addFuzzyOutput(FuzzyOutput *out)
     */
     //implemented in rules.cc
    virtual bool isComplete();

    /**
     * It adds next rule into list. When it is too much rules here, error is indicated.<br>
     * 
     * Pøidá dal¹í pravidlo do seznamu. Pokud u¾ je definováno pøíli¹ pravidel, nastane chyba.
     * @param rule Inference rule who is represented by tree structure.<br>
     *             Inferenèní pravidlo reprezentované stromovou strukturou.
     * @param release It has no meaning here.<br>Zde nemá význam.
     */
    //implemented in rules.cc
    virtual void add(FuzzyRule *rule, bool release=true);

    /**
     * It evaluates all rules. This method is defaultly called from a method Behavior in class
     * FuzzyRSBlock.<br>
     * Vyhodnotí pravidla. Tato funkce je standardnì volána z funkce Behavior tøídy 
     * FuzzyRSBlock.
     */
    //implemented in rules.cc
    virtual void evaluate();
  protected:
    /** Vector of rules. */
    std::vector<FuzzyRule *> rules;
  private:
}; // FuzzyGeneralRules

/**
 * FuzzyRSBlock is base class for inference blocks with sampled input and explicitly defined
 * inference rules.<br>
 * FuzzyRSBlock je bázová tøída pro inferenèní bloky se vzorkovanýxm vstupem a  explicitnì
 * definovanými inferenèními pravidly.
 * @ingroup fuzzy
 */
class FuzzyRSBlock
: public FuzzySampledBlock
{
  public:
    /** 
     * Constructor - sets object with inference rules.<br>
     * Konstruktor - nastaví objekt s inferenèními pravidly.  
     */
    FuzzyRSBlock(FuzzyInferenceRules &r) :rules(r) { }
    
    /** Destructor - destruktor. */
    virtual ~FuzzyRSBlock() 
    { TRACE(printf("~FuzzyRuledSampledBlock()\n")); }

    /**
     * It specifies behavior of this block. It evaluates all inference rules. <br>
     * Specifikuje chování tohoto bloku. Vyhodnotí v¹echna inferenèní pravidla.
     */
    virtual void Behavior()
    { rules.evaluate(); }  

  protected:
    FuzzyInferenceRules &rules;
}; // FuzzyRSBlock

///////////////////////////////////////////////////////////////////////////////////////////////
// FuzzyRule
// Reprezentace pravidla pomocí stromu objektù. Tento zpùsob realizace pravidla není pøíli¹
// efektivní. Na druhou stranu je dostateènì obecný na to, aby se s tím dalo pracovat. Tento
// pøístup je zamý¹len spí¹e tak, ¾e vytváøení pravidel pomocí stromu je vnìj¹í rozhraní, které
// je v rámci mo¾ností u¾ivatelsky pøíjemné. Pravidlo reprezentované tímto stromem se v ka¾dém
// z potomkù tøídy FuzzyInferenceRules pøekonvertuje na nìjaký efektivnìj¹í formát.  
// 
// Pokud by se ov¹em vyskytla potøeba opravdu obecného zápisu pravidel, je mo¾né tento strom
// pou¾ít pøímo. Ve¹keré vyhodnocování stromu se ale musí dìlat zvenèí, proto¾e tøídy tak, jak 
// jsou nyní navr¾eny mohou slou¾it POUZE pro reprezentaci stromu. S pøihlédnutím k tomu, aby
// byla manipulace s objekty co nejjednodu¹¹í, mají v¹echny tøídy v¹echny své datové prvky 
// veøejné. 
///////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Virtual knot in tree. From descendants is created a tree, who represents inference rule.<br>
 * Virtuální uzel ve stromu. Z potomkù se vytváøí strom, který reprezentuje inferenèní pravidlo.
 * @ingroup fuzzy
 */
class FONode
{
  public:
    /**
     * It does nothing. It is here only to make this class abstract one.<br>
     * Prázdná èistì virtuální funkce, která dìlá z této tøídy abstraktní tøídu.
     */
    /**
     * Destruktor
     */
    virtual ~FONode() { TRACE(printf("~FONode\n")); };
    
    /**
     * It returns value of node - for evaluation of rule.<br>Vrací hodnotu uzlu - pro 
     * vyhodnocování pravidel 
     */
    virtual double getValue() = 0;
}; // FONode

/**
 * Knot in a tree, who represents inference rule. This knot represents an operation.<br>
 * Uzel ve stromu, který reprezentuje inferenèní pravidlo. Tento uzel reprezentuje operaci.
 * @ingroup fuzzy
 */
class FOperation : public FONode
{
  public:
    /** Operator. */
    FuzzyInferenceRules::Operations op;
    
    /** Left operand. <br> Levý operand. */
    FONode *L;
    
    /** Right operand. <br> Pravý operand. */
    FONode *R;
    
    /**
     * Constructor for a binary operator. <br> Konstruktor pro binární operátor.
     */
    FOperation(FONode *left, FONode *right, FuzzyInferenceRules::Operations operation)
    : //FONode(FONode::ntOperation), 
      op(operation), L(left), R(right){}

    /**
     * Constructor for an unary operator. <br> Konstruktor pro unární operátor.
     */
    // implemented in ruletree.cc
    FOperation(FONode *operand, FuzzyInferenceRules::Operations operation);
    
    /**
     * Destructor releases all subtrees. <br> Destruktor uvolní oba podstromy.
     */
    virtual ~FOperation()
    { 
      if (R != NULL) delete R; 
      if (L != NULL) delete L; 
      TRACE(printf("~FOperation\n"));
    }
    
    /**
     * It returns value after doing operation op.<br>Vrací hodnotu po provedení operace op.
     */
    // implemented in ruletree.cc
    virtual double getValue();
    
    
  protected:
  private:
}; // FOperation

/**
 * Leaf of tree. It represents comparing (in == "low") or (in != "too"). If var is 
 * FuzzyOutput, this object can represents assigning (out = "reduce").<br>
 * List stromu. Reprezentuje porovnání typu (in == "malo") pripadne (in != "hodne"). Pokud
 * se do promìnné var pøiøadí FuzzyOutput, mù¾e tento objekt reprezentovat pøiøazení typu
 * (out = "uber".)
 * @ingroup fuzzy
 */
class FPair : public FONode
{
  public:
    /** Equal or not equal. <br> Rovno nebo nerovno. */
    bool eq;
    /** Fuzzy variable. <br> Fuzzy promìnná. */
    FuzzyVariable *var;
    /** Word value. <br> Slovní hodnota. */
    const char * wordValue;
    /** Index of word value in FuzzyVariable.<br> Index slovní hodnoty ve FuzzyVariable. */
    int indexWV;
    
    /**
     * Constructor.
     */
    FPair(FuzzyVariable *variable, const char * wordvalue, bool equal=true) 
    : //FONode(FONode::ntPair), 
      eq(equal), var(variable), wordValue(wordvalue) 
    {
      indexWV = var->search(wordvalue);
    }
    
     /**
      * Destructor does not release memory alocated by var and wordValue.<br>
      * Destruktor neuvolòuje pamì» alokovanou promìnnými var a wordValue.
      */
    virtual ~FPair()
    {
      var = NULL;
      wordValue = NULL;
      TRACE(printf("~FPair\n"));
    }
    
    /**
     * If var is the FuzzyInput then this method returns fuzzified value of input.
     * In other words - value of membership function with name wordValue. Do not use if var is
     * a class FuzzyOutput.
     *
     * Jestli¾e parametr var je FuzzyInput, tato metoda vrací fuzzifikovanou hodnotu vstupu.
     * Jinými slovy - hodnotu funkce pøíslu¹nosti se jménem wordValue. Nepou¾ívejte jestli¾e je 
     * var tøídy FuzzyOutput.
     */
    // implemented in ruletree.cc
    virtual double getValue();
  protected:
  private:
};  // FPair

/**
 * Class for representing one inference rule in tree.<br>
 * Tøída reprezentující jedno inferenèní pravidlo ve formì stromové struktury.
 * @ingroup fuzzy
 */
class FuzzyRule
{
  public:
    /** Constructor. */
     //implemented in ruletree.cc
    FuzzyRule();
    
    /**
     * Destructor releases memory alocated by tree. <br> Destruktor uvolní pamì» alokovanou stromem.
     */
     //implemented in ruletree.cc
    virtual ~FuzzyRule();
    
    /** It adds the lvalue of rule. <br> Pøidá levou stranu pravidla. */
    //implemented in ruletree.cc
    void addLeft(FOperation *left);
    
    /** It adds one command into list. <br> Pøidá dal¹í pøíkaz do seznamu. */
    //implemented in ruletree.cc
    void addRight(FPair *right);
    
    /**
     * It evaluates one inference rule according to fuzzy model Mamdani.<br>
     * Vyhodnotí jedno inferenèní pravidlo podle fuzzy modelu Mamdani. 
     */
    //implemented in ruletree.cc
    virtual void evaluate();
    
    /** Lvalue. <br> Levá strana. */
    FOperation * left;
    
    /** Rvalue. <br> Pravá strana. */
    std::vector<FPair *> right;
  protected:
  private:
}; // FuzzyRule

/**
 * Class for user friendly creating of inference rules.<br>
 * Tøída pro u¾ivatelsky pøíjemnìj¹í vytváøení inferenèních pravidel.
 * @ingroup fuzzy
 */
class FuzzyRuleFactory
{
  public:
    /**
     * It safely creates fuzzy inference rule.<br>
     * Vrací bezpeèným zpùsobem vytvoøené pravidlo.
     */
    // implemented in ruletree.cc
    FuzzyRule * createRule();

    /**
     * It adds condition of rule.<br>
     * Pøidá podmínkovou èást do pravidla.
     */
     //implemented in ruletree.cc
    void addCondition(FOperation * operation);

    /**
     * It adds next assign command into rule.<br>
     * Pøidá dal¹í pøiøazovací pøíkaz do pøíkazové èásti pravidla (konsekvent). 
     */
     //implemented in ruletree.cc
    void addConsequent(FPair * consequent);  
    
    /**
     * It creates a leaf knot of rule. <br>
     * Vytvoøí listový uzel pravidla.
     * @param equal true for == and not for !=
     */
     //implemented in ruletree.cc
    FPair * createNode(FuzzyVariable *var, const char * wordvalue, bool equal=true);
    
    /**
     * It creates a nonleaf knot of rule representing binary operator.<br>
     * Vytvoøí nelistový uzel pravidla reprezentující binární operátor.
     */
     //implemented in ruletree.cc
    FOperation * createNode(FONode *left, FONode *right, FuzzyInferenceRules::Operations operation);
    
    /**
     * It creates a nonleaf knot of rule representing unary operator.<br>
     * Vytvoøí nelistový uzel pravidla reprezentující unární operátor.
     */
     //implemented in ruletree.cc
    FOperation * createNode(FONode *operand, FuzzyInferenceRules::Operations operation);
    
    /** Destructor */
    virtual ~FuzzyRuleFactory()
    {
      if (rule != NULL) delete rule; 
      TRACE(printf("~FuzzyRuleFactory\n"));
    }
  protected:
    /** 
     * Constructor is protected against user. Object of this class can create only
     * object of class FuzzyInferenceRules.<br>
     * Konstruktor je chránìn pøed u¾ivatelem. Objekt této tøídy mù¾e vytvoøit
     * pouze objekt tøídy FuzzyInferenceRules.
     */
    friend class FuzzyInferenceRules;
    // implemented in ruletree.cc
    FuzzyRuleFactory(FuzzyInferenceRules * owner);
    FuzzyInferenceRules * owner; /**< Vlastník tohoto objektu */
    FuzzyRule * rule;            /**< Právì vytváøené pravidlo */
  private:
}; // FuzzyRuleFactory



/**
 * A static class for creating general inference rules. See the class FuzzyExpr and FuzzyGeneralRules.
 *
 * Statická tøída pro vytváøení obecných inferenèních pravidel. Podívejte se na tøídu FuzzyExpr a
 * FuzzyGeneralRules.
 */
class Rules
{
  public:

    /**
     * It adds next input into the list. This is needed because of error checking.<br>
     * Pøidá dal¹í vstup do seznamu. Tato metoda je potøeba kvùli o¹etøení chyb.
     */
    static void addFuzzyInput(FuzzyInput * input);

    /**
     * It adds next output into the list. This is needed because of error checking.<br>
     * Pøidá dal¹í výstup do seznamu. Tato metoda je potøeba kvùli o¹etøení chyb.
     */
    static void addFuzzyOutput(FuzzyOutput * output);
    
    /**
     * It returns complete definition of inference rules. See at class FuzzyExpr to see how to
     * create inference rules.<br>
     * Vrací kompletní definici inferenèních pravidel. Podívejte se na tøídu FuzzyExpr jak se
     * vytváøí inferenèní pravidla.
     */
    static FuzzyGeneralRules * getRules();
    
    /**
     * It produces error if there is not enough inputs and outputs or it returns FuzzyRuleFactory.<br>
     * Zpùsobí chybu, jestli¾e není definováno dostateèné mno¾ství vstupù a výstupù nebo vrátí
     * FuzzyRuleFactory.
     */
    static FuzzyRuleFactory * Factory();
    
    /**
     * It adds new rule into rules.<br>Pøidá nové pravidlo do rules.
     */
    static void addNewRule();
    
    static bool addRule;                  /**< Create new rule in operator ==  */
  private:
    /** This class can not be instanciated. */
    Rules() {}
    /** It initializes data members. */
    static void init();
    static bool addedInput;               /**< Is here any input? */
    static bool addedOutput;              /**< Is here any output? */
    static FuzzyGeneralRules * rules;     /**< Rules to return. */
    static FuzzyRuleFactory * factory;    /**< Rule factory. */
};//class Rules

/////////////////////////////////////////////////////////////////////////////
// FuzzyExpr --- fuzzy expression value class
/**
 * "User friendly" writing of inference rules.<br>"U¾ivatelsky pøátelský zápis" inferenèních pravidel.
 *
 * Implementation of "user friendly" writing of inference rules. This way can not be use
 * inside threads because of using global variables!
 *
 * Implementace "u¾ivatelsky pøátelského" zápisu inferenèních pravidel. Tento zpùsob nemù¾e
 * být pou¾it uvnitø vláken kvùli pou¾ívání globálních promìnných.
 *
 * Example/Pøíklad:
 * @code
 * FuzzyInferenceRules * createRules(FuzzyInput *speed, 
 *                                   FuzzyInput *distance, 
 *                                   FuzzyInput *wet, 
 *                                   FuzzyOutput *acceleration,
 *                                   FuzzyOutput *brake)
 * {
 *   Rules::addFuzzyInput(speed);
 *   Rules::addFuzzyInput(distance);
 *   Rules::addFuzzyInput(wet);
 *   Rules::addFuzzyOutput(acceleration);
 *   Rules::addFuzzyOutput(brake);
 *  
 *   if (speed=="high" && (distance=="middle" || wet != "damp"))
 *   { 
 *     acceleration = "zero";
 *     brake = "middle";
 *   } 
 *     ...
 *
 *   return Rules::getRules();
 * }
 * @endcode
 * @ingroup fuzzy
 */
class FuzzyExpr 
{
    FONode * value;
  public:
    /**
     * Objects of this class are usualy created inside if command.<br>
     * Objekty této tøídy jsou obvykle vytváøeny uvnitø pøíkazu if.
     */
    // implemented in fuzzyrul.cc
    explicit FuzzyExpr(FONode * value);
    /** It returns value of expression.<br>Vrací hodnotu výrazu. */
    FONode * Value() 
    {
      FONode * node = value;
      value = NULL;
      return node; 
    }
    /**
     * It creates object tree.<br>Vytvoøí strom objektù.
     */
    // implemented in fuzzyrul.cc
    operator bool(); // if(bool(expr)) actions; --- store alpha
    
};// FuzzyExpr

/////////////////////////////////////////////////////////////////////////////
// Fuzzy operators --- used in FuzzyBlock::Behavior() 
//
// implemented in fuzzyrul.cc
/**
 * @ingroup fuzzy
 * if ((FuzzyExpr) && (FuzzyExpr)) 
 */
FuzzyExpr operator && (FuzzyExpr o1, FuzzyExpr o2);        // AND
/**
 * @ingroup fuzzy
 * if ((FuzzyExpr) || (FuzzyExpr)) 
 */
FuzzyExpr operator || (FuzzyExpr o1, FuzzyExpr o2);        // OR
/**
 * @ingroup fuzzy
 * if (!(FuzzyExpr)) 
 */
FuzzyExpr operator ! (FuzzyExpr o1);                       // NOT
/**
 * @ingroup fuzzy
 * if ((input == "wordvalue")...) 
 */
FuzzyExpr operator == (FuzzyInput &s, const char *value);  // is


} // namespace

#endif
