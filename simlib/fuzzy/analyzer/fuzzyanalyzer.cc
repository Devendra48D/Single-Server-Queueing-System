/////////////////////////////////////////////////////////////////////////////
// fuzzyanalyzer.cc
//
// SIMLIB version: 2.16.3
// Date: 2001-05-24
// Copyright (c) 1999-2001  David Martinek, Dr. Ing. Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//
// Warning: this is EXPERIMENTAL code, interfaces can be changed
//
// Fuzzy subsystem for SIMLIB
// version 1 --- Th may 24 18:33:52 CEST 2001 
// 
/////////////////////////////////////////////////////////////////////////////
// Implementation of fuzzy XML analyzer according to SAX2.
// For this module Xerces 1.3 library is needed.
/////////////////////////////////////////////////////////////////////////////

//#include <iostream>
#include <stdlib.h>
#include <errno.h>

#include "simlib.h"
#include <internal.h>

#include "fuzzyanalyzer.h"
#include <strx.h>

#include <util/PlatformUtils.hpp>
#include <sax2/SAX2XMLReader.hpp>
#include <sax2/XMLReaderFactory.hpp>


/**
 * It analyzes file with xml definition of fuzzy model. XML file must have the same format
 * as MeFE program uses.<br>
 * Analyzuje soubor, který obsahuje xml definici fuzzy modelu. XML musí být stejného
 * formátu jako pou¾ívá program MeFE.
 * @param fileName Name of file with XML data.<br>Jméno souboru s XML daty.
 */
void FuzzyAnalyzer::analyze(char * fileName)
{
  try
  {
    XMLPlatformUtils::Initialize();
    handler.init();
  }
  catch(const XMLException& e)
  {
    StrX emsg(e.getMessage());
    SIMLIB_error(emsg.toAscii());
  }
  SAX2XMLReader * parser = XMLReaderFactory::createXMLReader();
  parser->setFeature(XMLString::transcode("http://xml.org/sax/features/validation"), validation);
  parser->setFeature(XMLString::transcode("http://xml.org/sax/features/namespaces"), nameSpaces);
  parser->setFeature(XMLString::transcode("http://apache.org/xml/features/validation/dynamic"), false);
  
  try
  {
    parser->setContentHandler(&handler);
    parser->setErrorHandler(&handler);
    const unsigned long startMillis = XMLPlatformUtils::getCurrentMillis();
    parser->parse(fileName);
    const unsigned long stopMillis = XMLPlatformUtils::getCurrentMillis();
    analyzeTime = stopMillis - startMillis;
    data = handler.getData();
  }
  catch(const XMLException& e)
  {
    StrX emsg(e.getMessage());
    SIMLIB_error(emsg.toAscii());
  }
  
  delete parser;

  XMLPlatformUtils::Terminate();
}  
/**
 * It returns data obtained by analysis. Product can be retyped into FuzzyData type.<br>
 * Vrátí data získaná analýzou. Výsledek je mo¾né pøetypovat na FuzzyData.
 */
AnalyzedData * FuzzyAnalyzer::getAnalyzedData()
{
  return data;
}

/////////////////////////////////////////////////////////////////////////////////////
//
//   FuzzyHandler
//
/////////////////////////////////////////////////////////////////////////////////////

/**
 * It set all variables into their default values.<br>
 * Nastaví implicitní hodnoty v¹ech vnitøních promìnných.
 */
void FuzzyHandler::init()
{
  fSawErrors = false;
  errors = 0;
  fatalErrors = 0;
  warnings = 0;
  in1 = NULL;
  in2 = NULL;
  out = NULL;
  rules = NULL;
  data  = NULL;
  fset = NULL;
  mf = NULL;
  isClass = false;
}


// ---------------------------------------------------------------------------
//  fuzzyhandler: Implementation of the SAX DocumentHandler interface
// ---------------------------------------------------------------------------
/** Start document event treatment.<br>O¹etøení události zaèátku dokumentu. */
void FuzzyHandler::startDocument()
{
//  cout << "startDocument" << endl;
  numSets = 0;
  numRows = 0;
  rowLength = 0;
}

