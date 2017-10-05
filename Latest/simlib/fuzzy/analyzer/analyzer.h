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
     * Vrací true, kdy¾ jsou data kompletní.
     */
    virtual bool isComplete() = 0;
    /**
     * If true is set, it will be dealocated all data members of object by destruction.<br>
     * Pokud je nastaveno true, budou se pøi destrukci objektu dealokovat i v¹echny datové èleny.
     */
    void setReleasable(bool release) { releasable = release; }
  protected:
    bool releasable; /**< Data will be dealocated or not. */
  private:
};

/**
 * An abstract interface for external data analysis.<br>
 * Tøída Anaylser je abstraktní tøída, která slou¾í jako rozhraní pro analýzu externích dat.
 * @ingroup xml
 */
class Analyzer
{
  public:
    Analyzer() { analyzeTime = 0; }
    virtual ~Analyzer() {}
    /**
     * It realizes the analysis.<br>Provede analýzu zadaného souboru.
     */
    virtual void analyze(char * fileName) = 0;
    /**
     * It gets analyzed data.<br>Vrátí data získaná analýzou. 
     */
    virtual AnalyzedData * getAnalyzedData() = 0;
    /**
     * It returns time in miliseconds which is needed for analysis.<br>
     * Vrací èas v milisekundách spotøebovaný analýzou dat.
     */
    unsigned long getAnalyzeTime() { return analyzeTime; }
    /**
     * It returns number of indicated erros.<br>Vrací poèet indikovaných chyb.
     */
    virtual int getNumErrors() = 0;
    /**
     * It returns number of indicated fatal errors.<br>Vrací poèet indikovaných fatálních chyb.
     */
    virtual int getNumFatalErrors() = 0;
    /**
     * It returns number of indicated warnings.<br> Vrací poèet indikovaných varování.
     */
    virtual int getNumWarnings() = 0;
  protected:
    unsigned long analyzeTime; /**< Time needed for analysis.<br>Doba spotøebovaná na analýzu dat. */
  private:
};

#endif
