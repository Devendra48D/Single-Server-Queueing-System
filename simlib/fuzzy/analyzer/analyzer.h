/////////////////////////////////////////////////////////////////////////////
// analyzer.h
//
// SIMLIB version: 2.16.3
// Date: 2001-04-04
// Copyright (c) 1999-2001  David Martinek, Dr. Ing. Petr Peringer
//
// This library is licensed under GNU Library GPL. See the file COPYING.
//
// Warning: this is EXPERIMENTAL code, interfaces can be changed
//
// General data extraction subsystem for SIMLIB
// version 0.6 --- We apr 4 10:33:52 CEST 2001 
// 
/////////////////////////////////////////////////////////////////////////////
// A general interface for external data analysis. It can be used not only for
// XML analysis.
/////////////////////////////////////////////////////////////////////////////

#ifndef ANALYZER_H
#define ANALYZER_H
/**
 * An abstract class. Its descendants contains analyzed data.<br>
 * Abstraktni trida. Jeji potomci obsahuji analyzovana data.
 * @ingroup xml
 */
class AnalyzedData
{
  public:
    /**
     * It returns true, if data are complete.<br>
     * Vrac� true, kdy� jsou data kompletn�.
     */
    virtual bool isComplete() = 0;
    /**
     * If true is set, it will be dealocated all data members of object by destruction.<br>
     * Pokud je nastaveno true, budou se p�i destrukci objektu dealokovat i v�echny datov� �leny.
     */
    void setReleasable(bool release) { releasable = release; }
  protected:
    bool releasable; /**< Data will be dealocated or not. */
  private:
};

/**
 * An abstract interface for external data analysis.<br>
 * T��da Anaylser je abstraktn� t��da, kter� slou�� jako rozhran� pro anal�zu extern�ch dat.
 * @ingroup xml
 */
class Analyzer
{
  public:
    Analyzer() { analyzeTime = 0; }
    virtual ~Analyzer() {}
    /**
     * It realizes the analysis.<br>Provede anal�zu zadan�ho souboru.
     */
    virtual void analyze(char * fileName) = 0;
    /**
     * It gets analyzed data.<br>Vr�t� data z�skan� anal�zou. 
     */
    virtual AnalyzedData * getAnalyzedData() = 0;
    /**
     * It returns time in miliseconds which is needed for analysis.<br>
     * Vrac� �as v milisekund�ch spot�ebovan� anal�zou dat.
     */
    unsigned long getAnalyzeTime() { return analyzeTime; }
    /**
     * It returns number of indicated erros.<br>Vrac� po�et indikovan�ch chyb.
     */
    virtual int getNumErrors() = 0;
    /**
     * It returns number of indicated fatal errors.<br>Vrac� po�et indikovan�ch fat�ln�ch chyb.
     */
    virtual int getNumFatalErrors() = 0;
    /**
     * It returns number of indicated warnings.<br> Vrac� po�et indikovan�ch varov�n�.
     */
    virtual int getNumWarnings() = 0;
  protected:
    unsigned long analyzeTime; /**< Time needed for analysis.<br>Doba spot�ebovan� na anal�zu dat. */
  private:
};

#endif
