////////////////////////////////////////////////////////////////////////////
// Model pocsit                   SIMLIB/C++
//
// Pøíklad simulaèního programu - poèítaèová sí» v obchodním domu
//

// èas simulace:
//                 113 s na 386/40 bez koprocesoru
//                  31 s na 386/40 s koprocesorem
//                  10 s na P90/Win95/DosPrompt
//

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// Zadání:                                                                 //
//  -------                                                                //
//    Obchodní dùm  je vybaven poèítaèovou sítí,  která spojuje jednotlivá //
//  oddìlení a sklady a øídí se pomocí ní distribuce zbo¾í. Ka¾dé prodejní //
//  oddìlení obchodního  domu je vybaveno mikropoèítaèem,  ke kterému jsou //
//  pøipojeny  pokladny  daného  oddìlení  a  terminál vedoucího oddìlení, //
//  kterı  umo¾òuje  získávat  pøehled  o  zbo¾í  a tr¾bì. Mikropoèítaè je //
//  spojen  s  hlavním  poèítaèem,  kterı  v  urèitıch intervalech pøebírá //
//  informace o prodaném zbo¾í.                                            //
//                                                                         //
//      Bloková struktura mikropoèítaèe na oddìlení :                      //
//                                                                         //
// +----------------------------------------------------------------------‹//
// |‚‹                                                          |//
// |ˆ mikro-   ˜˜˜˜˜‹     |//
// |ˆ procesor ˆ   ˆ       ˆ       ˆ         ˆ         ˆ            ˆ     |//
// |ŸŠ   ˆ       ˆ       ˆ         ˆ         ˆ            ˆ     |//
// |            ‚–‹ ‚–‹ ‚–‹  ‚–‹ ‚–‹    ‚–‹ |//
// |            ˆ RAM ˆ ˆ DMA ˆ ˆ PPI ˆ  ˆ USART ˆ ˆ USART ˆ .. ˆ USART ˆ |//
// |            ŸŠ Ÿ˜Š Ÿ˜Š  Ÿ˜Š Ÿ˜Š    Ÿ˜Š |//
// |                       ˆ       ˆ          ˆ        ˆ            ˆ     |//
// |                   ‚–‹   ˆ          ˆ        ˆ            ˆ     |//
// |                   ˆ USART ˆ   ˆ          ˆ        ˆ            ˆ     |//
// |                   Ÿ˜Š   ˆ          ˆ        ˆ            ˆ     |//
// Ÿ-----------------------ˆ-------ˆ----------ˆ--------ˆ------------ˆ-----Š//
//                         ˆ       ˆ          ˆ        ˆ            ˆ      //
//       hlavní poèítaè  <Š  ‚–‹ ‚–‹ ‚–‹      ‚–‹   //
//                            ˆ terminál ˆ ˆ      ˆ ˆ     ˆ ...  ˆ     ˆ   //
//                            ŸŠ ŸŠ ŸŠ      ŸŠ   //
//                                                 POKLADNY                //
//                                                                         //
//  Pomocí  modelu   provìøte,  zda  vyhovuje   následující  konfigurace   //
// mikropoèítaèe na jednom oddìlení obchodního domu :                      //
//                                                                         //
// - poèet pokladen : 3                                                    //
// - doba  mezi pøíchody po¾adavkù  na zaplacení 1  ks zbo¾í je  náhodná   //
//   velièina  s exponenciálním  rozlo¾ením pravdìpodobnosti  se støední   //
//   hodnotou 10 s                                                         //
// - pøenosová rychlost mezi pokladnou a mikropoèítaèem je 1200 b/s        //
// - velikost  pøená¹eného bloku informací pøíslu¹ejícího  1 ks zbo¾í je   //
//   100 bytù                                                              //
// - doba mezi otázkami zadávanımi na terminál je náhodná velièina s expo- //
//   nenciálním rozlo¾ením pravdìpodobnosti se støední hodnotou 10 minut   //
// - pøenosová rychlost mezi terminálem a mikropoèítaèem je 1 byte/100 ©s  //
// - velikost bloku pøíslu¹ejícího otázce i odpovìdi je 100 a¾ 1000 bytù   //
//   (rovnomìrné rozlo¾ení)                                                //
// - doba,  ne¾ hlavní poèítaè pøevezme  otázku : 0 a¾  10 s (rovnomìrné   //
//   rozlo¾ení)                                                            //
// - doba vypracování odpovìdi : 0.5 a¾ 15 s (rovnomìrné rozlo¾ení)        //
// - interval mezi pøenosy dat  do hlavního poèítaèe je náhodná velièina   //
//   s exponenciálním rozlo¾ením pravdìpodobnosti  se støední hodnotou 5   //
//   minut                                                                 //
// - pøenosová rychlost mezi hlavním poèítaèem a mikropoèítaèem je 19200   //
//   bitù za sekundu                                                       //
// - kapacita pamìti mikropoèítaèe je 24 kB                                //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#include "simlib.h"

