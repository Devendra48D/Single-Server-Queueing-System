// cz - p��li� �lu�ou�k� k�� �p�l ��belsk� �dy
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
 * FuzzySIMLIB je ��st� simula�n� knihovny SIMLIB a obsahuje fuzzy a xml modul.
 */
 
/**
 * @defgroup fuzzy Fuzzy modul
 * This module contains number of classes which is needed for fuzzy controllers modeling.
 *
 * Tento modul obsahuje soubor t��d pot�ebn�ch pro modelov�n� fuzzy regul�tor�.
 */

/**
 * @defgroup xml XML modul
 * General interface for loading of external data. Now only interface for reding fuzzy models
 * from XML is implemented here.
 *
 * Obecn� rozhran� pro na��t�n� extern� ulo�en�ch dat. Prozat�m je zde implementov�no
 * konkr�tn� rozhran� pro na��t�n� fuzzy model� z XML soubor�.
 */


/**
 * @example auta.cc
 * This is complete general interface example for typing inference rules in. Time for start 
 * is a little slower but the run is quite fast. This interface is quite uncomfortable.
 * You can use rather interface which is described in FuzzyExpr and FuzzyGeneralRules classes.
 *
 * Toto je kompletn� p��klad pou�it� obecn�ho rozhran� pro zad�v�n� inferen�n�ch
 * pravidel. Start tohoto programu je trochu del��, ale samotn� b�h modelu je
 * pom�rn� rychl�. Toto rozhran� je celkem nepohodln�. Rad�ji pou��vejte rozhran� popsan�
 * ve t��d�ch FuzzyExpr a FuzzyGeneralRules.
 */
/**
 * @example auta2.cc
 * This is comfortable general interface example for typing inference rules in. This is the
 * slowest but the  most general solution.
 *
 * P��klad komfortn�ho obecn�ho rozhran� pro z�pis inferen�n�ch pravidel. Toto je nejpomalej��
 * ale nejobecn�j�� �e�en�.
 */
/**
 * @example autanottree.cc
 * This is complete specialized interface example for typing infrence rules in. This is the most 
 * faster solution from all these examples.
 * 
 * Toto je kompletn� p��klad pou�it� specializovan�ho rozhran� pro zad�v�n� inferen�n�ch
 * pravidel. Pou�it�m tohoto rozhran� je inicializace modelu nejrychlej�� ze v�ech 
 * t�chto p��klad�.
 */
/**
 * @example xml.cc 
 * This is complete example of use XML for model specification. The analyzis takes a quite
 * muxh time, but it has no influence on time for run. This solution is most flexible.
 *
 * Toto je kompletn� p��klad pou�it� XML p�i specifikaci modelu. Anal�za xml zp�sobuje
 * pom�rn� dlouhou inicialializaci, ale na b�h modelu nem� ��dn� vliv. Toto �e�en�
 * je nejv�ce flexibiln�.
 */
/**
 * @example ftycka.cc 
 * Next complete example of use XML for model specification.  The analyzis takes a quite
 * muxh time, but it has no influence on time for run. This solution is most flexible.
 *
 * Dal�� kompletn� p��klad pou�it� XML p�i specifikaci modelu. Anal�za xml zp�sobuje
 * pom�rn� dlouhou inicialializaci, ale na b�h modelu nem� ��dn� vliv. Toto �e�en�
 * je nejv�ce flexibiln�.
 */


/////////////////////////////////////////////////////////////////////////////
// general membership function
/////////////////////////////////////////////////////////////////////////////
/**
 * Abstract baze class for membership functions.<br>
 * Abstraktn� b�zov� t��da pro funkce p��slu�nosti.
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
     * P�i�ad� slovn� hodnotu funkci p��slu�nosti. Nekop�ruje ukazatel parametru name, ale vytv���
     * kopii pam�ti.
     * @param name Word value for this function.<br>Slovn� hodnota pro tuto funkci
     */
    // implemented in fuzzymf.cc
    FuzzyMembershipFunction(const char *name);
    /**
     * Copy constructor. It creates copy of all member variables. For poiners creates copy of 
     * memory block.<br>
     * Kopy konstruktor. Vytv��� kopii v�ech �lensk�ch prom�nn�ch. Pro ukazatele vytvo�� kopii
     * pam�ti.
     * @param duplicate Object that will be duplicated.<br>Objekt, kter� bude duplikov�n.
     */
    // implemented in fuzzymf.cc
    FuzzyMembershipFunction(const FuzzyMembershipFunction &duplicate);
    /** Destructor */
    // implemented in fuzzymf.cc
    virtual ~FuzzyMembershipFunction();
    /** Gets word value of this function.<br>Vrac� slovn� hodnotu t�to funkce. */
    const char *wordValue() const { return Name; }
    /** It duplicates object.<br>Duplikuje objekt. */
    virtual FuzzyMembershipFunction *clone() const = 0; 
    /** It computes function value (membership).<br>Vypo�te funk�n� hodnotu (p��slu�nost). */
    virtual double Membership(double x) const = 0; 
    /** Center of this function.<br>St�ed t�to funkce. */
    virtual double center() const = 0;
    /** First occurence of maximum in this function.<br>Prvn� v�skyt maxima v t�to funkci. */
    virtual double min1() const = 0;
    /** Last occurence of maximum in this function.<br>Posledn� v�skyt maxima v t�to funkci. */
    virtual double max1() const = 0;
    /** Print of membership table.<br>Tisk tabulky p��slu�nosti. */
    // implemented in fuzzymf.cc
    void Print(double a, double b) const;
    /** Test if is definition of function complete.<br>Test jestli je definice funkce kompletn�. */
    bool isComplete() { return defValues == getNumValues(); }
    /** This adds next definition value.<br>P�id� dal�� defini�n� hodnotu. */
    virtual void addDefValue(double value) = 0;
    /** It retuns number of definition values.<br>Vr�t� po�et defini�n�ch hodnot. */
    virtual int getNumValues() = 0;
};//FuzzyMembershipFunction 

/////////////////////////////////////////////////////////////////////////////
// special membership functions:
/////////////////////////////////////////////////////////////////////////////
/**
 * Membership function of type impulse.<br>
 * Funkce p��slu�nosti typu impuls.
 * @ingroup fuzzy
 */
class FuzzySingleton: public FuzzyMembershipFunction
{
    double x0;
  public:
    /**
     * It assigns word value to the membership function and sets definition value.<br>
     * P�i�ad� slovn� hodnotu funkci p��slu�nosti a nastav� defini�n� hodnotu.
     * @param name Word value for this function.<br>Slovn� hodnota pro tuto funkci
     * @param a A definition value.<br>Defini�n� hodnota.
     */
    // implemented in fuzzymf.cc
    FuzzySingleton(const char *name, double a);
    /**
     * It assigns word value to the membership function.<br>
     * P�i�ad� slovn� hodnotu funkci p��slu�nosti.
     * @param name Word value for this function.<br>Slovn� hodnota pro tuto funkci
     */
    // implemented in fuzzymf.cc
    FuzzySingleton(const char *name);
    // implemented in fuzzymf.cc
    
