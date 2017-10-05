/////////////////////////////////////////////////////////////////////////////
// fuzzyanalyzer.h
//
// SIMLIB version: 2.16.3
// Date: 2001-05-24
// Copyright (c) 1999-2001  David Martinek, Dr. Ing. Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//
// Warning: this is EXPERIMENTAL code, interfaces can be changed
//
// XML data analysis for Fuzzy subsystem for SIMLIB
// version 1 --- Th may 24 18:33:52 CEST 2001 
// 
/////////////////////////////////////////////////////////////////////////////
// For this module Xerces 1.3 library is needed.
/////////////////////////////////////////////////////////////////////////////
#include <sax2/Attributes.hpp>
#include <sax2/DefaultHandler.hpp>
#include "analyzer.h"
#include "fuzzy.h"
#include <sax/Locator.hpp>
/**
 * Data which FuzzyHandler and FuzzyAnalyzer can return.<br>
 * Data, kter� m��e vr�tit FuzzyHandler a FuzzyAnalyzer. 
 * @ingroup xml
 */
class FuzzyData
: public AnalyzedData
{
  public:
    /** 
     * Parameterless constructor. Values must be added explicitly.<br>
     * Bezparametrick� konstruktor. Hodnoty je pot�eba doplnit explicitn�. 
     */
    FuzzyData() : cl(false), in1(NULL), in2(NULL), out(NULL), fset(NULL) 
    { setReleasable(true); }
    /** 
     * It creates a fully defined object which contains data for fuzzy controller creation.<br> 
     * Vytvo�� pln� definovan� objekt obsahuj�c� data k vytvo�en� fuzzy regul�toru. 
     */
    FuzzyData(FuzzyInput * input1, FuzzyInput * input2, FuzzyOutput * output, FuzzyIIORules * rules)
    : cl(true), in1(input1), in2(input2), out(output), r(rules), fset(NULL) 
    { setReleasable(true); }
    /**
     * It creates a fully defined object which contains fuzzy set definition.<br>
     * Vytvo�� pln� definovan� objekt obsahuj�c� definici fuzzy mno�iny 
     */
    FuzzyData(FuzzySet * set)
    : cl(false), in1(NULL), in2(NULL), out(NULL), fset(set) 
    { setReleasable(true); }
    
    /** 
     * If true was set by setReleasable() method, it dealocates memory for variables.<br>
     * Pokud se metodou setReleasable() nenastavilo false, uvoln� pam� zabranou prom�nn�mi. 
     */
    virtual ~FuzzyData() 
    {
      if (releasable)
      {
        if (in1 != NULL) delete in1;
        if (in2 != NULL) delete in2;
        if (out != NULL) delete out;
        if (r != NULL) delete r;
        if (fset != NULL) delete fset;
      }
    }
    
    /** It returns true, if object is complete.<br> Vrac� true, kdy� je objekt kompletn�. */
    virtual bool isComplete() 
    { return ((in1 != NULL) && (in2 != NULL) && (out != NULL) && (r != NULL)) || (fset != NULL); }
    
    /**
     * It says, if object contains controller definition or fuzzy set definition.
     * If true is returned, content of object is fuzzy regulator definition. In this case
     * method getFuzzySet() NULL returns. If false is result, method getFuzzySet returs 
     * one fuzzy set definition and others get...() methods returns NULL.<br>
     * Ur�uje, jestli objekt obsahuje definici regul�toru nebo samostatn� fuzzy mno�iny.
     * Pokud vr�t� true, je obsahem objektu definice fuzzy regul�toru. Metoda getFuzzySet() vrac� 
     * v tomto p��pad� NULL. Pokud je v�sledkem false, metoda getFuzzySet() vr�t� definici jedn�
     * fuzzy mno�iny a ostatn� funkce get...() vracej� NULL.
     */
    bool isClass() { return cl; }
    
    /** It returns first input variable.<br>Vrac� prvn� vstupn� fuzzy prom�nnou. */
    FuzzyInput * getInput1() { return in1; }
    /** It returns second input variable.<br>Vrac� druhou vstupn� fuzzy prom�nnou. */
    FuzzyInput * getInput2() { return in2; }
    /** It returns output variable.<br>Vrac� v�stupn� fuzzy prom�nnou. */
    FuzzyOutput * getOutput() { return out; }
    /** It returns inference rules definition.<br>Vrac� definici inferen�n�ch pravidel. */
    FuzzyIIORules * getRules() { return r; }
    /** It returns fuzzy set definition.<br>Vrac� definici fuzzy mno�iny. */
    FuzzySet * getFuzzySet() { return fset; }
    
    /** It sets first input variable.<br>Nastav� prvn� vstupn� prom�nnou. */
    void setInput1(FuzzyInput * input) { in1 = input; cl = true; }
    /** It sets second input variable.<br>Nastav� druhou vstupn� prom�nnou. */
    void setInput2(FuzzyInput * input) { in2 = input; cl = true; }
    /** It sets output variable.<br>Nastav� v�stupn� prom�nnou. */
    void setOutput(FuzzyOutput * output) { out = output; cl = true; }
    /** It sets inference rules definition.<br>Nastav� definici inferen�n�ch pravidel. */
    void setRules(FuzzyIIORules * rules) { r = rules; cl = true; }
    /** It sets fuzzy set definition.<br>Nastav� definici fuzzy mno�iny. */
    void setFuzzySet(FuzzySet * set) { fset = set; cl = false; }
  protected:
    bool cl;             /**< It contains class or fuzzy set. */
    FuzzyInput * in1;    /**< First input variable.  */
    FuzzyInput * in2;    /**< Second input variable.  */
    FuzzyOutput * out;   /**< Output variable. */ 
    FuzzyIIORules * r;   /**< Inference rules definition. */
    FuzzySet * fset;     /**< Fuzzy set definition. */
  private:
};

