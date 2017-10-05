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
 * Data, která mù¾e vrátit FuzzyHandler a FuzzyAnalyzer. 
 * @ingroup xml
 */
class FuzzyData
: public AnalyzedData
{
  public:
    /** 
     * Parameterless constructor. Values must be added explicitly.<br>
     * Bezparametrický konstruktor. Hodnoty je potøeba doplnit explicitnì. 
     */
    FuzzyData() : cl(false), in1(NULL), in2(NULL), out(NULL), fset(NULL) 
    { setReleasable(true); }
    /** 
     * It creates a fully defined object which contains data for fuzzy controller creation.<br> 
     * Vytvoøí plnì definovaný objekt obsahující data k vytvoøení fuzzy regulátoru. 
     */
    FuzzyData(FuzzyInput * input1, FuzzyInput * input2, FuzzyOutput * output, FuzzyIIORules * rules)
    : cl(true), in1(input1), in2(input2), out(output), r(rules), fset(NULL) 
    { setReleasable(true); }
    /**
     * It creates a fully defined object which contains fuzzy set definition.<br>
     * Vytvoøí plnì definovaný objekt obsahující definici fuzzy mno¾iny 
     */
    FuzzyData(FuzzySet * set)
    : cl(false), in1(NULL), in2(NULL), out(NULL), fset(set) 
    { setReleasable(true); }
    
    /** 
     * If true was set by setReleasable() method, it dealocates memory for variables.<br>
     * Pokud se metodou setReleasable() nenastavilo false, uvolní pamì» zabranou promìnnými. 
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
    
    /** It returns true, if object is complete.<br> Vrací true, kdy¾ je objekt kompletní. */
    virtual bool isComplete() 
    { return ((in1 != NULL) && (in2 != NULL) && (out != NULL) && (r != NULL)) || (fset != NULL); }
    
    /**
     * It says, if object contains controller definition or fuzzy set definition.
     * If true is returned, content of object is fuzzy regulator definition. In this case
     * method getFuzzySet() NULL returns. If false is result, method getFuzzySet returs 
     * one fuzzy set definition and others get...() methods returns NULL.<br>
     * Urèuje, jestli objekt obsahuje definici regulátoru nebo samostatné fuzzy mno¾iny.
     * Pokud vrátí true, je obsahem objektu definice fuzzy regulátoru. Metoda getFuzzySet() vrací 
     * v tomto pøípadì NULL. Pokud je výsledkem false, metoda getFuzzySet() vrátí definici jedné
     * fuzzy mno¾iny a ostatní funkce get...() vracejí NULL.
     */
    bool isClass() { return cl; }
    
    /** It returns first input variable.<br>Vrací první vstupní fuzzy promìnnou. */
    FuzzyInput * getInput1() { return in1; }
    /** It returns second input variable.<br>Vrací druhou vstupní fuzzy promìnnou. */
    FuzzyInput * getInput2() { return in2; }
    /** It returns output variable.<br>Vrací výstupní fuzzy promìnnou. */
    FuzzyOutput * getOutput() { return out; }
    /** It returns inference rules definition.<br>Vrací definici inferenèních pravidel. */
    FuzzyIIORules * getRules() { return r; }
    /** It returns fuzzy set definition.<br>Vrací definici fuzzy mno¾iny. */
    FuzzySet * getFuzzySet() { return fset; }
    
    /** It sets first input variable.<br>Nastaví první vstupní promìnnou. */
    void setInput1(FuzzyInput * input) { in1 = input; cl = true; }
    /** It sets second input variable.<br>Nastaví druhou vstupní promìnnou. */
    void setInput2(FuzzyInput * input) { in2 = input; cl = true; }
    /** It sets output variable.<br>Nastaví výstupní promìnnou. */
    void setOutput(FuzzyOutput * output) { out = output; cl = true; }
    /** It sets inference rules definition.<br>Nastaví definici inferenèních pravidel. */
    void setRules(FuzzyIIORules * rules) { r = rules; cl = true; }
    /** It sets fuzzy set definition.<br>Nastaví definici fuzzy mno¾iny. */
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
 * Definice analyzátoru xml podle rozhraní SAX2. FuzzyAnalyzer pou¾ívá objekt této tøídy,
 * proto není tøeba jej vytváøet explicitnì.
 * @ingroup xml
 */
class FuzzyHandler : public DefaultHandler
{
  public:
    /** It creates fully defined object.<br>Vytvoøí plnì definovaný objekt. */
    FuzzyHandler() { init(); }
      
    virtual ~FuzzyHandler() {}

    /** It returns true, if error was indicated.<br>Vrací true, kdy¾ byla indikována chyba. */
    bool getSawErrors() const { return fSawErrors;}

    /** It returns number of indicated erros.<br>Vrací poèet indikovaných chyb. */
    int getNumErrors() { return errors; }

    /** It returns number of indicated fatal errors.<br>Vrací poèet indikovaných fatálních chyb. */
    int getNumFatalErrors() { return fatalErrors; }

    /** It returns number of indicated warnings.<br> Vrací poèet indikovaných varování. */
    int getNumWarnings() { return warnings; }

    /** Start document event treatment.<br>O¹etøení události zaèátku dokumentu. */
    virtual void startDocument();

    /** Start element event treatment.<br>O¹etøení události výskytu zaèátku elementu. */
    virtual void startElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname, const Attributes& attrs);

    /** End element event treatment.<br>O¹etøení události výskytu konce elementu. */
    virtual void endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname);
  //  virtual void characters(const XMLCh* const chars, const unsigned int length);
  //  virtual void ignorableWhitespace(const XMLCh* const chars, const unsigned int length);
    /** It sets document locator for error handling.<br>Nastaví lokátor pro obsluhu chyb. */
    virtual void setDocumentLocator(const Locator * const locator)
    { this->locator = locator; }

    /** Printout of warnning.<br>Výpis varování. */
    void warning(const SAXParseException& exception);

    /** Printout of error message.<br>Výpis chybového hlá¹ení. */
    void error(const SAXParseException& exception);

    /** Printout of fatal error message.<br>Výpis chybového hlá¹ení pøi fatální chybì. */
    void fatalError(const SAXParseException& exception);

    /** 
     * It returns analyzed data. User must see about dealocation of memory.<br>
     * Vrací analyzovaná data. U¾ivatel se musí sám postarat o uvolnìní alokované pamìti. 
     */
    FuzzyData * getData() { return data; }
    
    /**
     * It set all variables into their default values.<br>
     * Nastaví implicitní hodnoty v¹ech vnitøních promìnných.
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
 * Tøída FuzzyAnalyzer zapouzdøuje pou¾ití XML parseru v programu a dodává pohodlné
 * rozhraní pro práci. Pro provedení analýzy je tøeba zavolat metodu analyze().
 * @ingroup xml
 */
