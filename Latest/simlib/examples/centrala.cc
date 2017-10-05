////////////////////////////////////////////////////////////////////////////
// Model centrala.cc             SIMLIB/C++
//
// P��klad simula�n�ho programu - hovory p�es centr�lu
//
// Velk� podnik m� vnit�n� telefonn� s� se 200 klapkami, spojenou p�es
// centr�lu s 6 telefony s ve�ejnou sit�. P�edpokl�dejme, �e po�adavky
// spojen� p�ich�zej� z ve�ejn� sit� pr�m�rn� po 2 minut�ch (s exponen-
// ci�ln�m rozlo�en�m). V p��pad�, �e klapka nen� voln� s pravd�podob-
// nost� 90% si volaj�c� po�k� ne� se uvoln�. Hovor trv� pr�m�rn� 7minut.
// Sou�asn� se uskute��uj� vnit�n� hovory pr�m�rn� po 30 sekund a trvaj�
// pr�m�rn� 3 minuty. P�ibli�n� po 10 minut�ch p�ijde po�adavek na hovor
// zevnit� ven, kter� trv� pr�m�rn� 10 minut. Zjist�te pr�m�rn� zat�en�
// centr�ly a pr�m�rn� doby �ek�n� na spojen� vn�j��ch
// a vnit�n�ch hovor�.
//
// Zdroj: dom�c� �koly
//

#include "simlib.h"

////////////////////////////////////////////////////////////////////////////
// �asov� konstanty  (za jednotku �asu pova�ujeme 1 minutu)
const double DOBA_SIMULACE       = 8*60;// 8 hodin

// st�edn� doba mezi p��chody:
const double PRICHOD_VNEJSICH    = 2;   // vn�j��ch po�adavk�
const double PRICHOD_VNITRNICH   = 0.5; // vnit�n�ch po�adavk�
const double PRICHOD_ZEVNITR_VEN = 10;  // po�adavk� zevnit� ven

// doby hovoru v jednotliv�ch p��padech:
const double HOVOR_VNEJSI        = 7;
const double HOVOR_VNITRNI       = 3;
const double HOVOR_ZEVNITR_VEN   = 10;

////////////////////////////////////////////////////////////////////////////
// deklarace glob�ln�ch objekt�
const int N_TEL = 200;          // po�et telefon�
Store Centrala("Centr�la",6);   // centr�la - kapacita 6 spojen� najednou
Facility Telefon[N_TEL];        // telefony

Histogram Tabulka1("�ekan� na spojen�: vnit�n� po�adavky",0,0.1,20);
Histogram Tabulka2("�ekan� na spojen�: vn�j�� po�adavky",0,0.1,20);

// n�hodn� v�b�r telefonu (rovnom�rn� rozd�len�) 
int RandomTel() {
    return int(N_TEL*Random()); //  0 .. N_TEL-1
}

////////////////////////////////////////////////////////////////////////////
// t��dy modeluj�c� hovory
//
class Hovor : public Process {
 protected:
   double prichod;             // doba vstupu po�adavku do vnit�n� s�t�
   int odkud, kam;             // ��sla telefon� (-1 == venku)
 public:
   Hovor() : prichod(0), odkud(-1), kam(-1) { Activate(); }
};

class HovorZvenku : public Hovor { // vn�j�� po�adavky
   void Behavior() {
      kam = RandomTel();        // komu se vol�
      Enter(Centrala,1);        // obsad� jeden z telefon� centr�ly
      prichod = Time;           // po�adavek vstoupil do vnit�n� s�t�
      if (!Telefon[kam].Busy() || Random()<0.9) {
	 // p��pad, �e volan� telefon je voln�, anebo obsazen� ale
	 // volaj�c� je ochoten �ekat (p=90%).
	 Seize(Telefon[kam]);      // obsazen� nebo �ek�n�
	 Tabulka2(Time-prichod);   // z�znam doby �ek�n� do tabulky
	 Wait(Exponential(HOVOR_VNEJSI)); // prob�h� hovor
	 Release(Telefon[kam]);    // zav�en� telefonu = uvoln�n� linky
      }
      Leave(Centrala,1);           // uvoln�n� jednoho telefonu centr�ly
   } //Behavior
 public:
   static void Create() { new HovorZvenku; }
};

class VnitrniHovor : public Hovor { // hovory uvnit� podniku
   double prichod;              // doba p��chodu po�adavku
   void Behavior() {
      // n�hodn� vybere volaj�c� telefon - 'odkud' (mus� b�t voln�)
      do odkud=RandomTel(); while(Telefon[odkud].Busy());
      prichod=Time;
      Seize(Telefon[odkud]);    // zvedne telefon (obsad� vnit�n� linku)
      // n�hodn� vybere volan� ��slo telefonu - kam (podm�nka: kam!=odkud)
      do kam=RandomTel(); while(kam==odkud);
      // vyto�� ��slo a �ek�...
      Seize(Telefon[kam]);      // volan� zved� telefon
      Tabulka1(Time-prichod);   // z�znam doby �ek�n�
      Wait(Exponential(HOVOR_VNITRNI)); // prob�h� hovor
      Release(Telefon[kam]);    // hovor skon�il = uvoln�n� linek
      Release(Telefon[odkud]);
   } //Behavior
 public:
   static void Create() { new VnitrniHovor; }
};

class HovorZevnitrVen : public Hovor { // po�adavky na hovory ven
   void Behavior() {
      // n�hodn� vybere volaj�c� telefon - 'odkud' (mus� b�t voln�)
      do odkud=RandomTel(); while(Telefon[odkud].Busy());
      Seize(Telefon[odkud]);    // zvedne telefon (obsad� linku)
      Enter(Centrala,1);        // obsad� jeden z telefon� centr�ly
      Wait(Exponential(HOVOR_ZEVNITR_VEN));  // prob�h� hovor
      Leave(Centrala,1);        // uvoln�n� telefonu centr�ly
      Release(Telefon[odkud]);  // zav�s� telefon = uvoln�n� linky
   } //Behavior
 public:
   static void Create() { new HovorZevnitrVen; }
};

/////////////////////////////////////////////////////////////////////////////
// gener�tor po�adavk�
//
typedef void (*CreatePtr_t)();         // typ ukazatel na statickou metodu
class Generator : public Event {       // gener�tor vn�j��ch po�adavk�
   CreatePtr_t create;  // ukazatel na metodu Create()
   double dt;           // interval mezi vytvo�en�m po�adavk�
   void Behavior() {
      create();                        // generov�n� po�adavku
      Activate(Time+Exponential(dt));  // dal�� po�adavek
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
   Print("Model telefonov�n� p�es centr�lu\n");
   Init(0,DOBA_SIMULACE);                // inicializace experimentu
   // aktivace gener�tor� po�adavk�
   new Generator(HovorZvenku::Create, PRICHOD_VNEJSICH);
   new Generator(VnitrniHovor::Create, PRICHOD_VNITRNICH);
   new Generator(HovorZevnitrVen::Create, PRICHOD_ZEVNITR_VEN);
   // simulace
   Run();
   // tisk v�sledk�
   Centrala.Output();
   Tabulka1.Output();
   Tabulka2.Output();
   SIMLIB_statistics.Output(); // print run statistics
}

//
