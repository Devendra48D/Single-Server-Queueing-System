////////////////////////////////////////////////////////////////////////////
// Model kostky2              SIMLIB/C++
//
// P��klad simula�n�ho programu - prodejna s Rubikov�mi kostkami (verze 2)
//
// Do hra�k��stv� p�ivezli Rubikovy kostky: r�no v 8 hodin 500 kus�
// a odpoledne  v 15  hodin op�t  500 kus�.  V hra�k��stv� se otev�r�
// v 8 hodin r�no a zav�r� se  v 18 hodin ve�er. Z�kazn�ci p�ich�zeli
// od  8  hodin  pr�m�rn�  po  3/4 minuty (s  exponenci�ln�m
// rozlo�en�m) a  ka�d� z�kazn�k  koupil jen jednu kostku. Kdy�
// u� byly  v�echny kostky  vyprod�ny, z�kazn�ci  se  nevzdali, st�li
// zp�sobn� ve front�  a �ekali, a�  p�ivezou dal��ch  500 kus� odpo-
// ledne v 15 hodin. Mezit�m  ov�em p�ich�zeli nov� z�kazn�ci. Ur�ete
// max. d�lku fronty  a pr�m�rnou  dobu �ek�n�, spo��tejte, kolik z�-
// kazn�k�  ode�lo  s nepo��zenou, proto�e vedouc� prodejnu  striktn�
// zav�ela  v 18 hodin  a nebrala  ohled  na lidi, kte��  u� st�li ve
// front�. Po 18.hodin� u� dal�� z�kazn�ci nep�ich�zeli. Doba obsluhy
// je 1 minuta.
//

//#define SIMLIB_DEBUG 1
#include "simlib.h"

// doba simulace :  T_POC - po��te�n�, T_KON - kone�n�
const double T_POC =  8.0;  // hodin
const double T_KON = 18.0;  // hodin

// konstanty:
const double T_OBSLUHY  = 1.0/60;    // doba obsluhy z�kazn�ka - 1 minuta
const double T_DODAVKY1 = T_POC;     // �as dod�vky kostek - r�no
const double T_DODAVKY2 = 15.0;      // �as dod�vky kostek - odpoledne
const double T_PRICH    = 0.75/60;   // pr�m�rn� interval p��chodu z�kazn�k� 3/4 min

// histogram pro z�znam �ekac�ch dob z�kazn�k�
Histogram Tabulka ("�ekac� doby z�kazn�k�",0,0.15,20);

unsigned PocZakazniku = 0;   // celkov� po�et neobslou�en�ch z�kazn�k�

Queue F("Fronta z�kazn�k�");

class Zakaznik : public Process {  // t��da z�kazn�k�
  double Prichod;                  // �as p��chodu z�kazn�ka
  void Behavior() {                // popis chov�n�
    Prichod = Time;
    PocZakazniku++;
    Into(F);                       // n�kup kdy� jsou, jinak �ek�n�
    Passivate();
    // obsluha v obchode
    PocZakazniku--;
    Tabulka(Time - Prichod);       // z�znam doby �ek�n� do tabulky
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


class Generator : public Event {        // gener�tor z�kazn�k�
  void Behavior() {                     // popis chov�n� gener�toru
    (new Zakaznik)->Activate();         // nov� z�kazn�k
    Activate(Time+Exponential(T_PRICH));
  }
};

class Dodavka : public Event {          // dod�vka kostek
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
  SetOutput("kostky2.out");          // nastaven� v�stupn�ho souboru
  Init(T_POC,T_KON);                 // inicializace experimentu
  Tabulka.Clear(); // we start at non-zero time
  F.Clear();
  Obchod *o = new Obchod;
  new Dodavka(o,T_DODAVKY1);         // inicializace dod�vky zbo�� r�no
  new Dodavka(o,T_DODAVKY2);         // inicializace dod�vky zbo�� odpoledne
  (new Generator)->Activate();       // inicializace gener�toru z�kazn�k�
  Print(" KOSTKY2 --- simulace obchodu\n");
  Run();                             // simulace
  Print("\n S nepo��zenou ode�lo %u z�kazn�k� \n\n", PocZakazniku);
  Tabulka.Output();
  F.Output();
  SIMLIB_statistics.Output();     // print run statistics
}

//
