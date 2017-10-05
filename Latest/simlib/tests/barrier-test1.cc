// Barrier: simple test of basic functionality
#include <simlib.h>

Barrier b("b", 5);

class BarrierTest1 : public Process {
    void Behavior(void) {
        Print("b:start %p\n", this);
        b.Wait(); // wait on barrier
        Print("b:end %p\n", this);
    }
};

int main()
{
    //DebugON();
    Init(0, 5);
    for (unsigned a=0; a < b.Height()+2; ++a)
        (new BarrierTest1)->Activate();
    Run();
    b.Output();
}
