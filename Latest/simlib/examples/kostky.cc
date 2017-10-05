////////////////////////////////////////////////////////////////////////////
// Model kostky               SIMLIB/C++
//
// Example of queueing system
//
// P��klad simula�n�ho programu - prodejna s Rubikov�mi kostkami (verze 1)
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
// Zdroj: dom�c� �koly student�

#include "simlib.h"

// time of simulation:  T_POC - start, T_KON - stop
const double T_POC = 8.0;   // hodin
const double T_KON = 18.0;  // hodin

// definice konstant
const double T_OBSLUHY  = 1.0/60;   // doba obsluhy z�kazn�ka - 1 minuta
const double T_DODAVKY1 = T_POC;    // �as dod�vky kostek - r�no
const double T_DODAVKY2 = 15.0;     // �as dod�vky kostek - odpoledne
const double T_PRICH    = 0.75/60;  // pr�m�rn� interval p��chodu z�kazn�k� 3/4 min

// deklarace globalnich objektu
Facility  Prodejna;           // prodejna kostek
Histogram Tabulka ("�ekac� doby z�kazn�k�",0,0.15,20);
                              // histogram pro z�znam �ekac�ch dob z�kazn�k�
unsigned Kostek = 0;          // po�et kostek v prodejn�
unsigned PocZakazniku = 0;    // celkov� po�et neobslou�en�ch z�kazn�k�


class Zakaznik : public Process {  // t��da z�kazn�k�
  double Prichod;               // �as p��chodu ka�d�ho z�kazn�ka
  void Behavior() {             // popis chov�n� z�kazn�ka
    Prichod = Time;
    PocZakazniku++;
    Seize(Prodejna);
    WaitUntil(Kostek > 0);      // �ekej, jestli�e kostky nejsou
    Kostek--;
    Wait(T_OBSLUHY);
    Release(Prodejna);
    PocZakazniku--;
    Tabulka(Time - Prichod);    // z�znam doby �ek�n� do tabulky
  }
};

class Generator : public Event {        // gener�tor z�kazn�k�
  void Behavior() {                     // popis chov�n� gener�toru
    (new Zakaznik)->Activate();         // nov� z�kazn�k
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
    Print("\n S nepo��zenou ode�lo %u z�kazn�k� \n\n", PocZakazniku);
    Prodejna.Output();          // print results
    Tabulka.Output();
    SIMLIB_statistics.Output(); // print run statistics
}

//
