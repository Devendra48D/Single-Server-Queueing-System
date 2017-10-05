////////////////////////////////////////////////////////////////////////////
// Model MODEL2.CPP               SIMLIB/C++
//
// Ukázkový model jednoduchého obslu¾ného støediska
//

#include "simlib.h"

int main() {                 // popis experimentu s modelem
  //DebugON();
  Print("Test of internal random generator of SIMLIB/C++\n");
  SetOutput("random-test.out");
  for(int i=0; i<100; i++)
      Print("%g\n",Random());
}

// konec
