/////////////////////////////////////////////////////////////////////////////
// strx.h
//
// SIMLIB version: 2.16.3
// Date: 2001-04-04
// Copyright (c) 1999-2001  David Martinek, Dr. Ing. Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//
// Warning: this is EXPERIMENTAL code, interfaces can be changed
//
// Fuzzy subsystem for SIMLIB
// version 0.6 --- We apr 4 10:33:52 CEST 2001 
// 
/////////////////////////////////////////////////////////////////////////////
// Utility for transcoding between unicode and ascii.
/////////////////////////////////////////////////////////////////////////////

#include <iostream.h>
#include <util/PlatformUtils.hpp>
#include <util/XMLString.hpp>

//#include <stdlib.h>
//#include <sax2/XMLReaderFactory.hpp>
//#include <sax2/SAX2XMLReader.hpp>

/**
 * Class for transcoding between unicode and ascii.<br>Tøída pro pøevod mezi unicode a ascii.
 *
 * This is a simple class that lets us do easy (thoutgh not terribly efficient)
 * transcoding of XMLCh data (unicode) to local code page (ascii) and on the 
 * contrary.
 *
 * Toto je jednoduchá tøída, která umo¾òuje jednodu¹e (i kdy¾ ne pøíli¹ efektivnì)
 * pøevádìt data z XMLCh (unicode) do lokálního kódování (ascii) a naopak. 
 * @ingroup xml
 */
class StrX
{
public :
    // -----------------------------------------------------------------------
    //  Constructors and Destructor
    // -----------------------------------------------------------------------
    /** 
     * It creates a copy according to unicode value.<br>
     * Vytvoøí kopii podle unicode hodnoty. 
     */
    StrX(const XMLCh* const value) : ascii(NULL), unicode(NULL)
    {
      setValue(value);
    }
    /** 
     * It creates a copy according to ascii value.<br>
     * Vytvoøí kopii podle ascii hodnoty. 
     */
    StrX(const char* const value) : ascii(NULL), unicode(NULL)
    {
      setValue(value);
    }
    
    /** 
     * It creates a copy according to unicode value.<br>
     * Vytvoøí kopii podle unicode hodnoty. 
     */
    void setValue(const XMLCh* const value)
    {
      release();
      ascii = XMLString::transcode(value);
      unicode = XMLString::replicate(value);
    }
    
    /** 
     * It creates a copy according to ascii value.<br>
     * Vytvoøí kopii podle ascii hodnoty. 
     */
    void setValue(const char* const value)
    {
      release();
      ascii = XMLString::replicate(value);
      unicode = XMLString::transcode(value);
    }
    /** It releases memory.<br>Uvolní pamì». */
    ~StrX()
    {
      release();
    }

    // -----------------------------------------------------------------------
    //  Getter methods
    // -----------------------------------------------------------------------
    /** It returns ascii string.<br>Vrátí ascii øetìzec. */
    const char* toAscii() const
    {
        return ascii;
    }
    /** It returns unicode strign.<br>Vrátí unicode øetìzec. */
    const XMLCh* toUnicode() const
    {
      return unicode;
    }
    /** Comparising with ascii string.<br>Porovnání s ascii øetìzcem. */
    bool equals(const char * str)
    {
      return XMLString::compareString(ascii, str) == 0;
    }
    /** Comparising with unicode string.<br>Porovnání s unicode øetìzcem. */
    bool equals(const XMLCh * str)
    {
      return XMLString::compareString(unicode, str) == 0;
    }

private :
    // -----------------------------------------------------------------------
    //  Private data members
    //
    //  ascii
    //      This is the local code page form of the string.
    // -----------------------------------------------------------------------
    char*   ascii;    /**< Ascii string. */
    XMLCh*  unicode;  /**< Unicode string. */
    /** It releases memory.<br>Uvolní pamì». */
    void release()
    {
      if (ascii != NULL) delete [] ascii;
      if (unicode != NULL) delete [] unicode;
      ascii = NULL;
      unicode = NULL;
    }
};

/**
 * @ingroup xml
 * Writing into stream.<br>Zápis do streamu. 
 */
inline ostream& operator<<(ostream& target, const StrX& toDump)
{
    target << toDump.toAscii();
    return target;
}
