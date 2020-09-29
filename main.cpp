#include <iostream>
#include "memory.h"
#include "cpu.h"
using namespace std;

int main ()
{
   cout << "\n\n\n";

   CPU* cpu = new CPU();

   cpu->changeDebugMode(true, true, true, true);

   cpu->disasemble (0x00, 0x0100, true);
}