/**
 * Definition of analyzer of XML according to SAX2 interface. FuzzyAnalyzer uses object of 
 * this class for it is not necessary to create it explicitly.<br>
 * Definice analyz�toru xml podle rozhran� SAX2. FuzzyAnalyzer pou��v� objekt t�to t��dy,
 * proto nen� t�eba jej vytv��et explicitn�.
 * @ingroup xml
 */
class FuzzyHandler : public DefaultHandler
{
  public:
    /** It creates fully defined object.<br>Vytvo�� pln� definovan� objekt. */
    FuzzyHandler() { init(); }
      
    virtual ~FuzzyHandler() {}

    /** It returns true, if error was indicated.<br>Vrac� true, kdy� byla indikov�na chyba. */
    bool getSawErrors() const { return fSawErrors;}

    /** It returns number of indicated erros.<br>Vrac� po�et indikovan�ch chyb. */
    int getNumErrors() { return errors; }

    /** It returns number of indicated fatal errors.<br>Vrac� po�et indikovan�ch fat�ln�ch chyb. */
    int getNumFatalErrors() { return fatalErrors; }

    /** It returns number of indicated warnings.<br> Vrac� po�et indikovan�ch varov�n�. */
    int getNumWarnings() { return warnings; }

    /** Start document event treatment.<br>O�et�en� ud�losti za��tku dokumentu. */
    virtual void startDocument();

    /** Start element event treatment.<br>O�et�en� ud�losti v�skytu za��tku elementu. */
    virtual void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const Attributes& attrs);

    /** End element event treatment.<br>O�et�en� ud�losti v�skytu konce elementu. */
    virtual void endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);
  //  virtual void characters(const XMLCh* const chars, const unsigned int length);
  //  virtual void ignorableWhitespace(const XMLCh* const chars, const unsigned int length);
    /** It sets document locator for error handling.<br>Nastav� lok�tor pro obsluhu chyb. */
    virtual void setDocumentLocator(const Locator * const locator)
    { this->locator = locator; }

    /** Printout of warnning.<br>V�pis varov�n�. */
    void warning(const SAXParseException& exception);

    /** Printout of error message.<br>V�pis chybov�ho hl�en�. */
    void error(const SAXParseException& exception);

    /** Printout of fatal error message.<br>V�pis chybov�ho hl�en� p�i fat�ln� chyb�. */
    void fatalError(const SAXParseException& exception);

    /** 
     * It returns analyzed data. User must see about dealocation of memory.<br>
     * Vrac� analyzovan� data. U�ivatel se mus� s�m postarat o uvoln�n� alokovan� pam�ti. 
     */
    FuzzyData * getData() { return data; }
    
    /**
     * It set all variables into their default values.<br>
     * Nastav� implicitn� hodnoty v�ech vnit�n�ch prom�nn�ch.
     */
    void init();

  private:
    //  fSawErrors
    //      This is set by the error handlers, and is queryable later to
    //      see if any errors occured.
    bool fSawErrors;                  /**< Are there any error?  */
    int  errors;                      /**< A number of errors.  */
    int  fatalErrors;                 /**< A number of fatal errors. */
    int  warnings;                    /**< A number of warnings. */
    unsigned int  numSets;            /**< A number of actualy defined fuzzy sets. */
    unsigned int  numRows;            /**< A number of actualy readed rows in inference rule matrix. (Section <behavior> in XML.)*/
    unsigned int  rowLength;          /**< A lenght of row in inference rule matrix. */
    FuzzyInput * in1;                 /**< First input variable. */
    FuzzyInput * in2;                 /**< Second input variable. */
    FuzzyOutput * out;                /**< Output variable. */
    FuzzyIIORules * rules;            /**< A sefinition of fuzzy inference rules. */
    FuzzyData * data;                 /**< An data for returning. */
 
    FuzzySet * fset;                  /**< A definition of fuzzy set. */
    FuzzyMembershipFunction * mf;     /**< An auxiliary variable.  */
    bool isClass;                     /**< Is class defined? */
    
    const Locator * locator;          /**< A locator for error handling. */
};