class FuzzyAnalyzer: public Analyzer
{
  public:
    /** It creates a fully defined object.<br>Vytvoøí plnì definovaný objekt. */
    FuzzyAnalyzer() : validation(true), nameSpaces(true), data(NULL) { }
    virtual ~FuzzyAnalyzer() {}
    /**
     * It analyzes file with xml definition of fuzzy model. XML file must have the same format
     * as MeFE program uses.<br>
     * Analyzuje soubor, který obsahuje xml definici fuzzy modelu. XML musí být stejného
     * formátu jako pou¾ívá program MeFE.
     * @param fileName Name of file with XML data.<br>Jméno souboru s XML daty.
     */
    virtual void analyze(char * fileName);

    /** It returns number of indicated erros.<br>Vrací poèet indikovaných chyb. */
    virtual int getNumErrors() { return handler.getNumErrors(); }
    /** It returns number of indicated fatal errors.<br>Vrací poèet indikovaných fatálních chyb. */
    virtual int getNumFatalErrors() { return handler.getNumFatalErrors(); }
    /** It returns number of indicated warnings.<br> Vrací poèet indikovaných varování. */
    virtual int getNumWarnings() { return handler.getNumWarnings(); }
    
    /**
     *  It returns data obtained by analysis. Product can be retyped into FuzzyData type.<br>
     *  Vrátí data získaná analýzou. Výsledek je mo¾né pøetypovat na FuzzyData.
     */
    virtual AnalyzedData * getAnalyzedData();
    
    /**
     * It sets parameters of analysis. They are set on true by default.<br> 
     * Nastaví parametry analýzy. Implicitnì jsou v¹echny nastaveny na true;
     * @param validation  It enables validation against DTD or XSD.<br>Povoluje ovìøování správnosti proti DTD nebo XSD
     * @param nameSpaces  It enables using of namespaces in XML.<br>Povoluje pou¾ití prostoru jmen v XML
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
 * Vyjímka generovaná metodou create() tøídy MFFactory.
 * @ingroup xml
 */
class MFFException
{
    const char * m;
  public:
    /** It creates a exeption with error message.<br>Vytvoøí výjimku s chybovou hlá¹kou. */
    MFFException(const char *msg) : m(msg) {}
    /** It returns error message.<br>Vrací chybové hlá¹ení. */
    const char * getMsg() const { return m; }
  protected:
  private:
};

/**
 * Class for safe making of FuzzyMembershipFunction.<br>
 * Tøída pro bezpeèné vytváøení FuzzyMembershipFunction.
 * @ingroup xml
 */
class MFFactory
{
  public:
    /**
     * Safe making of FuzzyMembershipFunction. It can generate MFFException.<br>
     * Bezpeèné vytváøení FuzzyMembershipFunction. Mù¾e generovat výjimku MFFException.
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
