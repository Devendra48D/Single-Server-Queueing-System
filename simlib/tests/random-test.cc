////////////////////////////////////////////////////////////////////////////
// Model MODEL2.CPP               SIMLIB/C++
//
// Uk�zkov� model jednoduch�ho obslu�n�ho st�ediska
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