/** 
 * Class FuzzyAnalyzer encapsulates use of XML parser in program and provides with comfortable
 * interface. For analysis is need to call method analyze().
 * T��da FuzzyAnalyzer zapouzd�uje pou�it� XML parseru v programu a dod�v� pohodln�
 * rozhran� pro pr�ci. Pro proveden� anal�zy je t�eba zavolat metodu analyze().
 * @ingroup xml
 */
class FuzzyAnalyzer: public Analyzer
{
  public:
    /** It creates a fully defined object.<br>Vytvo�� pln� definovan� objekt. */
    FuzzyAnalyzer() : validation(true), nameSpaces(true), data(NULL) { }
    virtual ~FuzzyAnalyzer() {}
    /**
     * It analyzes file with xml definition of fuzzy model. XML file must have the same format
     * as MeFE program uses.<br>
     * Analyzuje soubor, kter� obsahuje xml definici fuzzy modelu. XML mus� b�t stejn�ho
     * form�tu jako pou��v� program MeFE.
     * @param fileName Name of file with XML data.<br>Jm�no souboru s XML daty.
     */
    virtual void analyze(char * fileName);

    /** It returns number of indicated erros.<br>Vrac� po�et indikovan�ch chyb. */
    virtual int getNumErrors() { return handler.getNumErrors(); }
    /** It returns number of indicated fatal errors.<br>Vrac� po�et indikovan�ch fat�ln�ch chyb. */
    virtual int getNumFatalErrors() { return handler.getNumFatalErrors(); }
    /** It returns number of indicated warnings.<br> Vrac� po�et indikovan�ch varov�n�. */
    virtual int getNumWarnings() { return handler.getNumWarnings(); }
    
    /**
     *  It returns data obtained by analysis. Product can be retyped into FuzzyData type.<br>
     *  Vr�t� data z�skan� anal�zou. V�sledek je mo�n� p�etypovat na FuzzyData.
     */
    virtual AnalyzedData * getAnalyzedData();
    
    /**
     * It sets parameters of analysis. They are set on true by default.<br> 
     * Nastav� parametry anal�zy. Implicitn� jsou v�echny nastaveny na true;
     * @param validation  It enables validation against DTD or XSD.<br>Povoluje ov��ov�n� spr�vnosti proti DTD nebo XSD
     * @param nameSpaces  It enables using of namespaces in XML.<br>Povoluje pou�it� prostoru jmen v XML
     */
    void setParameters(bool validation, bool nameSpaces)
    {
      this->validation = validation;
      this->nameSpaces = nameSpaces;
    }
    
    
  protected:
    FuzzyHandler handler;   /**< Handler for SAX2 interface. */
    bool validation;        /**< Can be validation used? */
    bool nameSpaces;        /**< Can be namespaces used? */
    FuzzyData *data;        /**< Obtained data. */
  private:
};


/**
 * Exception generated by method create() of class MFFactory.<br>
 * Vyj�mka generovan� metodou create() t��dy MFFactory.
 * @ingroup xml
 */
class MFFException
{
    const char * m;
  public:
    /** It creates a exeption with error message.<br>Vytvo�� v�jimku s chybovou hl�kou. */
    MFFException(const char *msg) : m(msg) {}
    /** It returns error message.<br>Vrac� chybov� hl�en�. */
    const char * getMsg() const { return m; }
  protected:
  private:
};

/**
 * Class for safe making of FuzzyMembershipFunction.<br>
 * T��da pro bezpe�n� vytv��en� FuzzyMembershipFunction.
 * @ingroup xml
 */
class MFFactory
{
  public:
    /**
     * Safe making of FuzzyMembershipFunction. It can generate MFFException.<br>
     * Bezpe�n� vytv��en� FuzzyMembershipFunction. M��e generovat v�jimku MFFException.
     */
    static FuzzyMembershipFunction * create(const char* mftype, const char* wordvalue)
    {
      if (XMLString::compareString(mftype, "FuzzyTriangle") == 0)
      { return new FuzzyTriangle(wordvalue); }
      else if (XMLString::compareString(mftype, "FuzzyTrapez") == 0)
      { return new FuzzyTrapez(wordvalue); }
      else if (XMLString::compareString(mftype, "FuzzyGauss") == 0)
      { return new FuzzyGauss(wordvalue); }
      else if (XMLString::compareString(mftype, "FuzzySingleton") == 0)
      { return new FuzzySingleton(wordvalue); }
      else
        throw MFFException("Unknown type of member function!");
    }
   
  protected:
  private:
};
