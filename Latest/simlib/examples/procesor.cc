////////////////////////////////////////////////////////////////////////////
// Model procesor                 SIMLIB/C++
//
// Testovací model (viz. skripta)
//

#include "simlib.h"

#define minut         * (60 * 1.0e3)
#define minuty        minut
#define sekund        * 1.0e3
#define sekundy       sekund
#define milisekund    * 1.0

// doba simulace
const double TSIM     = 30 minut ;

// poèty objektù
const int N_CPU       = 3;
const int N_DISK      = 2;
const int N_CHANNEL   = 1;

const double ARRTM    = 9 sekund ;
const double PROCTM   = 6 sekund ;
const double T_SEARCH = 80 milisekund ;
const double T_TR     = 30 milisekund ;  

// pomocná funkce:
int Uniform(int min, int max) {           // bez max
  return int( Uniform(double(min), double(max/*+1*/)) );
}
// deklarace globálních objektù
Facility  cpu[N_CPU];
Queue     q_cpu("q_cpu");

Facility  disk[N_DISK];

Facility  channel[N_CHANNEL];
Queue     q_channel("q_channel");

Store     memory("Pamì»",128);

Histogram table("Doba zpracování", 0, 2 sekundy, 25);

class Job : public Process {  // tøída po¾adavkù
  double t0;
  double tcpu;
  double tio;
  double deltaT;
  int mem;
  int recs;
  int d;
  void Behavior() {
    t0 = Time;
    Enter(memory,mem);
    while(tcpu>0)
    {
       int i,j;
       //do {
	   for(i=0; i<N_CPU; i++) 
	      if(!cpu[i].Busy()) { 
                  Seize(cpu[i]); 
                  break; 
              }
	   if(i==N_CPU) { 
              Into(q_cpu); 
              Passivate();  // wait in queue
              //Out(); 
           }
       //}while(i==N_CPU);
       deltaT = Exponential(tio);
       Wait(deltaT);  // in cpu
       tcpu -= deltaT;
       for(i=0; i<N_CPU; i++)
         if(cpu[i].in==this) Release(cpu[i]);
       d = Uniform(0,N_DISK);
       Seize(disk[d]);
       Wait(Uniform(0.0,T_SEARCH));  // search
       //do {
           for(j=0; j<N_CHANNEL; j++) 
               if(!channel[j].Busy()) { Seize(channel[j]); break; } 
	   if(j==N_CHANNEL) { 
              Into(q_channel); 
              Passivate();  // wait in queue
              //Out(); 
           }
       //}while(j==N_CHANNEL);
       Wait(T_TR/10 + Uniform(0.0,T_TR));  // transfer
       for(j=0; j<N_CHANNEL; j++)
         if(channel[j].in==this) Release(channel[j]);
       Release(disk[d]);
    }
    Leave(memory,mem);
    table(Time-t0);
  }
public:
  Job() {
    tcpu = Exponential(PROCTM);
    mem  = Uniform(20,61);
    recs = (tcpu/1000 * Uniform(2.0,10.0)) + 1;
    tio  = tcpu/recs;
    Activate();
  }
};

class Generator : public Event {  // generátor po¾adavkù
  void Behavior() {
    new Job;
    Activate(Time+Exponential(ARRTM));
  }
 public:
  Generator() { Activate(); }
};

int main() {
  //DebugON();
  SetOutput("procesor.out");
  Print("PROCESOR --- model poèítaèového systému\n");
  // nastavení spoleèných front
  int i;
  for(i=0; i<N_CPU; i++)      cpu[i].SetQueue(q_cpu);
  for(i=0; i<N_CHANNEL; i++)  channel[i].SetQueue(q_channel);
  Init(0,TSIM);
  new Generator;
  Run();
  table.Output();
  SIMLIB_statistics.Output();     // print run statistics
}

//
