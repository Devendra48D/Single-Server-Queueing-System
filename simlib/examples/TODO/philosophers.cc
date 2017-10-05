////////////////////////////////////////////////////////////////////////////
// Dining philosophers model using SIMLIB/C++
//

#include "simlib.h"

const double ENDTime = 100;      // time of simulation

struct DiningPhilosophers {

    static const int N = 5;     // number of philosophers

    class Fork {
        Semaphore s;
      public:
        void Get()  { s.P(); } 
        void Free() { s.V(); }
    };

    class Philosopher: public Process {
        enum { THINKING, EATING } state;
        int number;
        Fork &left, &right;
        void Behavior() {
            while(true) {
                Wait(Random());
                left.Get();
                right.Get();
                state = EATING;
                Print("Philosopher%d:start\n", number);
                Wait(Random());
                state = THINKING;
                Print("Philosopher%d:stop\n", number);
                left.Free();
                right.Free();
            }
        } 
      public:
        Philosopher(int id, Fork &l, Fork &r):
            state(THINKING), number(id), left(l), right(r) { }
    };

    Philosopher *p[N];  // array of philosopher handles
    Fork f[N];          // array of forks

    // constructor creates ring structure
    DiningPhilosophers() {
        for(int i = 0; i < N; i++)
            p[i] = new Philosopher(i+1, f[i], f[(i+1)%N]);
    }

    void ActivateAll() {
        for(int i = 0; i < N; i++)
            p[i]->Activate();   // start behavior
    }

}; // model class

DiningPhilosophers m;      // model object

int main()
{
    _Print("Model of Dining Philosophers\n");
    Init(0, ENDTime);           // initialize time 0..ENDTime
    m.ActivateAll();            // initialize model
    Run();                      // simulation experiment
}

//
