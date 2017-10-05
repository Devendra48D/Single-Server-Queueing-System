////////////////////////////////////////////////////////////////////////////
// Model pocsit                   SIMLIB/C++
//
// P��klad simula�n�ho programu - po��ta�ov� s� v obchodn�m domu
//

// �as simulace:
//                 113 s na 386/40 bez koprocesoru
//                  31 s na 386/40 s koprocesorem
//                  10 s na P90/Win95/DosPrompt
//

/////////////////////////////////////////////////////////////////////////////
//                                                                         //
// Zad�n�:                                                                 //
//  -------                                                                //
//    Obchodn� d�m  je vybaven po��ta�ovou s�t�,  kter� spojuje jednotliv� //
//  odd�len� a sklady a ��d� se pomoc� n� distribuce zbo��. Ka�d� prodejn� //
//  odd�len� obchodn�ho  domu je vybaveno mikropo��ta�em,  ke kter�mu jsou //
//  p�ipojeny  pokladny  dan�ho  odd�len�  a  termin�l vedouc�ho odd�len�, //
//  kter�  umo��uje  z�sk�vat  p�ehled  o  zbo��  a tr�b�. Mikropo��ta� je //
//  spojen  s  hlavn�m  po��ta�em,  kter�  v  ur�it�ch intervalech p�eb�r� //
//  informace o prodan�m zbo��.                                            //
//                                                                         //
//      Blokov� struktura mikropo��ta�e na odd�len� :                      //
//                                                                         //
// +----------------------------------------------------------------------�//
// |������������                                                          |//
// |� mikro-   ������������������������������������������������������     |//
// |� procesor �   �       �       �         �         �            �     |//
// |������������   �       �       �         �         �            �     |//
// |            ������� ������� �������  ��������� ���������    ��������� |//
// |            � RAM � � DMA � � PPI �  � USART � � USART � .. � USART � |//
// |            ������� ������� �������  ��������� ���������    ��������� |//
// |                       �       �          �        �            �     |//
// |                   ���������   �          �        �            �     |//
// |                   � USART �   �          �        �            �     |//
// |                   ���������   �          �        �            �     |//
// �-----------------------�-------�----------�--------�------------�-----�//
//                         �       �          �        �            �      //
//       hlavn� po��ta�  <��  ������������ �������� �������      �������   //
//                            � termin�l � �      � �     � ...  �     �   //
//                            ������������ �������� �������      �������   //
//                                                 POKLADNY                //
//                                                                         //
//  Pomoc�  modelu   prov��te,  zda  vyhovuje   n�sleduj�c�  konfigurace   //
// mikropo��ta�e na jednom odd�len� obchodn�ho domu :                      //
//                                                                         //
// - po�et pokladen : 3                                                    //
// - doba  mezi p��chody po�adavk�  na zaplacen� 1  ks zbo�� je  n�hodn�   //
//   veli�ina  s exponenci�ln�m  rozlo�en�m pravd�podobnosti  se st�edn�   //
//   hodnotou 10 s                                                         //
// - p�enosov� rychlost mezi pokladnou a mikropo��ta�em je 1200 b/s        //
// - velikost  p�en�en�ho bloku informac� p��slu�ej�c�ho  1 ks zbo�� je   //
//   100 byt�                                                              //
// - doba mezi ot�zkami zad�van�mi na termin�l je n�hodn� veli�ina s expo- //
//   nenci�ln�m rozlo�en�m pravd�podobnosti se st�edn� hodnotou 10 minut   //
// - p�enosov� rychlost mezi termin�lem a mikropo��ta�em je 1 byte/100 �s  //
// - velikost bloku p��slu�ej�c�ho ot�zce i odpov�di je 100 a� 1000 byt�   //
//   (rovnom�rn� rozlo�en�)                                                //
// - doba,  ne� hlavn� po��ta� p�evezme  ot�zku : 0 a�  10 s (rovnom�rn�   //
//   rozlo�en�)                                                            //
// - doba vypracov�n� odpov�di : 0.5 a� 15 s (rovnom�rn� rozlo�en�)        //
// - interval mezi p�enosy dat  do hlavn�ho po��ta�e je n�hodn� veli�ina   //
//   s exponenci�ln�m rozlo�en�m pravd�podobnosti  se st�edn� hodnotou 5   //
//   minut                                                                 //
// - p�enosov� rychlost mezi hlavn�m po��ta�em a mikropo��ta�em je 19200   //
//   bit� za sekundu                                                       //
// - kapacita pam�ti mikropo��ta�e je 24 kB                                //
//                                                                         //
/////////////////////////////////////////////////////////////////////////////

