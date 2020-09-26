#include <iostream>
#include "memory.h"
#include "cpu.h"
using namespace std;

int main ()
{
   CPU* cpu = new CPU();

   cpu->displayInstructionSet(true);
}