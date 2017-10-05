////////////////////////////////////////////////////////////////////////////
// barrier-test2.cc            SIMLIB/C++
//
// Basic class Barrier tests
//

#include "simlib.h"

Barrier   B("Barrier", 4);
Histogram Table("Total time spent in system", 0, 10, 15);

class Customer: public Process {
    double Prichod;             // atribute
    void Behavior() {           // --- example behavior ---
        Prichod = Time;
        B.Enter(this);          // create group of customers
        Print("Time %g --- %s\n", Time, Name());
        Wait(10);               // service time
        Table(Time - Prichod);  // total time spent in system
    }
};

class Generator: public Event {
    void Behavior() {
        (new Customer)->Activate();     // create new customer
        if (Time > 300)
            B.ChangeHeight(8); // rise barrier
        Activate(Time + Exponential(1e3 / 150));        // poisson process
    }
};

int main()
{                               // experiment control
    Print(" BARRIER  test \n");
    // DebugON();
    Init(0, 1000);              // initialize
    (new Generator)->Activate();        // create generator
    Run();                      // simulate
    // print results:
    B.Output();
    Table.Output();
}