// deklarace konstant

#define T_POC  0.0         // doba simulace poèáteèní (v sek)
#define T_KON 10.0*3.6e3   //               koneèná
#define P_POKL 3           // poèet pokladen
#define T_POKL 10.0        // støední doba mezi pøíchody po¾adavkù na pokladnu
#define V_POKLMIKR  1.2e3  // pøenosová rychlost pokladna - mikropoèítaè (b/s)
#define V_TERMMIKR  8.0e4  // pøenosová rychlost terminál - mikropoèítaè (b/s)
#define V_HLPOCMIKR 1.92e4 // pøenosová rychlost hl.poèítaè - mikropoèítaè (b/s)
#define S_BLOKZBOZI  100   // velikost bloku pøíslu¹ejícího 1 ks zbo¾í
#define Smin_DOTAZODPOV  100 // velikost bloku (minim.) pro otázku a odpovìï
#define Smax_DOTAZODPOV 1000 //     -  ||  -   (maxim.)   -  ||  -
#define T_TERM 6e2           // doba mezi zadáním nové úlohy na terminál
#define Tmin_PREVDOTAZU 0.0  // doba pøevzetí dotazu hl. poèítaèem (minim.)
#define Tmax_PREVDOTAZU 10.0 //             -  ||  -               (maxim.)
#define Tmin_ODPOVEDI    0.5 // doba vypracování odpovìdi (minim.)
#define Tmax_ODPOVEDI   15.0 //      -  ||  -             (maxim.)
#define T_HLPOC 1.8e2        // støední doba mezi pøenosy dat do hl. poèítaèe
#define KAP_PAMETI 24576     // bytù

// deklarace globálních objektù

Store Pamet("Pamì»",KAP_PAMETI);
Facility Procesor("Procesor"),
	 Sbernice("Sbìrnice"),
	 DMA("DMA");
Histogram TPokladna ("Doba obsluhy po¾adavkù z pokladny",0,1,10);
Histogram TTerminal ("Doba obsluhy po¾adavkù z terminálu",0,5,10);
unsigned ObsazKap = 0;  // kapacita pamìti, urèené pro pøenos do hl. poèítaèe


class Pokladna : public Process {  // tøída úloh zpracovávanıch pokladnou
  double Prichod;
  void Behavior() {
    Prichod = Time;      // doba pøíchodu po¾adavku na nìkterou z pokladen
    Seize(Procesor); //,1);   // vy¹¹í priorita, aby pokladna dlouho neèekala
    Seize(Sbernice);
    Enter(Pamet,S_BLOKZBOZI);
    ObsazKap+=S_BLOKZBOZI;
    Wait(S_BLOKZBOZI*8/V_POKLMIKR);  // pøenos bloku do mikropoèítaèe
    Release(Sbernice);
    Release(Procesor);
    TPokladna(Time - Prichod);  // záznam doby zpracování do tabulky
  }
 public: Pokladna() { Activate(); }
};