#include "simlib.h"

// deklarace konstant

#define T_POC  0.0         // doba simulace po��te�n� (v sek)
#define T_KON 10.0*3.6e3   //               kone�n�
#define P_POKL 3           // po�et pokladen
#define T_POKL 10.0        // st�edn� doba mezi p��chody po�adavk� na pokladnu
#define V_POKLMIKR  1.2e3  // p�enosov� rychlost pokladna - mikropo��ta� (b/s)
#define V_TERMMIKR  8.0e4  // p�enosov� rychlost termin�l - mikropo��ta� (b/s)
#define V_HLPOCMIKR 1.92e4 // p�enosov� rychlost hl.po��ta� - mikropo��ta� (b/s)
#define S_BLOKZBOZI  100   // velikost bloku p��slu�ej�c�ho 1 ks zbo��
#define Smin_DOTAZODPOV  100 // velikost bloku (minim.) pro ot�zku a odpov��
#define Smax_DOTAZODPOV 1000 //     -  ||  -   (maxim.)   -  ||  -
#define T_TERM 6e2           // doba mezi zad�n�m nov� �lohy na termin�l
#define Tmin_PREVDOTAZU 0.0  // doba p�evzet� dotazu hl. po��ta�em (minim.)
#define Tmax_PREVDOTAZU 10.0 //             -  ||  -               (maxim.)
#define Tmin_ODPOVEDI    0.5 // doba vypracov�n� odpov�di (minim.)
#define Tmax_ODPOVEDI   15.0 //      -  ||  -             (maxim.)
#define T_HLPOC 1.8e2        // st�edn� doba mezi p�enosy dat do hl. po��ta�e
#define KAP_PAMETI 24576     // byt�

// deklarace glob�ln�ch objekt�

Store Pamet("Pam�",KAP_PAMETI);
Facility Procesor("Procesor"),
	 Sbernice("Sb�rnice"),
	 DMA("DMA");
Histogram TPokladna ("Doba obsluhy po�adavk� z pokladny",0,1,10);
Histogram TTerminal ("Doba obsluhy po�adavk� z termin�lu",0,5,10);
unsigned ObsazKap = 0;  // kapacita pam�ti, ur�en� pro p�enos do hl. po��ta�e


class Pokladna : public Process {  // t��da �loh zpracov�van�ch pokladnou
  double Prichod;
  void Behavior() {
    Prichod = Time;      // doba p��chodu po�adavku na n�kterou z pokladen
    Seize(Procesor); //,1);   // vy��� priorita, aby pokladna dlouho ne�ekala
    Seize(Sbernice);
    Enter(Pamet,S_BLOKZBOZI);
    ObsazKap+=S_BLOKZBOZI;
    Wait(S_BLOKZBOZI*8/V_POKLMIKR);  // p�enos bloku do mikropo��ta�e
    Release(Sbernice);
    Release(Procesor);
    TPokladna(Time - Prichod);  // z�znam doby zpracov�n� do tabulky
  }
 public: Pokladna() { Activate(); }
};

class GenerPokladna : public Event {  // t��da gener�tor� �loh pro pokladny
  void Behavior() {                   // popis chov�n� gener�toru
    new Pokladna;
    Activate(Time+Exponential(T_POKL));
  }
 public: GenerPokladna() { Activate(); }
};


