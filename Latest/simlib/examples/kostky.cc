////////////////////////////////////////////////////////////////////////////
// Model kostky               SIMLIB/C++
//
// Example of queueing system
//
// Pøíklad simulaèního programu - prodejna s Rubikovými kostkami (verze 1)
//
// Do hraèkáøství pøivezli Rubikovy kostky: ráno v 8 hodin 500 kusù
// a odpoledne  v 15  hodin opìt  500 kusù.  V hraèkáøství se otevírá
// v 8 hodin ráno a zavírá se  v 18 hodin veèer. Zákazníci pøicházeli
// od  8  hodin  prùmìrnì  po  3/4 minuty (s  exponenciálním
// rozlo¾ením) a  ka¾dý zákazník  koupil jen jednu kostku. Kdy¾
// u¾ byly  v¹echny kostky  vyprodány, zákazníci  se  nevzdali, stáli
// zpùsobnì ve frontì  a èekali, a¾  pøivezou dal¹ích  500 kusù odpo-
// ledne v 15 hodin. Mezitím  ov¹em pøicházeli noví zákazníci. Urèete
// max. délku fronty  a prùmìrnou  dobu èekání, spoèítejte, kolik zá-
// kazníkù  ode¹lo  s nepoøízenou, proto¾e vedoucí prodejnu  striktnì
// zavøela  v 18 hodin  a nebrala  ohled  na lidi, kteøí  u¾ stáli ve
// frontì. Po 18.hodinì u¾ dal¹í zákazníci nepøicházeli. Doba obsluhy
// je 1 minuta.
//
// Zdroj: domácí úkoly studentù

#include "simlib.h"

// time of simulation:  T_POC - start, T_KON - stop
const double T_POC = 8.0;   // hodin
const double T_KON = 18.0;  // hodin

// definice konstant
const double T_OBSLUHY  = 1.0/60;   // doba obsluhy zákazníka - 1 minuta
const double T_DODAVKY1 = T_POC;    // èas dodávky kostek - ráno
const double T_DODAVKY2 = 15.0;     // èas dodávky kostek - odpoledne
const double T_PRICH    = 0.75/60;  // prùmìrný interval pøíchodu zákazníkù 3/4 min

// deklarace globalnich objektu
Facility  Prodejna;           // prodejna kostek
Histogram Tabulka ("Èekací doby zákazníkù",0,0.15,20);
                              // histogram pro záznam èekacích dob zákazníkù
unsigned Kostek = 0;          // poèet kostek v prodejnì
unsigned PocZakazniku = 0;    // celkový poèet neobslou¾ených zákazníkù


class Zakaznik : public Process {  // tøída zákazníkù
  double Prichod;               // èas pøíchodu ka¾dého zákazníka
  void Behavior() {             // popis chování zákazníka
    Prichod = Time;
    PocZakazniku++;
    Seize(Prodejna);
    WaitUntil(Kostek > 0);      // èekej, jestli¾e kostky nejsou
    Kostek--;
    Wait(T_OBSLUHY);
    Release(Prodejna);
    PocZakazniku--;
    Tabulka(Time - Prichod);    // záznam doby èekání do tabulky
  }
};

class Generator : public Event {        // generátor zákazníkù
  void Behavior() {                     // popis chování generátoru
    (new Zakaznik)->Activate();         // nový zákazník
    Activate(Time+Exponential(T_PRICH));
  }
};

class Dodavka : public Event {          // supply
  void Behavior() {
     Kostek = Kostek + 500;
// corrected:  Terminate();
  }
};

int main() {
    SetOutput("kostky.out");    // output redirection
    Prodejna.SetName("Prodejna");
    Init(T_POC, T_KON);         // initialize, non-zero initial time
    Tabulka.Clear();            // we start at non-zero time
    Prodejna.Clear();
    (new Dodavka)->Activate(T_DODAVKY1);        // morning supply
    (new Dodavka)->Activate(T_DODAVKY2);        // afternoon
    (new Generator)->Activate();        // incoming customers generator
    Print(" KOSTKY --- simulace obchodu\n");
    Run();                      // simulation
    Print("\n S nepoøízenou ode¹lo %u zákazníkù \n\n", PocZakazniku);
    Prodejna.Output();          // print results
    Tabulka.Output();
    SIMLIB_statistics.Output(); // print run statistics
}

//