/** Start element event treatment.<br>O¹etøení události výskytu zaèátku elementu. */
void FuzzyHandler::startElement(const XMLCh* const uri
                                   , const XMLCh* const localname
                                   , const XMLCh* const qname
                                   , const Attributes& attrs)
{
// Nikdo tuto metodu v zájmu svého du¹evního zdraví NEZKOUMEJTE, proto¾e je del¹í ne¾ je zdrávo.
// Autor.
  errno = 0;  
  SAXParseException *spe;
//cout << "startElement" << endl;
  StrX *local = new StrX(localname);
  if (local->equals("fuzzyclass"))
  {
    isClass = true;
  }
  else if (local->equals("fuzzytype"))
  {
    StrX *parameter = new StrX("minrange");
    StrX *value = new StrX(attrs.getValue(parameter->toUnicode()));
//cout << "<fuzzytype minrange=" << value->toAscii();
    double min = strtod(value->toAscii(), (char**)NULL);
    if (errno != 0)
    {
      value->setValue("Bad number format for attribute \"minrange\"!");
      spe = new SAXParseException(value->toUnicode(), *locator);
      error(*spe);
      delete parameter;
      throw spe;
    }
    parameter->setValue("maxrange");
    value->setValue(attrs.getValue(parameter->toUnicode()));
//cout << " maxrange=" << value->toAscii();
    double max = strtod(value->toAscii(), (char**)NULL);
    if (errno != 0)
    {
      value->setValue("Bad number format for attribute \"maxrange\"!");
      spe = new SAXParseException(value->toUnicode(), *locator);
      error(*spe);
      delete parameter;
      throw spe;
    }
    
    parameter->setValue("typename");
    value->setValue(attrs.getValue(parameter->toUnicode()));
//cout << "typename=" << value->toAscii() << ">" << endl;
    fset = new FuzzySet(value->toAscii(), min, max);
    delete parameter;
    delete value;
  }
  else if (local->equals("fuzzymf"))
  {
    StrX* mftype = new StrX("mftype");
    mftype->setValue(attrs.getValue(mftype->toUnicode()));
    StrX* wordvalue = new StrX("wordvalue");
    wordvalue->setValue(attrs.getValue(wordvalue->toUnicode()));
    try
    {
//cout << "<fuzzymf mftype=" << mftype->toAscii() << " wordvalue=" << wordvalue->toAscii() << ">" << endl;
      mf = MFFactory::create(mftype->toAscii(), wordvalue->toAscii());
    }
    catch(const MFFException& e)
    {
      wordvalue->setValue(e.getMsg());
      spe = new SAXParseException(wordvalue->toUnicode(), *locator);
      error(*spe);
      delete mftype;
      throw spe;
    }
    delete mftype;
    delete wordvalue;    
  }
  else if (local->equals("value"))
  {
    StrX * value = new StrX("value");
    value->setValue(attrs.getValue(value->toUnicode()));
//cout << "<value value=" << value->toAscii() << ">" << endl;
    double val = strtod(value->toAscii(), (char**)NULL);
    if (errno != 0)
    {
      value->setValue("Bad number format for attribute \"value\"!");
      spe = new SAXParseException(value->toUnicode(), *locator);
      error(*spe);
      throw spe;
    }
    mf->addDefValue(val);
    delete value;
  }
  else if (local->equals("behavior"))
  {
//cout << "<behavior>" << endl;
    rules = new FuzzyIIORules(in1, in2, out);
  }
  else if (local->equals("row"))
  {
//cout << "<row>" << endl;
    numRows++;
    rowLength = 0;
  }
  else if (local->equals("outvalue"))
  {
    rowLength++;
    StrX *value = new StrX("value");
    value->setValue(attrs.getValue(value->toUnicode()));
//cout << "<outvalue value=" << value->toAscii() << ">" << endl;
    rules->add(FuzzyIIORules::opAND, rowLength-1, numRows-1, value->toAscii());
  }
  
  delete local;
//    fElementCount++;
//    fAttrCount += attrs.getLength();
}

/** End element event treatment.<br>O¹etøení události výskytu konce elementu. */
void FuzzyHandler::endElement(const XMLCh* const uri, const XMLCh* const localname, const XMLCh* const qname)
{
//cout << "endElement" << endl;
  SAXParseException *spe;
  StrX * local = new StrX(localname);
  if (local->equals("fuzzymf"))
  {
    fset->add(*mf);
    delete mf;
    mf = NULL;
  }
  else if (local->equals("fuzzytype"))
  {
    if (isClass) 
    {
      switch (numSets++)
      {
        case 0:
          in1 = new FuzzyInput(*(this->fset));
          break;
        case 1:
          in2 = new FuzzyInput(*fset);
          break;
        case 2:
          out = new FuzzyOutput(*fset);
          break;
        default: break;
      }
      delete fset;
      fset = NULL;
    }
    else
    {
      data = new FuzzyData(fset);
    }
  }
  else if (local->equals("row"))
  {
    if (rowLength != in1->count())
    {
      local->setValue("Bad count of \"outvalue\" tags!");
      spe = new SAXParseException(local->toUnicode(), *locator);
      error(*spe);
      throw spe;
    }
  }
  else if (local->equals("behavior"))
  {
    if (numRows != in2->count())
    {
      local->setValue("Bad count of \"rows\" tags!");
      spe = new SAXParseException(local->toUnicode(), *locator);
      error(*spe);
      throw spe;
    }
  }
  else if (local->equals("fuzzyclass"))
  {
    data = new FuzzyData(in1, in2, out, rules);
  }
  
  delete local;
}

// ---------------------------------------------------------------------------
//  FuzzyHandler: Overrides of the SAX ErrorHandler interface
// ---------------------------------------------------------------------------
/** Printout of error message.<br>Výpis chybového hlá¹ení. */
void FuzzyHandler::error(const SAXParseException& e)
{
  fSawErrors = true;
  errors++;
  cerr << "\nError at file " << StrX(e.getSystemId())
	 << ", line " << e.getLineNumber()
	 << ", char " << e.getColumnNumber()
   << "\n  Message: " << StrX(e.getMessage()) << endl;
}

/** Printout of fatal error message.<br>Výpis chybového hlá¹ení pøi fatální chybì. */
void FuzzyHandler::fatalError(const SAXParseException& e)
{
  fSawErrors = true;
  fatalErrors++;
  cerr << "\nFatal Error at file " << StrX(e.getSystemId())
	 << ", line " << e.getLineNumber()
	 << ", char " << e.getColumnNumber()
   << "\n  Message: " << StrX(e.getMessage()) << endl;
}

/** Printout of warnning.<br>Výpis varování. */
void FuzzyHandler::warning(const SAXParseException& e)
{
  warnings++;
  cerr << "\nWarning at file " << StrX(e.getSystemId())
	 << ", line " << e.getLineNumber()
	 << ", char " << e.getColumnNumber()
   << "\n  Message: " << StrX(e.getMessage()) << endl;
}