class Terminal : public Process { // t��da �loh zpracov�van�ch termin�lem
  double Prichod;                 // �as vzniku �lohy zpracov�van� termin�lem
  int Otazka, Odpoved;            // velikost ot�zky a odpov�di v bytech
  void Behavior() {
    Prichod = Time;
    Seize(Procesor);
    Seize(Sbernice);
    Otazka = int(Uniform(Smin_DOTAZODPOV,Smax_DOTAZODPOV));
    Enter(Pamet,Otazka);
    Wait(Otazka*8/V_TERMMIKR);   // p�enos ot�zky do mikropo��ta�e
    Release(Sbernice);
    Release(Procesor);
    Wait(Uniform(Tmin_PREVDOTAZU,Tmax_PREVDOTAZU));
				 // p�evzet� ot�zky hl. po��ta�em
    Seize(DMA);
    Seize(Sbernice,1);
    Wait(Otazka*8/V_HLPOCMIKR);  // p�enos ot�zky do hl. po��ta�e
    Leave(Pamet,Otazka);
    Release(Sbernice);
    Release(DMA);
    Wait(Uniform(Tmin_ODPOVEDI,Tmax_ODPOVEDI));  // vypracov�n� odpov�di
    Odpoved = int(Uniform(Smin_DOTAZODPOV,Smax_DOTAZODPOV));
    Seize(DMA);
    Seize(Sbernice,1);
    Enter(Pamet,Odpoved);
    Wait(Odpoved*8/V_HLPOCMIKR);  // p�enos odpov�di do mikropo��ta�e
    Release(Sbernice);
    Release(DMA);
    Seize(Procesor);
    Seize(Sbernice);
    Wait(Odpoved*8/V_TERMMIKR);   // p�enos odpov�di na termin�l
    Leave(Pamet,Odpoved);
    Release(Sbernice);
    Release(Procesor);
    TTerminal(Time - Prichod);    // z�znam doby zpracov�n� do tabulky
  }
 public: Terminal() { Activate(); }
};

class GenerTerminal : public Event {  // gener�tor �loh pro termin�l
  void Behavior() {                   // popis chov�n� gener�toru
    new Terminal;
    Activate(Time+Exponential(T_TERM));
  }
 public: GenerTerminal() { Activate(); }
};


class HlPocitac : public Process {   // �lohy zpracov�van� hlavn�m po��ta�em
  void Behavior() {
    Seize(DMA);
    Seize(Sbernice); //,1);
    Wait(ObsazKap*8/V_HLPOCMIKR);   // p�enos dat do hlavn�ho po��ta�e
    Leave(Pamet,ObsazKap);
    ObsazKap = 0;
    Release(Sbernice);
    Release(DMA);
  }
 public: HlPocitac() { Activate(); }
};

class GenerHlPoc : public Event {     // gener�tor �loh pro hlavn� po��ta�
  void Behavior() {                   // popis chov�n� gener�toru
    new HlPocitac;
    Activate(Time+Exponential(T_HLPOC));
  }
 public: GenerHlPoc() { Activate(); }
};


int main() {
  SetOutput("pocsit.out");       // nastaven� v�stupn�ho souboru
  Init(T_POC,T_KON);             // inicializace experimentu
  for (int k=0; k<P_POKL; k++)
    new GenerPokladna;           // inicializace gener�tor� �loh pro pokladny
  new GenerTerminal;             // inicializace gener�toru �loh pro termin�l
  new GenerHlPoc;                // inicializace gener�toru �loh pro hl. po�.
  Print(" POCSIT --- model po��ta�ov� s�t�\n");
  Run();                         // simulace
  Procesor.Output();             // tisk v�sledk�
  Sbernice.Output();
  DMA.Output();
  Pamet.Output();
  TPokladna.Output();
  TTerminal.Output();
  SIMLIB_statistics.Output();     // print run statistics
}

//
