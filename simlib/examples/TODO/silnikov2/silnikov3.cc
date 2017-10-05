
#include "simlib.h"
#include <iostream>
#include <cmath>

using namespace std;

const double StepPrn = 0.05;    // step of output

// model:
class Silnikov {
  public:
    Parameter a, b, c, d;
    Integrator x1, x2, x3;
    Silnikov(double _a, double _b, double _c, double _d) :
        a(_a), b(_b), c(_c), d(_d),
        x1(x2, 0.1234),
        x2(x3, 0.2),
        x3(-a * x3 - x2 + b * x1 * (1 - c * x1 - d * x1 * x1), 0.1)
      {}
};

Silnikov e(0.4, 0.65, 0, 1);

// output sampling:
void Sample()
{
    cout << Time << ' ' << e.x1.Value() << ' ' << e.x2.Value() << '\n';
    if (fabs(e.x1.Value() * e.x2.Value()) > 1e3)
        Stop();
}
Sampler S(Sample, StepPrn);

// experiment description:
int main()
{
    cout << "# Silnikov equation output: step 1e-10 .. 1e-3\n";
    SetStep(1e-10,1e-3);
    SetMethod("abm4"); // Available methods: abm4 rkf5 euler fw rke rkf3 rkf5 rkf8
    Init(0, 350);               // experiment initialization
    Run();                      // simulation

    cout << "\n\n";  // output data separator for Gnuplot

    cout << "# Silnikov equation output: step 1e-10 .. 1e-3\n";
    SetStep(1e-10,1e-3);
    SetMethod("rkf5"); // Available methods: abm4 rkf5 euler fw rke rkf3 rkf5 rkf8
    Init(0, 350);               // experiment initialization
    Run();                      // simulation

    cout << "\n\n";  // output data separator for Gnuplot

    cout << "# Silnikov equation output: step 1e-10 .. 1e-3\n";
    SetStep(1e-10,1e-3);
    SetMethod("rkf8"); // Available methods: abm4 rkf5 euler fw rke rkf3 rkf5 rkf8
    Init(0, 350);               // experiment initialization
    Run();                      // simulation

    cout << "\n\n";  // output data separator for Gnuplot

    cout << "# Silnikov equation output: step 1e-10 .. 1e-3\n";
    SetStep(1e-10,1e-3);
    SetMethod("rke"); // Available methods: abm4 rkf5 euler fw rke rkf3 rkf5 rkf8
    Init(0, 350);               // experiment initialization
    Run();                      // simulation
}