class GenerPokladna : public Event {  // tøída generátorù úloh pro pokladny
  void Behavior() {                   // popis chování generátoru
    new Pokladna;
    Activate(Time+Exponential(T_POKL));
  }
 public: GenerPokladna() { Activate(); }
};


class Terminal : public Process { // tøída úloh zpracovávanıch terminálem
  double Prichod;                 // èas vzniku úlohy zpracovávané terminálem
  int Otazka, Odpoved;            // velikost otázky a odpovìdi v bytech
  void Behavior() {
    Prichod = Time;
    Seize(Procesor);
    Seize(Sbernice);
    Otazka = int(Uniform(Smin_DOTAZODPOV,Smax_DOTAZODPOV));
    Enter(Pamet,Otazka);
    Wait(Otazka*8/V_TERMMIKR);   // pøenos otázky do mikropoèítaèe
    Release(Sbernice);
    Release(Procesor);
    Wait(Uniform(Tmin_PREVDOTAZU,Tmax_PREVDOTAZU));
				 // pøevzetí otázky hl. poèítaèem
    Seize(DMA);
    Seize(Sbernice,1);
    Wait(Otazka*8/V_HLPOCMIKR);  // pøenos otázky do hl. poèítaèe
    Leave(Pamet,Otazka);
    Release(Sbernice);
    Release(DMA);
    Wait(Uniform(Tmin_ODPOVEDI,Tmax_ODPOVEDI));  // vypracování odpovìdi
    Odpoved = int(Uniform(Smin_DOTAZODPOV,Smax_DOTAZODPOV));
    Seize(DMA);
    Seize(Sbernice,1);
    Enter(Pamet,Odpoved);
    Wait(Odpoved*8/V_HLPOCMIKR);  // pøenos odpovìdi do mikropoèítaèe
    Release(Sbernice);
    Release(DMA);
    Seize(Procesor);
    Seize(Sbernice);
    Wait(Odpoved*8/V_TERMMIKR);   // pøenos odpovìdi na terminál
    Leave(Pamet,Odpoved);
    Release(Sbernice);
    Release(Procesor);
    TTerminal(Time - Prichod);    // záznam doby zpracování do tabulky
  }
 public: Terminal() { Activate(); }
};

class GenerTerminal : public Event {  // generátor úloh pro terminál
  void Behavior() {                   // popis chování generátoru
    new Terminal;
    Activate(Time+Exponential(T_TERM));
  }
 public: GenerTerminal() { Activate(); }
};


class HlPocitac : public Process {   // úlohy zpracovávané hlavním poèítaèem
  void Behavior() {
    Seize(DMA);
    Seize(Sbernice); //,1);
    Wait(ObsazKap*8/V_HLPOCMIKR);   // pøenos dat do hlavního poèítaèe
    Leave(Pamet,ObsazKap);
    ObsazKap = 0;
    Release(Sbernice);
    Release(DMA);
  }
 public: HlPocitac() { Activate(); }
};

class GenerHlPoc : public Event {     // generátor úloh pro hlavní poèítaè
  void Behavior() {                   // popis chování generátoru
    new HlPocitac;
    Activate(Time+Exponential(T_HLPOC));
  }
 public: GenerHlPoc() { Activate(); }
};


int main() {
  SetOutput("pocsit.out");       // nastavení vıstupního souboru
  Init(T_POC,T_KON);             // inicializace experimentu
  for (int k=0; k<P_POKL; k++)
    new GenerPokladna;           // inicializace generátorù úloh pro pokladny
  new GenerTerminal;             // inicializace generátoru úloh pro terminál
  new GenerHlPoc;                // inicializace generátoru úloh pro hl. poè.
  Print(" POCSIT --- model poèítaèové sítì\n");
  Run();                         // simulace
  Procesor.Output();             // tisk vısledkù
  Sbernice.Output();
  DMA.Output();
  Pamet.Output();
  TPokladna.Output();
  TTerminal.Output();
  SIMLIB_statistics.Output();     // print run statistics
}

//