    virtual ~FuzzySingleton() { TRACE(printf("~FuzzySingleton()\n")); }
    /** It duplicates object.<br>Duplikuje objekt. */
    // implemented in fuzzymf.cc
    virtual FuzzySingleton *clone() const;
    /** It computes function value (membership).<br>Vypo�te funk�n� hodnotu (p��slu�nost). */
    // implemented in fuzzymf.cc
    virtual double Membership(double x) const;
    /** Center of this function.<br>St�ed t�to funkce. */
    virtual double center() const { return x0; } 
    /** First occurence of maximum in this function.<br>Prvn� v�skyt maxima v t�to funkci. */
    virtual double min1() const { return x0; }
    /** Last occurence of maximum in this function.<br>Posledn� v�skyt maxima v t�to funkci. */
    virtual double max1() const { return x0; }
    /** This adds next definition value.<br>P�id� dal�� defini�n� hodnotu. */
    // implemented in fuzzymf.cc
    virtual void addDefValue(double value);
    /** It retuns number of definition values.<br>Vr�t� po�et defini�n�ch hodnot. */
    virtual int getNumValues() { return 1; }
};//FuzzySingleton

/**
 * Triangulal membership function.<br>Troj�heln�kov� funkce p��slu�nosti.
 * @ingroup fuzzy
 */
class FuzzyTriangle: public FuzzyMembershipFunction 
{
    double x0, x1, x2;
  public:
    /**
     * It assigns word value to the membership function and sets definition values.<br>
     * P�i�ad� slovn� hodnotu funkci p��slu�nosti a nastav� defini�n� hodnotu.
     * @param name Word value for this function.<br>Slovn� hodnota pro tuto funkci
     * @param a A left vertex.<br>Lev� vrchol.
     * @param b A center vertex.<br>Prost�edn� vrchol.
     * @param c A right vertex.<br>Prav� vrchol.
     */
    // implemented in fuzzymf.cc
    FuzzyTriangle(const char *name, double a, double b, double c);
    /**
     * It assigns word value to the membership function.<br>
     * P�i�ad� slovn� hodnotu funkci p��slu�nosti.
     * @param name Word value for this function.<br>Slovn� hodnota pro tuto funkci
     */
    // implemented in fuzzymf.cc
    FuzzyTriangle(const char* name);
    /** Destructor. */
    virtual ~FuzzyTriangle() { TRACE(printf("~FuzzyTriangle\n")); } 
    /** It duplicates object.<br>Duplikuje objekt. */
    // implemented in fuzzymf.cc
    virtual FuzzyTriangle *clone() const;
    /** It computes function value (membership).<br>Vypo�te funk�n� hodnotu (p��slu�nost). */
    // implemented in fuzzymf.cc
    virtual double Membership(double x) const;
    /** Center of this function.<br>St�ed t�to funkce. */
    virtual double center() const { return (x0+x1+x2)/3; } 
    /** First occurence of maximum in this function.<br>Prvn� v�skyt maxima v t�to funkci. */
    virtual double min1() const { return x1; }
    /** Last occurence of maximum in this function.<br>Posledn� v�skyt maxima v t�to funkci. */
    virtual double max1() const { return x1; }
    /** It retuns number of definition values.<br>Vr�t� po�et defini�n�ch hodnot. */
    virtual int getNumValues() { return 3; } 
    /** This adds next definition value.<br>P�id� dal�� defini�n� hodnotu. */
    // implemented in fuzzymf.cc
    virtual void addDefValue(double value);
};//FuzzyTriangle

/////////////////////////////////////////////////////////////////////////////
// FuzzyTrapez ---
/**
 * Trapezoidal membership function.<br>
 * Lichob�n�kov� funkce p��slu�nosti.
 * @ingroup fuzzy
 */
class FuzzyTrapez: public FuzzyMembershipFunction 
{
    double x0, x1, x2, x3;
  public:
    /**
     * It assigns word value to the membership function and sets definition values.<br>
     * P�i�ad� slovn� hodnotu funkci p��slu�nosti a nastav� defini�n� hodnotu.
     * @param name Word value for this function.<br>Slovn� hodnota pro tuto funkci
     * @param a A left down vertex.<br>Lev� spodn� vrchol.
     * @param b A left top vertex.<br>Lev� horn� vrchol.
     * @param c A right top vertex.<br>Prav� horn� vrchol.
     * @param d A right down vertex.<br>Prav� spodn� vrchol.
     */
    // implemented in fuzzymf.cc
    FuzzyTrapez(const char *name, double a, double b, double c, double d);
    /**
     * It assigns word value to the membership function.<br>
     * P�i�ad� slovn� hodnotu funkci p��slu�nosti.
     * @param name Word value for this function.<br>Slovn� hodnota pro tuto funkci
     */
    // implemented in fuzzymf.cc
    FuzzyTrapez(const char* name);
    /** Destructor. */
    virtual ~FuzzyTrapez() { TRACE(printf("~FuzzyTrapez()\n")); }
    /** It duplicates object.<br>Duplikuje objekt. */
    // implemented in fuzzymf.cc
    virtual FuzzyTrapez *clone() const;
    /** It computes function value (membership).<br>Vypo�te funk�n� hodnotu (p��slu�nost). */
    // implemented in fuzzymf.cc
    virtual double Membership(double x) const;
//    virtual double center() const { return (x1+x2)/2; } // bad### 
    /** Center of this function.<br>St�ed t�to funkce. */
    virtual double center() const { return (x0+2*x1+2*x2+x3)/6; } 
    /** First occurence of maximum in this function.<br>Prvn� v�skyt maxima v t�to funkci. */
    virtual double min1() const { return x1; }
    /** Last occurence of maximum in this function.<br>Posledn� v�skyt maxima v t�to funkci. */
    virtual double max1() const { return x2; }
    /** It retuns number of definition values.<br>Vr�t� po�et defini�n�ch hodnot. */
    virtual int getNumValues() { return 4; } 
    /** This adds next definition value.<br>P�id� dal�� defini�n� hodnotu. */
    // implemented in fuzzymf.cc
    virtual void addDefValue(double value);
};// FuzzyTrapez

/////////////////////////////////////////////////////////////////////////////
// FuzzyGauss ---
/**
 * Gaussian membership function.<br>
 * Gaussovsk� funkce p��slu�nosti.
 * @ingroup fuzzy
 */
class FuzzyGauss: public FuzzyMembershipFunction 
{
    double sigma, c, twoSqrSigma;
  public:
    /**
     * It assigns word value to the membership function and sets definition values.<br>
     * P�i�ad� slovn� hodnotu funkci p��slu�nosti a nastav� defini�n� hodnoty.
     * @param name Word value for this function.<br>Slovn� hodnota pro tuto funkci
     * @param center A center of function.(mi) <br>St�ed funkce. (mi)
     * @param radius A radius of function. (3*sigma)<br>Polom�r funkce. (3*sigma)
     */
    // implemented in fuzzymf.cc
    FuzzyGauss(const char *name, double center, double radius);
    /**
     * It assigns word value to the membership function.<br>
     * P�i�ad� slovn� hodnotu funkci p��slu�nosti.
     * @param name Word value for this function.<br>Slovn� hodnota pro tuto funkci
     */
    // implemented in fuzzymf.cc
    FuzzyGauss(const char* name);
    /** Destructor. */
    virtual ~FuzzyGauss() { TRACE(printf("~FuzzyGauss()\n")); }
    /** It duplicates object.<br>Duplikuje objekt. */
    // implemented in fuzzymf.cc
    virtual FuzzyGauss *clone() const;
    /** It computes function value (membership).<br>Vypo�te funk�n� hodnotu (p��slu�nost). */
    // implemented in fuzzymf.cc
    virtual double Membership(double x) const;
    /** Center of this function.<br>St�ed t�to funkce. */
    virtual double center() const { return c; }
    /** First occurence of maximum in this function.<br>Prvn� v�skyt maxima v t�to funkci. */
    virtual double min1() const { return c; }
    /** Last occurence of maximum in this function.<br>Posledn� v�skyt maxima v t�to funkci. */
    virtual double max1() const { return c; }
    /** It retuns number of definition values.<br>Vr�t� po�et defini�n�ch hodnot. */
    int getNumValues() { return 2; } 
    /** This adds next definition value.<br>P�id� dal�� defini�n� hodnotu. */
    // implemented in fuzzymf.cc
    virtual void addDefValue(double value);
};// FuzzyGauss

