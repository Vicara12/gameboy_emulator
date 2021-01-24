#include <iostream>
#include "gbmemory.h"
#include "cpu.h"
using namespace std;

int main ()
{
   CPU cpu;

   //cpu.displayInstructionInfo(0x02, false, false);
   for (int j = 0; j < 0x10; j++)
   {
      for (int i = 0; i < 0x100; i+= 0x10)
      {
         int n = (i | j);
         cpu.displayInstructionInfo(n, false, false);
      }
      std::cout << "~~~~~~~~~~~" << std::endl;
   }
}