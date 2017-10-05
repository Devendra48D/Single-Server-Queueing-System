////////////////////////////////////////////////////////////////////////////
// model2-timeout.cc              SIMLIB/C++
//
// Queueing system M/D/1 with impatient transactions
//

#include "simlib.h"

const int TIMEOUT = 20;
const int SERVICE = 10;         // constant service time
long ncount = 0;                // number of impatient transactions

// model objects
Facility  Box("Box");
Histogram Table("Table", 0, 1, SERVICE+TIMEOUT+1);

// timing class
class Timeout : public Event {
    Process *ptr;               // which process
  public:
    Timeout(double t, Process *p): ptr(p) {
      Activate(Time+t);         // when to time-out the process
    }
    void Behavior() {
      //ptr->Out();             // take out of queue (automatic in destructor) 
      //ptr->Terminate();       // kill process (is default in delete)
      delete ptr;               // kill process
      ncount++;                 // counter of time-out operations
      //Cancel();               // end periodic event (old SIMLIB) -- no more needed
    }
};

// transaction beahavior
class Customer : public Process {
  double ArrivalTime;           // atribute
  void Behavior() {             // --- transaction behavior description ---
    ArrivalTime = Time;         // mark time of arrival
    Event *timeout = new Timeout(TIMEOUT,this); // set timeout
    Seize(Box);                 // seize facility Box or wait in queue
    delete timeout;             // cancel timeout
    Wait(SERVICE);              // service time
    Release(Box);               // release facility Box
    Table(Time-ArrivalTime);    // record time spent in system
  }
};

// generator of customers - periodical event
class Generator : public Event {
  void Behavior() {               // --- behavior description ---
    (new Customer)->Activate();   // create new customer, activate now
    Activate(Time+Exponential(1e3/150)); // inter-arrival interval
  }
};

// experiment description
int main() {
  //  DebugON();                // for debugging only
  SetOutput("model2-timeout.out");
  Print("model2-timeout -- SIMLIB/C++ example\n");
  Init(0,1000);                 // initalize experiment, time 0..1000
  (new Generator)->Activate();  // create/activate customer generator
  Run();                        // run simulation experiment
  Print("Number of impatient customers: %d \n", ncount);
  Box.Output();                 // print statistics
  Table.Output();
  SIMLIB_statistics.Output();   // print run statistics
}

//