/**
 * Nonsymetric Gaussian membership function.<br>
 * Nesymetrick� Gaussovsk� funkce p��slu�nosti.
 * @ingroup fuzzy
 */
class FuzzyGauss2 : public FuzzyMembershipFunction 
{
  public:
    /**
     * It assigns word value to the membership function and sets definition values.<br>
     * P�i�ad� slovn� hodnotu funkci p��slu�nosti a nastav� defini�n� hodnoty.
     * @param name Word value for this function.<br>Slovn� hodnota pro tuto funkci
     * @param leftCenter A center of the left function.(mi) <br>St�ed lev� funkce. (mi)
     * @param leftRadius A radius of the left function. (3*sigma)<br>Polom�r lev� funkce. (3*sigma)
     * @param rightCenter A center of the right function.(mi) <br>St�ed prav� funkce. (mi)
     * @param rightRadius A radius of the right function. (3*sigma)<br>Polom�r prav� funkce. (3*sigma)
     */
    // implemented in fuzzymf.cc
    FuzzyGauss2(const char* name, 
                double leftCenter, double leftRadius, 
                double rightCenter, double rightRadius);
    /**
     * It assigns word value to the membership function.<br>
     * P�i�ad� slovn� hodnotu funkci p��slu�nosti.
     * @param name Word value for this function.<br>Slovn� hodnota pro tuto funkci
     */
    // implemented in fuzzymf.cc
    FuzzyGauss2(const char* name);
    /** Destructor. */
    virtual ~FuzzyGauss2() { TRACE(printf("~FuzzyGauss2()\n")); }
    /** It duplicates object.<br>Duplikuje objekt. */
    // implemented in fuzzymf.cc
    virtual FuzzyGauss2 *clone() const;
    /** It computes function value (membership).<br>Vypo�te funk�n� hodnotu (p��slu�nost). */
    // implemented in fuzzymf.cc
    virtual double Membership(double x) const;
    /** Center of this function.<br>St�ed t�to funkce. */
    // implemented in fuzzymf.cc
    virtual double center() const; 
    /** First occurence of maximum in this function.<br>Prvn� v�skyt maxima v t�to funkci. */
    // implemented in fuzzymf.cc
    virtual double min1() const;
    /** Last occurence of maximum in this function.<br>Posledn� v�skyt maxima v t�to funkci. */
    // implemented in fuzzymf.cc
    virtual double max1() const;
    /** It retuns number of definition values.<br>Vr�t� po�et defini�n�ch hodnot. */
    int getNumValues() { return 4; } 
    /** This adds next definition value.<br>P�id� dal�� defini�n� hodnotu. */
    // implemented in fuzzymf.cc
    virtual void addDefValue(double value);
  protected:
    double leftSigma;     /**< A radius of the left function. */
    double leftCenter;    /**< A center of the left function. */
    double rightSigma;    /**< A radius of the right function. */
    double rightCenter;   /**< A center of the right function. */
    /** Position of vertex when leftCenter > rightCenter.<br>Pozice vrcholu, kdy� leftCenter > rightCenter. */
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
 * Implementace fuzzy mno�iny.
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
     * @param min Minimal value of universum.<br>Spodn� mez univerza.
     * @param max Maximal value of universum.<br>Horn� mez univerza.
     */
    // implemented in fuzzy.cc
    FuzzySet(const char * name, double min, double max, 
     const FuzzyMembershipFunction &m1);
    /**
     * Creates fuzzy set.
     * @param min Minimal value of universum.<br>Spodn� mez univerza.
     * @param max Maximal value of universum.<br>Horn� mez univerza.
     */
    // implemented in fuzzy.cc
    FuzzySet(const char * name, double min, double max, 
     const FuzzyMembershipFunction &m1,
		 const FuzzyMembershipFunction &m2);
    /**
     * Creates fuzzy set.
     * @param min Minimal value of universum.<br>Spodn� mez univerza.
     * @param max Maximal value of universum.<br>Horn� mez univerza.
     */
    // implemented in fuzzy.cc 
    FuzzySet(const char * name, double min, double max, 
     const FuzzyMembershipFunction &m1,
		 const FuzzyMembershipFunction &m2,
		 const FuzzyMembershipFunction &m3);
    /**
     * Creates fuzzy set.
     * @param min Minimal value of universum.<br>Spodn� mez univerza.
     * @param max Maximal value of universum.<br>Horn� mez univerza.
     */
    // implemented in fuzzy.cc 
    FuzzySet(const char * name, double min, double max, 
     const FuzzyMembershipFunction &m1,
		 const FuzzyMembershipFunction &m2,
		 const FuzzyMembershipFunction &m3,
		 const FuzzyMembershipFunction &m4);
    /**
     * Creates fuzzy set.
     * @param min Minimal value of universum.<br>Spodn� mez univerza.
     * @param max Maximal value of universum.<br>Horn� mez univerza.
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
     * @param min Minimal value of universum.<br>Spodn� mez univerza.
     * @param max Maximal value of universum.<br>Horn� mez univerza.
     */
    // implemented in fuzzy.cc 
    FuzzySet(const char * name, double min, double max); 
    /** Destructor removes all membership functions.<br>Destruktor uvoln� v�echny funkce p��slu�nosti. */
    // implemented in fuzzy.cc
    virtual ~FuzzySet();
    /** It duplicates object.<br>Duplikuje objekt. */
    // implemented in fuzzy.cc
    virtual FuzzySet *clone() const;
    /**
     * It adds next word value into the universum.<br>P�id� dal�� slovn� hodnotu do univerza.
     * @param x Membership function representing word value.<br>Funkce p��slu�nosti reprezentuj�c� slovn� hodnotu.
     */
    // implemented in fuzzy.cc
    void add(const FuzzyMembershipFunction &x); 
    /** Number of membership functions.<br>Po�et funkc� p��slu�nosti. */
    int count() const { return n; } 
    /** It selects i-th member function.<br>Vybere i-tou funkci p��slu�nosti. */
    // implemented in fuzzy.cc
    virtual const FuzzyMembershipFunction *operator[] (int i) const;
    /** It computes i-th function value (membership).<br>Vypo�te i-tou funk�n� hodnotu (p��slu�nost). */
    // implemented in fuzzy.cc
    double Membership(int i, double x) const;
    /** Minimal value of universum.<br>Spodn� mez univerza. */
    double min() { return xmin; }
    /** Maximal value of universum.<br>Horn� mez univerza. */
    double max() { return xmax; }
    /** Name of this fuzzy set.<br>Jm�no t�to fuzzy mno�iny. */
    const char * name() const { return Name; }
    /** Word value of i-th membership function.<br>Slovn� hodnota i-t� funkce p��slu�nosti. */
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
 * Obecn� fuzzy prom�nn�. B�zov� t��da pro FuzzyInput a FuzzyOutput.
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
     * Spoj� fuzzy mno�inu s fuzzy prom�nnou. Pokud je prom�nn� vytv��ena
     * uvnit� t��dy FuzzyBlock, nen� t�eba volat metodu registerOwner. Objekt FuzzySet je p�i p�ed�n�
     * klonov�n a ulo�en uvnit� objektu. Proto mus�te uvolnit pam� alokovanou pro t vy sami.
     * @param t Fuzzy set.<br>Fuzzy mno�ina.
     */
    // implemented in fuzzy.cc
    FuzzyVariable(const FuzzySet &t);
    /** It releases memory allocated by FuzzySet.<br>Uvoln� pam� alokovanou pro FuzzySet. */
    virtual ~FuzzyVariable() 
    {
       TRACE(printf("~FuzzyVariable()\n")); 
       delete m;
       delete [] mval;
    }
      
