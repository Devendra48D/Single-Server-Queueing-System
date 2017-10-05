////////////////////////////////////////////////////////////////////////////
// Model centrala.cc             SIMLIB/C++
//
// Pøíklad simulaèního programu - hovory pøes centrálu
//
// Velký podnik má vnitøní telefonní sí» se 200 klapkami, spojenou pøes
// centrálu s 6 telefony s veøejnou sití. Pøedpokládejme, ¾e po¾adavky
// spojení pøicházejí z veøejné sití prùmìrnì po 2 minutách (s exponen-
// ciálním rozlo¾ením). V pøípadì, ¾e klapka není volná s pravdìpodob-
// ností 90% si volající poèká ne¾ se uvolní. Hovor trvá prùmìrnì 7minut.
// Souèasnì se uskuteèòují vnitøní hovory prùmìrnì po 30 sekund a trvají
// prùmìrnì 3 minuty. Pøibli¾nì po 10 minutách pøijde po¾adavek na hovor
// zevnitø ven, který trvá prùmìrnì 10 minut. Zjistìte prùmìrné zatí¾ení
// centrály a prùmìrné doby èekání na spojení vnìj¹ích
// a vnitøních hovorù.
//
// Zdroj: domácí úkoly
//

#include "simlib.h"

////////////////////////////////////////////////////////////////////////////
// èasové konstanty  (za jednotku èasu pova¾ujeme 1 minutu)
const double DOBA_SIMULACE       = 8*60;// 8 hodin

// støední doba mezi pøíchody:
const double PRICHOD_VNEJSICH    = 2;   // vnìj¹ích po¾adavkù
const double PRICHOD_VNITRNICH   = 0.5; // vnitøních po¾adavkù
const double PRICHOD_ZEVNITR_VEN = 10;  // po¾adavkù zevnitø ven

// doby hovoru v jednotlivých pøípadech:
const double HOVOR_VNEJSI        = 7;
const double HOVOR_VNITRNI       = 3;
const double HOVOR_ZEVNITR_VEN   = 10;

////////////////////////////////////////////////////////////////////////////
// deklarace globálních objektù
const int N_TEL = 200;          // poèet telefonù
Store Centrala("Centrála",6);   // centrála - kapacita 6 spojení najednou
Facility Telefon[N_TEL];        // telefony

Histogram Tabulka1("Èekaní na spojení: vnitøní po¾adavky",0,0.1,20);
Histogram Tabulka2("Èekaní na spojení: vnìj¹í po¾adavky",0,0.1,20);

// náhodný výbìr telefonu (rovnomìrné rozdìlení) 
int RandomTel() {
    return int(N_TEL*Random()); //  0 .. N_TEL-1
}

////////////////////////////////////////////////////////////////////////////
// tøídy modelující hovory
//
class Hovor : public Process {
 protected:
   double prichod;             // doba vstupu po¾adavku do vnitøní sítì
   int odkud, kam;             // èísla telefonù (-1 == venku)
 public:
   Hovor() : prichod(0), odkud(-1), kam(-1) { Activate(); }
};

class HovorZvenku : public Hovor { // vnìj¹í po¾adavky
   void Behavior() {
      kam = RandomTel();        // komu se volá
      Enter(Centrala,1);        // obsadí jeden z telefonù centrály
      prichod = Time;           // po¾adavek vstoupil do vnitøní sítì
      if (!Telefon[kam].Busy() || Random()<0.9) {
	 // pøípad, ¾e volaný telefon je volný, anebo obsazený ale
	 // volající je ochoten èekat (p=90%).
	 Seize(Telefon[kam]);      // obsazení nebo èekání
	 Tabulka2(Time-prichod);   // záznam doby èekání do tabulky
	 Wait(Exponential(HOVOR_VNEJSI)); // probíhá hovor
	 Release(Telefon[kam]);    // zavì¹ení telefonu = uvolnìní linky
      }
      Leave(Centrala,1);           // uvolnìní jednoho telefonu centrály
   } //Behavior
 public:
   static void Create() { new HovorZvenku; }
};

class VnitrniHovor : public Hovor { // hovory uvnitø podniku
   double prichod;              // doba pøíchodu po¾adavku
   void Behavior() {
      // náhodnì vybere volající telefon - 'odkud' (musí být volný)
      do odkud=RandomTel(); while(Telefon[odkud].Busy());
      prichod=Time;
      Seize(Telefon[odkud]);    // zvedne telefon (obsadí vnitøní linku)
      // náhodnì vybere volané èíslo telefonu - kam (podmínka: kam!=odkud)
      do kam=RandomTel(); while(kam==odkud);
      // vytoèí èíslo a èeká...
      Seize(Telefon[kam]);      // volaný zvedá telefon
      Tabulka1(Time-prichod);   // záznam doby èekání
      Wait(Exponential(HOVOR_VNITRNI)); // probíhá hovor
      Release(Telefon[kam]);    // hovor skonèil = uvolnìní linek
      Release(Telefon[odkud]);
   } //Behavior
 public:
   static void Create() { new VnitrniHovor; }
};

class HovorZevnitrVen : public Hovor { // po¾adavky na hovory ven
   void Behavior() {
      // náhodnì vybere volající telefon - 'odkud' (musí být volný)
      do odkud=RandomTel(); while(Telefon[odkud].Busy());
      Seize(Telefon[odkud]);    // zvedne telefon (obsadí linku)
      Enter(Centrala,1);        // obsadí jeden z telefonù centrály
      Wait(Exponential(HOVOR_ZEVNITR_VEN));  // probíhá hovor
      Leave(Centrala,1);        // uvolnìní telefonu centrály
      Release(Telefon[odkud]);  // zavìsí telefon = uvolnìní linky
   } //Behavior
 public:
   static void Create() { new HovorZevnitrVen; }
};

/////////////////////////////////////////////////////////////////////////////
// generátor po¾adavkù
//
typedef void (*CreatePtr_t)();         // typ ukazatel na statickou metodu
class Generator : public Event {       // generátor vnìj¹ích po¾adavkù
   CreatePtr_t create;  // ukazatel na metodu Create()
   double dt;           // interval mezi vytvoøením po¾adavkù
   void Behavior() {
      create();                        // generování po¾adavku
      Activate(Time+Exponential(dt));  // dal¹í po¾adavek
   }
 public:
   Generator(CreatePtr_t p, double _dt) : create(p), dt(_dt) {
     Activate();
   }
};

/////////////////////////////////////////////////////////////////////////////
// Experiment
//
int main() {
   SetOutput("centrala.out");
   Print("Model telefonování pøes centrálu\n");
   Init(0,DOBA_SIMULACE);                // inicializace experimentu
   // aktivace generátorù po¾adavkù
   new Generator(HovorZvenku::Create, PRICHOD_VNEJSICH);
   new Generator(VnitrniHovor::Create, PRICHOD_VNITRNICH);
   new Generator(HovorZevnitrVen::Create, PRICHOD_ZEVNITR_VEN);
   // simulace
   Run();
   // tisk výsledkù
   Centrala.Output();
   Tabulka1.Output();
   Tabulka2.Output();
   SIMLIB_statistics.Output(); // print run statistics
}

//
