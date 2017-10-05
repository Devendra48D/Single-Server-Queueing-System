////////////////////////////////////////////////////////////////////////////
// SIMLIB/C++ -- basic calendar test
//
// simple check: increasing time ordering of event execution
//
#include "simlib.h"
#include <cstdlib>
#include <cmath>

long     N         = 1000;
unsigned number    = 0;
double   Last_Time = 0.0;

class TestEvent : public Event {
  void Behavior() {
      if (Time < Last_Time)
          Error("Bad calendar implementation %g < %g", Time, Last_Time);
      Last_Time = Time;
  }
  public:
  TestEvent() { ++number; }
  ~TestEvent() { --number; }
};

// various test distributions of time intervals
// all should have mean==1
double DistributionExp() {
    double x = Exponential(1);
    return x;
}
double DistributionUni() {
    double x = 2*Random();
    return x;
}
double DistributionBiased() {
    double x = 0.9+0.2*Random();
    return x;
}
double DistributionBimodal() {
    double x = 0.95238*Random();
    if(Random()<0.1) x+=9.5238;
    return x;
}
double DistributionTriangular() {
    double x = 1.5*std::sqrt(Random());
    return x;
}

// 
const int N_D = 5;
double (*distribution[N_D])() = {
    DistributionUni,
    DistributionExp,
    DistributionBiased,
    DistributionBimodal,
    DistributionTriangular,
};

// 
double MAX=0;
int dd = 0;
double Distribution() {
    double x = distribution[dd]();
    if (x>MAX) MAX=x;
    return x;
}

// experiment
int main(int argc, char *argv[])
{
    //DebugON();
#if 1
    SetCalendar("cq"); // calendar queue
#endif
    if (argc > 1) {
        N = 0;
        N = std::strtoul(argv[1], 0, 10);
        if (N < 1)
            _Print("Error: bad argument\n");
    }
    Print("Calendar size: %ld\n", N);
    int i = 0;
    for(dd=0; dd < N_D; ++dd) try {
        Init(0);          // Initialize time, calendar, ...
        Last_Time=0.0;
        MAX=0;
        Print("Run#%d ", dd);
        // create and activate N events 
        // we need to:
        //  - preallocate Events
        //  - preallocate EventNotices
        for (i = 0; i < N; i++) {
            (new TestEvent)->Activate(Distribution());
        }
        // Calendar filled
        //Print("max=%g\n", MAX);
        // and now dequeue all
        Run();                  // simulation
        // delete events automatically by end of Behavior()
        Print("OK\n");
    }
    catch(std::bad_alloc) {
        Print("No memory after %d created events\n", i);
    }
    catch(...) {
        Print("Exception\n");
    }
}
