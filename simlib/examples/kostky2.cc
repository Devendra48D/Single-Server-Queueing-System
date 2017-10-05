////////////////////////////////////////////////////////////////////////////
// Model kostky2              SIMLIB/C++
//
// Pøíklad simulaèního programu - prodejna s Rubikovými kostkami (verze 2)
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

//#define SIMLIB_DEBUG 1
#include "simlib.h"

// doba simulace :  T_POC - poèáteèní, T_KON - koneèná
const double T_POC =  8.0;  // hodin
const double T_KON = 18.0;  // hodin

// konstanty:
const double T_OBSLUHY  = 1.0/60;    // doba obsluhy zákazníka - 1 minuta
const double T_DODAVKY1 = T_POC;     // èas dodávky kostek - ráno
const double T_DODAVKY2 = 15.0;      // èas dodávky kostek - odpoledne
const double T_PRICH    = 0.75/60;   // prùmìrný interval pøíchodu zákazníkù 3/4 min

// histogram pro záznam èekacích dob zákazníkù
Histogram Tabulka ("Èekací doby zákazníkù",0,0.15,20);

unsigned PocZakazniku = 0;   // celkový poèet neobslou¾ených zákazníkù

Queue F("Fronta zákazníkù");

class Zakaznik : public Process {  // tøída zákazníkù
  double Prichod;                  // èas pøíchodu zákazníka
  void Behavior() {                // popis chování
    Prichod = Time;
    PocZakazniku++;
    Into(F);                       // nákup kdy¾ jsou, jinak èekání
    Passivate();
    // obsluha v obchode
    PocZakazniku--;
    Tabulka(Time - Prichod);       // záznam doby èekání do tabulky
  }
};

class Obchod : public Process {
  int Sklad;
 public:
  Obchod() { Sklad = 0; Activate(); }
  void Behavior() {
    for(;;)
    {
      if(Sklad==0) Passivate();
      WaitUntil(!F.Empty());
      Zakaznik *z = (Zakaznik *)F.GetFirst();
      Wait(T_OBSLUHY);
      z->Activate();
      Sklad--;
    }
  }
  void Dodavka(int ks) {
    Sklad += ks;
    if(Sklad>0) Activate();
  }
  ~Obchod() {    
    Print("Ve skladu zbylo %d kostek\n", Sklad);
  }
};


class Generator : public Event {        // generátor zákazníkù
  void Behavior() {                     // popis chování generátoru
    (new Zakaznik)->Activate();         // nový zákazník
    Activate(Time+Exponential(T_PRICH));
  }
};

class Dodavka : public Event {          // dodávka kostek
  Obchod *Kam;
  void Behavior() {
     Kam->Dodavka(500);
     //Terminate(); no more needed - was SIMLIB BUG
  }
 public:
  Dodavka(Obchod *kam, double kdy) {
    Kam=kam;
    Activate(kdy);
  }
};

////////////////////////////////////////////////////////////////////////////
// experiment
//
int main() {
  SetOutput("kostky2.out");          // nastavení výstupního souboru
  Init(T_POC,T_KON);                 // inicializace experimentu
  Tabulka.Clear(); // we start at non-zero time
  F.Clear();
  Obchod *o = new Obchod;
  new Dodavka(o,T_DODAVKY1);         // inicializace dodávky zbo¾í ráno
  new Dodavka(o,T_DODAVKY2);         // inicializace dodávky zbo¾í odpoledne
  (new Generator)->Activate();       // inicializace generátoru zákazníkù
  Print(" KOSTKY2 --- simulace obchodu\n");
  Run();                             // simulace
  Print("\n S nepoøízenou ode¹lo %u zákazníkù \n\n", PocZakazniku);
  Tabulka.Output();
  F.Output();
  SIMLIB_statistics.Output();     // print run statistics
}

//