    /**
     * It registers owner of this variable.<br>Zaregistruje vlastn�ka t�to prom�nn�.
     * @param owner Owner of this variable.<br>Vlastn�k t�to prom�nn�.
     */
    // implemented in fuzzy.cc
    void registerOwner(FuzzyBlock *owner);  // registration inside owner
    /** Number of members.<br>Po�et �len�. */
    unsigned count() const { return n; } 
    /** I-th member function.<br>I-t� funkce p��slu�nosti. */
    // implemented in fuzzy.cc
    const FuzzyMembershipFunction *mf(int i) const;
    /** It gets center of i-th member function.<br>Vrac� st�ed i-t� funkce p��slu�nosti. */
    // implemented in fuzzy.cc
    double center(int i) const; 
    /** It gets i-th word value.<br>Vrac� i-tou slovn� hondotu. */
    // implemented in fuzzy.cc
    const char *wordValue(int i) const; // name of i-th membership function
    /** Get/set fuzzy value.<br>Vra�/nastav fuzzy hodnotu. */
    // implemented in fuzzy.cc
    double &operator[] (int i) const;
    /** Get/set fuzzy value.<br>Vra�/nastav fuzzy hodnotu. */
    double &operator[] (const char *s) const { return mval[search(s)]; }
    /** Search by member name.<br>Hled� podle jm�na. */
    // implemented in fuzzy.cc
    unsigned search(const char *s) const; 
    
