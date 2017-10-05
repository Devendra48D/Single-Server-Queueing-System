////////////////////////////////////////////////////////////////////////////
// Model: schema.cc
//
// Interesting continuous model (SOURCE: prof. Serba) -- v2
//

#include "simlib.h"

struct System {
    struct SubSystem {
        Integrator u, du;
        SubSystem(double a, double b, double c, double initval):
            du( a*du - b*u, initval ),
            u( c*du ) {}
        double x() { return du.Value(); }
        double y() { return u.Value();  }
    };
    SubSystem s1;
    SubSystem s2;
    System():
        s1(0.05, 0.1, 0.1, 0.9),       // Subsystem1
        s2(0.05, 10,  10,  0.1) {}     // Subsystem2
    double x()  { return s1.x() + s2.x(); }
    double y()  { return s1.y() + s2.y(); }
};

System k;                       // Model

void Sample() {
    Print("%f %g %g %g\n", Time, k.x(), k.y(), -k.y());
}

Sampler S(Sample, 0.01);

int main() {                    // Experiment ...
    SetOutput("schema.dat");
    Init(0, 195);
    SetStep(1e-6, 0.01);
    SetAccuracy(1e-3);
    Run();
}

