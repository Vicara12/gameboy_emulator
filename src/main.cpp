#include <iostream>
#include <unistd.h>
#include "gbmemory.h"
#include "padinput.h"
using namespace std;

#define MEM_FROM     0x0100
#define MEM_TO       0x010a

#define IO_REGISTER_ADDRESS  0xff00

int main ()
{   
   Interrupts inter;
   Memory* memory = Memory::getInstance();

   memory->changeIntEnabled(true);
   // enable p14
   memory->writeMem(IO_REGISTER_ADDRESS, 0x10, false);
  
   while (1)
   {
      PadInput::processInput();

      cout << Memory::getHex(memory->readMem(IO_REGISTER_ADDRESS, true)) << endl;
      usleep(1e5);
   }
}