    /** Fuzzify all membership functions.<br>Fuzzifikuje v�echny funkce p��slu�nosti.*/
    // implemented in fuzzyio.cc
    void Fuzzify(double x);
    /**
     * Initializes all values for membership functions or fuzzify all value<br>
     * Inicializuje v�echny hodnoty funkc� p��slu�nosti nebo provede fuzzifikaci.
     */
    virtual void Init() { for(unsigned i=0; i!=n; i++) mval[i]=0.0F; }
    virtual void Done() {}
    /** It gets owner.<br>Vrac� vlastn�ka. */
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
 * Vstupn� prom�nn�. Fuzzifikuje vstup. To znamen�, �e vezme ostrou vstupn� hodnotu 
 * (Input) a transformuje ji do fuzzy reprezentace.
 * @ingroup fuzzy
 */
class FuzzyInput : public FuzzyVariable {
    Input in; /**< continuous input */ 
  public:
    /**
     * It assignes continuous input and fuzzy set into this object.<br>
     * P�i�ad� spojit� vstup a fuzzy mno�inu do tohoto objektu.
     * @param i Continuous input.<br>Spojit� vstup.
     * @param t Fuzzy set.<br>Fuzzy mno�ina.
     */
    FuzzyInput(Input i, const FuzzySet &t) : FuzzyVariable(t), in(i) {}
    /**
     * It assignes only fuzzy set into this object. Continuous input must be 
     * added by setInput method. <br>
     * P�i�ad� tomuto objektu pouze fuzzy mno�inu. Spojit� vstup mus� b�t
     * p�id�n pomoc� metody setInput.
     * @param t Fuzzy set.<br>Fuzzy mno�ina.
     */
    FuzzyInput(const FuzzySet &t) : FuzzyVariable(t), in(0.0) {}
    /** Destructor. */
    virtual ~FuzzyInput() { TRACE(printf("~FuzzyInput()\n")); }
    /**
     * It adds continuous input into this object.<br>
     * P�id� spojit� vstup do tohoto objektu.
     * @param i Continuous input.<br>Spojit� vstup.
     */
    void setInput(Input i) { in = i; }
    /**
     * It returns sharp input value.<br>Vrac� ostrou vstupn� hodnotu.
     * @return It returns sharp input value.<br>Vrac� ostrou vstupn� hodnotu.
     */
    virtual double Value() { return in.Value(); }
    /**
     * It fuzzifies continuous input. This method is called every time before call 
     * to FuzzyBlock::Behavior().<br>
     * Fuzzifikuje spojit� vstup. Tato metoda je vol�na v�dy p�ed vol�n�m 
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
 * V�stupn� prom�nn�. Defuzzifikuje fuzzy hodnoty, kter� jsou z�sk�ny zpracov�n�m
 * inferen�n�ch pravidel.
 * @ingroup fuzzy
 */
class FuzzyOutput: public FuzzyVariable {
    double value; /**? value after defuzzification */ 
    double (*defuzzify)(const FuzzyVariable&); /**< defuzzification function */  // remove!!!!!!!####
  public:
    /**
     * It adds fuzzy set and defuzzification function.<br>
     * P�id� fuzzy mno�inu a defuzzifika�n� funkci.
     * @param t   Fuzzy set.<br>Fuzzy mno�ina.
     * @param def Defuzzification function.<br>Defuzzifika�n� funkce.
     */
    // implemented in fuzzyrul.cc
    FuzzyOutput(const FuzzySet &t, double (*def)(const FuzzyVariable&)=0);
    /**
     * It assignes fuzzy value by word value.<br>P�i�ad� fuzzy hodnotu pomoc� slovn� hodnoty.
     */
    // implemented in fuzzyrul.cc
    const char *operator = (const char *val);
    /** 
     * It defuzzifies itself.<br> Defuzzifikuje se. 
     * @return It returns sharp value.<br>Vr�t� osrou hodnotu.
     */
    // implemented in fuzzyrul.cc
    double Defuzzify();
    /** It adds defuzzification function.<br>P�id� defuzzifika�n� funkci. */
    // implemented in fuzzyrul.cc
    void SetDefuzzifyMethod(double (*f)(const FuzzyVariable&));
    /**
     * Evaluates FuzzyBlock (variable where) and after defuzzification returns sharp value.<br>
     * Vyhodnot� FuzzyBlock (prom�nnou whera) a po defuzzifikaci vr�t� ostrou hodnotu.
     * @return Sharp value after defuzzification.<br>Ostr� hodnota po defuzzifikaci.
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
 * Defuzzification method "mean-of-maximum".<br>Defuzifika�n� metoda "st�ed maxim". 
 */
double defuzMeanOfMax(const FuzzyVariable &fs);
/**
 * @ingroup fuzzy
 * Defuzzification method "min-of-maximum".<br>Defuzifika�n� metoda "minim�ln� maximum". 
 */
// implemented in fuzzyio.cc
double defuzMinOfMax(const FuzzyVariable &fs);  
/** 
 * @ingroup fuzzy
 * Defuzzification method "max-of-maximum".<br>Defuzifika�n� metoda "maxim�ln� maximum". 
 */
// implemented in fuzzyio.cc
double defuzMaxOfMax(const FuzzyVariable &fs); 
/**
 * @ingroup fuzzy
 * Defuzzification method "discrete-center-of-gravity".<br>Defuzifika�n� metoda "diskr�tn� t�i�t�". 
 */
// implemented in fuzzyio.cc
double defuzDCOG(const FuzzyVariable &fs);

/////////////////////////////////////////////////////////////////////////////
// FuzzyBlock --- base class for inference blocks
// 
/**
 * Base class for inference blocks. Representation of fuzzy regulator. <br>
 * B�zov� t��da pro inferen�n� bloky. Reprezentace fuzzy regul�toru.
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
     * Jestli�e jsou inferen�n� pravidla specifikov�na pomoc� FuzzyExpr, pak mus�te
     * zavolat metodu EndConstructor na konci konstruktoru.
     */
    // implemented in fuzzyrul.cc
    FuzzyBlock();
    /**
     * It evaluates whole block. It calls method Behavior.<br>
     * Vyhodnot� cel� blok. Vol� metodu Behavior.
     */
    // implemented in fuzzyrul.cc
    virtual void Evaluate();              // calls Behavior()
    /**
     * If inference rules are specified by FuzzyExpr way then you must call this
     * method on the end of constructor.<br>
     * Jestli�e jsou inferen�n� pravidla specifikov�na pomoc� FuzzyExpr, pak mus�te
     * zavolat tuto metodu na konci konstruktoru.
     */
    // implemented in fuzzyrul.cc
    void EndConstructor();        // ### marks end, should be called in constructor
    /**
     * It registers fuzzy variable inside this object. If inference rules are NOT specified 
     * by FuzzyExpr way then you must call this method inside the constructor for all FuzzyInput
     * and FuzzyOutput variables.<br>
     * Registruje fuzzy prom�nnou v tomto objektu. Jestli�e NEjsou inferen�n� pravidla 
     * specifikov�na pomoc� FuzzyExpr, pak mus�te zavolat tuto metodu v konstruktoru pro v�echny
     * prom�nn� typu FuzzyInput a FuzzyOutput.
     */
    // implemented in fuzzyrul.cc
    void Register(FuzzyVariable *obj); // register variable in vlist
    virtual ~FuzzyBlock() { TRACE(printf("~FuzzyBlock()\n")); }
}; // FuzzyBlock


/////////////////////////////////////////////////////////////////////////////
// Author: David Martniek
/**
 * Base class for inference blocks with sampled input.<br>
 * B�zov� t��da pro inferen�n� bloky se vzorkovan�m vstupem
 * @ingroup fuzzy
 */
class FuzzySampledBlock
: public FuzzyBlock
{
    /**
     * Inner class for sampling  
     * Vnit�n� t��da pro vzorkov�n�
     * @ingroup fuzzy
     */
    class FSampler 
    : public Event
    {
      
      double timeStep;
      FuzzySampledBlock *parent;
      public:
        /**
         * Creates sampler<br>Vytvo�� vzorkova�.
         * @param parent owner of this class<br>vlastn�k t�to t��dy
         */
        FSampler(FuzzySampledBlock *parent) 
        { 
          this->parent = parent;
          timeStep = 0;
        }
        ~FSampler() { TRACE(printf("~FSampler()\n"));  parent = 0; }
        /** It sets the time step for sampling.<br> Nastav� �asov� krok pro vzorkov�n�. */
        void setTimeStep(double timeStep) { this->timeStep = timeStep; }
        /** It gets time step.<br>Vr�t� �asov� krok. */
        double getTimeStep() { return timeStep; }
        /**
         * Sampling - it evaluates the parent object and shedules next activacion.<br>
         * Vzorkov�n� - vyhodnot� rodi�ovsk� objekt a napl�nuje dal�� spu�t�n�.
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
     * Bude obsahovat u�ivatelem definovan� inferen�n� pravidla.
     */
    virtual void Behavior() = 0;  
  public:
    /**
     * It creates and initializes the sampler object.
     * If inference rules are specified by FuzzyExpr way then you must call 
     * EndConstructor method on the end of constructor.<br>
     * Vytvo�� a inicalizuje vzorkova�.
     * Jestli�e jsou inferen�n� pravidla specifikov�na pomoc� FuzzyExpr, pak mus�te
     * zavolat metodu EndConstructor na konci konstruktoru.
     */
    FuzzySampledBlock() { sampler = new FSampler(this); }
    virtual ~FuzzySampledBlock() { TRACE(printf("~FuzzySampledBlock")); }
    /**
     * It sets the time step for sampling.<br>
     * Nastav� �asov� krok pro vzorkov�n�.
     */
    void setTimeStep(double timeStep) { sampler->setTimeStep(timeStep); }
    /** It gets time step.<br> Vr�t� �asov� krok. */
    double getTimeStep() { return sampler->getTimeStep(); }
    /** It starts sampling.<br> Nastartuje vzorkov�n�. */
    void Start() { sampler->Activate(); }
    /**
     * It evaluates fuzzy inference rules but only in sampled time steps.<br>
     * Vyhodnot� fuzzy inferen�n� pravidla, ale jenom ve vzorkovan�ch �asov�ch okam�ic�ch.
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
 * Abstraktn� t��da pro reprezentaci a vyhodnocen� inferen�n�ch pravidel. 
 * Definuje rozhran� pro obecn� vkl�d�n� vstup�, v�stup� a inferen�n�ch pravidel.
 * Neimplementuje konkr�tn� form�t ulo�en� pravidel.
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
     * Operace, kter� je mo�no pou��t uvnit� inferen�n�ch pravidel. 
     */
    enum Operations { opAND, opOR, opNOT, opNAND, opNOR };
    
    /**
     * Destructor - does NOT free memory allocated by FuzzyInput and FuzzyOutput!<br>
     *
     * Destruktor - NEuvol�uje pam� zabranou prom�nn�mi FuzzyInput a FuzzyOutput! 
     * Uvoln�n� t�to pam�ti se mus� prov�st JINDE!
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
     * Vlo�� dal�� vstupn� prom�nnou. P�ed vol�n�m je vhodn� pou��t funkci isComplete.
     * @param in Ipnut fuzzy variable. It has not to be registered inside FuzzyBlock yet.
     *           Vstupn� fuzzy prom�nn�. Zde je�t� nemus� b�t registrovan� uvnit� FuzzyBlock.
     * 
     * @see isComplete()
     */
    virtual void addFuzzyInput(FuzzyInput *in)
    { this->in.push_back(in); }

    /**
     * It adds next output variable. Before calling this method is recommended to call
     * isComplete. <br>
     *
     * Vlo�� v�stupn� prom�nnou. P�ed vol�n�m je vhodn� pou��t funkci isComplete. 
     * @param out Output fuzzy variable. It has not to be registered inside FuzzyBlock yet.
     *            V�stupn� fuzzy prom�nn�. Zde je�t� nemus� b�t registrovan� uvnit� FuzzyBlock.
     * @see isComplete()
     */
    virtual void addFuzzyOutput(FuzzyOutput *out)
    { this->out.push_back(out); }
    
    /**
     * It safely creates FuzzyRuleFactory.<br>
     * Bezpe�n�m zp�sobem vytvo�� FuzzyRuleFactory.
     */
    //implemented in rules.cc
    virtual FuzzyRuleFactory * createRuleFactory();

    /**
     * It returns true when all variables are assigned.<br>
     * Testuje, jestli u� jsou p�i�azeny v�echny prom�nn�.
     * @see addFuzzyInput(FuzzyInput *in)
     * @see addFuzzyOutput(FuzzyOutput *out)
     */
    virtual bool isComplete() = 0;
    
    /**
     * It adds next rule into list. When it is too much rules here, error is indicated.<br>
     * 
     * P�id� dal�� pravidlo do seznamu. Pokud u� je definov�no p��li� pravidel, nastane chyba.
     * @param rule Inference rule who is represented by tree structure.
     *             Inferen�n� pravidlo reprezentovan� stromovou strukturou.
     * @param release ��k�, �e se po vlo�en� uvoln� pam�. Implicitn� nastaveno true.
     *                Memory will be released after rule storing. Default it is true.
     */
    virtual void add(FuzzyRule *rule, bool release=true) = 0;
    
    /**
     * It evaluates all rules. This method is defaultly called from a method Behavior in class
     * FuzzyRSBlock.<br>
     * Vyhodnot� pravidla. Tato funkce je standardn� vol�na z funkce Behavior t��dy 
     * FuzzyRSBlock.
     */
     virtual void evaluate() = 0;
  protected:
    /** Vector of input variables. <br> Vektor vstupn�ch prom�nn�ch. */
    std::vector<FuzzyInput *>in;
    
    /** Vector of output variables. <br> Vektor v�stupn�ch prom�nn�ch. */
    std::vector<FuzzyOutput *>out;
  private:
}; // FuzzyInferenceRules

/**
 * This class is representing inference rules for classic fuzzy regulator with two inputs and
 * one output. It is possible to adds rules by new defined methods add(). These methods are
 * suitable for use during XML analysis.<br>
 * 
 * T��da reprezentuj�c� inferen�n� pravidla pro klasick� fuzzy regul�tor se dv�ma vstupy a 
 * jedn�m v�stupem. Pravidla je mo�n� vkl�dat pomoc� nov�ch funkc� add, kter� jsou vhodn�
 * pro pou�it� b�hem anal�zy dat z XML souboru. Krom� toho je mo�no pou��t zd�d�n� rozhran�
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
// rozhran� FuzzyInferenceRules -- interface FuzzyInferenceRules
/////////////////////////////////////////////////////////////////
    /**
     * Parameterless constructor. Fuzzy variables must be added explicitly.<br>
     * Konstruktor bez parametru. Fuzzy prom�nn� je nutno p�i�adit pomoc� dal��ch funkc� explicitn�.
     */
    FuzzyIIORules();
    
    /**
     * It initializes all input and output variables.<br>
     * Inicializuje v�echny vstupn� a v�stupn� prom�nn�.
     */
    //implemented in rules.cc
    FuzzyIIORules(FuzzyInput *in1, FuzzyInput *in2, FuzzyOutput *out);
    
    /**
     * Destructor - does NOT free memory allocated by FuzzyInput and FuzzyOutput!<br>
     * Destruktor - NEuvol�uje pam� zabranou prom�nn�mi FuzzyInput a FuzzyOutput! 
     * Uvoln�n� t�to pam�ti se mus� prov�st JINDE!
     */
    //implemented in rules.cc
    virtual ~FuzzyIIORules();
    
    /** It initializes all member variables. <br> Inicializuje vnit�n� prom�nn� */
    //implemented in rules.cc
    void init();
     
    /** It adds next input variable.<br> Vlo�� dal�� vstupn� prom�nnou. */
    //implemented in rules.cc
    virtual void addFuzzyInput(FuzzyInput *in);

    /** It adds output variable.<br> Vlo�� v�stupn� prom�nnou. */
    //implemented in rules.cc
    virtual void addFuzzyOutput(FuzzyOutput *out);

    /**
     * It tests if all variables are assigned.<br>
     * Testuje, jestli u� jsou p�i�azeny v�echny prom�nn�.
     * @see addFuzzyInput(FuzzyInput *in)
     * @see addFuzzyOutput(FuzzyOutput *out)
     */
     //implemented in rules.cc
    virtual bool isComplete();

    /**
     * It evaluates the rules.<br>
     * Vyhodnot� pravidla.
     */
     //implemented in rules.cc
    virtual void evaluate();

    /**
     * It adds next rule into list. If there is too much rules, an error is indicated.<br>
     * P�id� dal�� pravidlo do seznamu. Pokud u� je definov�no p��li� pravidel, nastane chyba.
     * @param rule Inference rule with tree structure.<br>Inferen�n� pravidlo reprezentovan� stromovou strukturou.
     * @param release Memory will be released after rule storing. Default it is true.
     *                ��k�, �e se po vlo�en� uvoln� pam�. Implicitn� nastaveno true.
     */
     //implemented in rules.cc
    virtual void add(FuzzyRule *rule, bool release=true);
    
//////////////////////////////////////
// nov� funkce -- new functions
//////////////////////////////////////
    /**
     * Adds a rule into the list.<br>
     * P��d� inferen�n� pravidlo do seznamu.
     * @param operation     operation which is used in rule<br>operace, kter� se pou�ije v pravidle
     * @param in1WordValue  word value of first fuzzy input variable<br>slovn� hodnota prvn� vstupn� fuzzy prom�nn�
     * @param in2WordValue  word value of second fuzzy input variable<br>slovn� hodnota druh� vstupn� fuzzy prom�nn�
     * @param outWordValue  word value of output fuzzy variable<br>slovn� hodnota v�stupn� fuzzy prom�nn�
     */
     //implemented in rules.cc
    void add (const Operations operation, 
              const char *in1WordValue, 
              const char *in2WordValue, 
              const char *outWordValue);
    
    /**
     * Adds a rule into the list.<br>
     * P��d� inferen�n� pravidlo do seznamu.
     * @param operation   operation which is used in rule<br>operace, kter� se pou�ije v pravidle
     * @param in1WVIndex  index of first fuzzy input variable<br>index slovn� hodnoty prvn� vstupn� prom�nn�
     * @param in2WVIndex  index of second fuzzy input variable<br>index slovn� hodnoty druh� vstupn� prom�nn�
     * @param outWVIndex  index of output fuzzy variable<br>index slovn� hodnoty v�stupn� prom�nn�
     */
     //implemented in rules.cc
    void add (const Operations operation, 
              int in1WVIndex, 
              int in2WVIndex, 
              int outWVIndex);
    /**
     * Adds a rule into the list.<br>
     * P��d� inferen�n� pravidlo do seznamu.
     * @param operation     operation which is used in rule<br>operace, kter� se pou�ije v pravidle
     * @param in1WVIndex    index of first fuzzy input variable<br>index slovn� hodnoty prvn� vstupn� prom�nn�
     * @param in2WVIndex    index of second fuzzy input variable<br>index slovn� hodnoty druh� vstupn� prom�nn�
     * @param outWordValue  word value of output fuzzy variable<br>slovn� hodnota v�stupn� fuzzy prom�nn�
     */
     //implemented in rules.cc
    void add (const Operations operation, 
              int in1WVIndex, 
              int in2WVIndex, 
              const char *outWordValue);

  protected:
    /** Array of indexes into FuzzyOutput variable.<br> Pole index� do prom�nn� FuzzyOutput. */
    int *outWV; 
    /** 
     * Array of corresponding operations between input variables.<br> Pole odpov�daj�c�ch
     * operac� mezi vstupn�mi prom�nn�mi.
     */
    Operations *operation;
    /** Number of variables and rules in object.<br> Po�et prom�nn�ch a pravidel v objektu */
    int inputs, outputs, rules;
    /** Maximum number of variables.<br> Maxim�ln� po�et prom�nn�ch. */
    static const int MAX_INPUTS = 2;
    static const int MAX_OUTPUTS = 1;
    
    /** It tests if all arrays are allocated.<br>Testuje, jestli u� jsou alokov�na pole. */
    //implemented in rules.cc
    bool isAllCreated();
  private:
    /**
     * It alocates a memory space for operation and outWV arrays. A alocated array size is
     * a product of numbers of word values of all input variables.<br>
     * Alokuje prostor pro pole operation a outWV. Velikost alokovan�ho pole je sou�inem 
     * po�t� slovn�ch hodnot v�ech jednotliv�ch vstupn�ch prom�nn�ch.
     */
     //implemented in rules.cc
    void createVectors();
}; // FuzzyIIORules

/**
 * General form of inference rules.<br>Obecn� tvar inferen�n�ch pravidel.
 *
 * General representation of inference rules, which allows to create more complex fuzzy regulators.
 * Usualy is this class used indirectly over overloaded operators. For more details see the example
 * for class FuzzyExpr and documentation for class Rules.
 *
 * Obecn� reprezentace inferen�n�ch pravidel, kter� dovoluje vytv��et slo�it�j�� fuzzy regul�tory.
 * Obvykle je tato t��da pou��van� nep��mo p�es p�et�en� oper�tory. Dal�� detaily viz p��klad 
 * u t��dy FuzzyExpr a dokumentace pro t��du Rules.
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
     * Testuje, jestli u� jsou p�i�azeny v�echny prom�nn�.
     * @see addFuzzyInput(FuzzyInput *in)
     * @see addFuzzyOutput(FuzzyOutput *out)
     */
     //implemented in rules.cc
    virtual bool isComplete();

    /**
     * It adds next rule into list. When it is too much rules here, error is indicated.<br>
     * 
     * P�id� dal�� pravidlo do seznamu. Pokud u� je definov�no p��li� pravidel, nastane chyba.
     * @param rule Inference rule who is represented by tree structure.<br>
     *             Inferen�n� pravidlo reprezentovan� stromovou strukturou.
     * @param release It has no meaning here.<br>Zde nem� v�znam.
     */
    //implemented in rules.cc
    virtual void add(FuzzyRule *rule, bool release=true);

    /**
     * It evaluates all rules. This method is defaultly called from a method Behavior in class
     * FuzzyRSBlock.<br>
     * Vyhodnot� pravidla. Tato funkce je standardn� vol�na z funkce Behavior t��dy 
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
 * FuzzyRSBlock je b�zov� t��da pro inferen�n� bloky se vzorkovan�xm vstupem a  explicitn�
 * definovan�mi inferen�n�mi pravidly.
 * @ingroup fuzzy
 */
class FuzzyRSBlock
: public FuzzySampledBlock
{
  public:
    /** 
     * Constructor - sets object with inference rules.<br>
     * Konstruktor - nastav� objekt s inferen�n�mi pravidly.  
     */
    FuzzyRSBlock(FuzzyInferenceRules &r) :rules(r) { }
    
    /** Destructor - destruktor. */
    virtual ~FuzzyRSBlock() 
    { TRACE(printf("~FuzzyRuledSampledBlock()\n")); }

    /**
     * It specifies behavior of this block. It evaluates all inference rules. <br>
     * Specifikuje chov�n� tohoto bloku. Vyhodnot� v�echna inferen�n� pravidla.
     */
    virtual void Behavior()
    { rules.evaluate(); }  

  protected:
    FuzzyInferenceRules &rules;
}; // FuzzyRSBlock

///////////////////////////////////////////////////////////////////////////////////////////////
// FuzzyRule
// Reprezentace pravidla pomoc� stromu objekt�. Tento zp�sob realizace pravidla nen� p��li�
// efektivn�. Na druhou stranu je dostate�n� obecn� na to, aby se s t�m dalo pracovat. Tento
// p��stup je zam��len sp�e tak, �e vytv��en� pravidel pomoc� stromu je vn�j�� rozhran�, kter�
// je v r�mci mo�nost� u�ivatelsky p��jemn�. Pravidlo reprezentovan� t�mto stromem se v ka�d�m
// z potomk� t��dy FuzzyInferenceRules p�ekonvertuje na n�jak� efektivn�j�� form�t.  
// 
// Pokud by se ov�em vyskytla pot�eba opravdu obecn�ho z�pisu pravidel, je mo�n� tento strom
// pou��t p��mo. Ve�ker� vyhodnocov�n� stromu se ale mus� d�lat zven��, proto�e t��dy tak, jak 
// jsou nyn� navr�eny mohou slou�it POUZE pro reprezentaci stromu. S p�ihl�dnut�m k tomu, aby
// byla manipulace s objekty co nejjednodu���, maj� v�echny t��dy v�echny sv� datov� prvky 
// ve�ejn�. 
///////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Virtual knot in tree. From descendants is created a tree, who represents inference rule.<br>
 * Virtu�ln� uzel ve stromu. Z potomk� se vytv��� strom, kter� reprezentuje inferen�n� pravidlo.
 * @ingroup fuzzy
 */
class FONode
{
  public:
    /**
     * It does nothing. It is here only to make this class abstract one.<br>
     * Pr�zdn� �ist� virtu�ln� funkce, kter� d�l� z t�to t��dy abstraktn� t��du.
     */
    /**
     * Destruktor
     */
    virtual ~FONode() { TRACE(printf("~FONode\n")); };
    
    /**
     * It returns value of node - for evaluation of rule.<br>Vrac� hodnotu uzlu - pro 
     * vyhodnocov�n� pravidel 
     */
    virtual double getValue() = 0;
}; // FONode

/**
 * Knot in a tree, who represents inference rule. This knot represents an operation.<br>
 * Uzel ve stromu, kter� reprezentuje inferen�n� pravidlo. Tento uzel reprezentuje operaci.
 * @ingroup fuzzy
 */
class FOperation : public FONode
{
  public:
    /** Operator. */
    FuzzyInferenceRules::Operations op;
    
    /** Left operand. <br> Lev� operand. */
    FONode *L;
    
    /** Right operand. <br> Prav� operand. */
    FONode *R;
    
    /**
     * Constructor for a binary operator. <br> Konstruktor pro bin�rn� oper�tor.
     */
    FOperation(FONode *left, FONode *right, FuzzyInferenceRules::Operations operation)
    : //FONode(FONode::ntOperation), 
      op(operation), L(left), R(right){}

    /**
     * Constructor for an unary operator. <br> Konstruktor pro un�rn� oper�tor.
     */
    // implemented in ruletree.cc
    FOperation(FONode *operand, FuzzyInferenceRules::Operations operation);
    
    /**
     * Destructor releases all subtrees. <br> Destruktor uvoln� oba podstromy.
     */
    virtual ~FOperation()
    { 
      if (R != NULL) delete R; 
      if (L != NULL) delete L; 
      TRACE(printf("~FOperation\n"));
    }
    
    /**
     * It returns value after doing operation op.<br>Vrac� hodnotu po proveden� operace op.
     */
    // implemented in ruletree.cc
    virtual double getValue();
    
    
  protected:
  private:
}; // FOperation

/**
 * Leaf of tree. It represents comparing (in == "low") or (in != "too"). If var is 
 * FuzzyOutput, this object can represents assigning (out = "reduce").<br>
 * List stromu. Reprezentuje porovn�n� typu (in == "malo") pripadne (in != "hodne"). Pokud
 * se do prom�nn� var p�i�ad� FuzzyOutput, m��e tento objekt reprezentovat p�i�azen� typu
 * (out = "uber".)
 * @ingroup fuzzy
 */
class FPair : public FONode
{
  public:
    /** Equal or not equal. <br> Rovno nebo nerovno. */
    bool eq;
    /** Fuzzy variable. <br> Fuzzy prom�nn�. */
    FuzzyVariable *var;
    /** Word value. <br> Slovn� hodnota. */
    const char * wordValue;
    /** Index of word value in FuzzyVariable.<br> Index slovn� hodnoty ve FuzzyVariable. */
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
      * Destruktor neuvol�uje pam� alokovanou prom�nn�mi var a wordValue.
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
     * Jestli�e parametr var je FuzzyInput, tato metoda vrac� fuzzifikovanou hodnotu vstupu.
     * Jin�mi slovy - hodnotu funkce p��slu�nosti se jm�nem wordValue. Nepou��vejte jestli�e je 
     * var t��dy FuzzyOutput.
     */
    // implemented in ruletree.cc
    virtual double getValue();
  protected:
  private:
};  // FPair

/**
 * Class for representing one inference rule in tree.<br>
 * T��da reprezentuj�c� jedno inferen�n� pravidlo ve form� stromov� struktury.
 * @ingroup fuzzy
 */
class FuzzyRule
{
  public:
    /** Constructor. */
     //implemented in ruletree.cc
    FuzzyRule();
    
    /**
     * Destructor releases memory alocated by tree. <br> Destruktor uvoln� pam� alokovanou stromem.
     */
     //implemented in ruletree.cc
    virtual ~FuzzyRule();
    
    /** It adds the lvalue of rule. <br> P�id� levou stranu pravidla. */
    //implemented in ruletree.cc
    void addLeft(FOperation *left);
    
    /** It adds one command into list. <br> P�id� dal�� p��kaz do seznamu. */
    //implemented in ruletree.cc
    void addRight(FPair *right);
    
    /**
     * It evaluates one inference rule according to fuzzy model Mamdani.<br>
     * Vyhodnot� jedno inferen�n� pravidlo podle fuzzy modelu Mamdani. 
     */
    //implemented in ruletree.cc
    virtual void evaluate();
    
    /** Lvalue. <br> Lev� strana. */
    FOperation * left;
    
    /** Rvalue. <br> Prav� strana. */
    std::vector<FPair *> right;
  protected:
  private:
}; // FuzzyRule

/**
 * Class for user friendly creating of inference rules.<br>
 * T��da pro u�ivatelsky p��jemn�j�� vytv��en� inferen�n�ch pravidel.
 * @ingroup fuzzy
 */
class FuzzyRuleFactory
{
  public:
    /**
     * It safely creates fuzzy inference rule.<br>
     * Vrac� bezpe�n�m zp�sobem vytvo�en� pravidlo.
     */
    // implemented in ruletree.cc
    FuzzyRule * createRule();

    /**
     * It adds condition of rule.<br>
     * P�id� podm�nkovou ��st do pravidla.
     */
     //implemented in ruletree.cc
    void addCondition(FOperation * operation);

    /**
     * It adds next assign command into rule.<br>
     * P�id� dal�� p�i�azovac� p��kaz do p��kazov� ��sti pravidla (konsekvent). 
     */
     //implemented in ruletree.cc
    void addConsequent(FPair * consequent);  
    
    /**
     * It creates a leaf knot of rule. <br>
     * Vytvo�� listov� uzel pravidla.
     * @param equal true for == and not for !=
     */
     //implemented in ruletree.cc
    FPair * createNode(FuzzyVariable *var, const char * wordvalue, bool equal=true);
    
    /**
     * It creates a nonleaf knot of rule representing binary operator.<br>
     * Vytvo�� nelistov� uzel pravidla reprezentuj�c� bin�rn� oper�tor.
     */
     //implemented in ruletree.cc
    FOperation * createNode(FONode *left, FONode *right, FuzzyInferenceRules::Operations operation);
    
    /**
     * It creates a nonleaf knot of rule representing unary operator.<br>
     * Vytvo�� nelistov� uzel pravidla reprezentuj�c� un�rn� oper�tor.
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
     * Konstruktor je chr�n�n p�ed u�ivatelem. Objekt t�to t��dy m��e vytvo�it
     * pouze objekt t��dy FuzzyInferenceRules.
     */
    friend class FuzzyInferenceRules;
    // implemented in ruletree.cc
    FuzzyRuleFactory(FuzzyInferenceRules * owner);
    FuzzyInferenceRules * owner; /**< Vlastn�k tohoto objektu */
    FuzzyRule * rule;            /**< Pr�v� vytv��en� pravidlo */
  private:
}; // FuzzyRuleFactory



/**
 * A static class for creating general inference rules. See the class FuzzyExpr and FuzzyGeneralRules.
 *
 * Statick� t��da pro vytv��en� obecn�ch inferen�n�ch pravidel. Pod�vejte se na t��du FuzzyExpr a
 * FuzzyGeneralRules.
 */
class Rules
{
  public:

    /**
     * It adds next input into the list. This is needed because of error checking.<br>
     * P�id� dal�� vstup do seznamu. Tato metoda je pot�eba kv�li o�et�en� chyb.
     */
    static void addFuzzyInput(FuzzyInput * input);

    /**
     * It adds next output into the list. This is needed because of error checking.<br>
     * P�id� dal�� v�stup do seznamu. Tato metoda je pot�eba kv�li o�et�en� chyb.
     */
    static void addFuzzyOutput(FuzzyOutput * output);
    
    /**
     * It returns complete definition of inference rules. See at class FuzzyExpr to see how to
     * create inference rules.<br>
     * Vrac� kompletn� definici inferen�n�ch pravidel. Pod�vejte se na t��du FuzzyExpr jak se
     * vytv��� inferen�n� pravidla.
     */
    static FuzzyGeneralRules * getRules();
    
    /**
     * It produces error if there is not enough inputs and outputs or it returns FuzzyRuleFactory.<br>
     * Zp�sob� chybu, jestli�e nen� definov�no dostate�n� mno�stv� vstup� a v�stup� nebo vr�t�
     * FuzzyRuleFactory.
     */
    static FuzzyRuleFactory * Factory();
    
    /**
     * It adds new rule into rules.<br>P�id� nov� pravidlo do rules.
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
 * "User friendly" writing of inference rules.<br>"U�ivatelsky p��telsk� z�pis" inferen�n�ch pravidel.
 *
 * Implementation of "user friendly" writing of inference rules. This way can not be use
 * inside threads because of using global variables!
 *
 * Implementace "u�ivatelsky p��telsk�ho" z�pisu inferen�n�ch pravidel. Tento zp�sob nem��e
 * b�t pou�it uvnit� vl�ken kv�li pou��v�n� glob�ln�ch prom�nn�ch.
 *
 * Example/P��klad:
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
     * Objekty t�to t��dy jsou obvykle vytv��eny uvnit� p��kazu if.
     */
    // implemented in fuzzyrul.cc
    explicit FuzzyExpr(FONode * value);
    /** It returns value of expression.<br>Vrac� hodnotu v�razu. */
    FONode * Value() 
    {
      FONode * node = value;
      value = NULL;
      return node; 
    }
    /**
     * It creates object tree.<br>Vytvo�� strom objekt�.
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
