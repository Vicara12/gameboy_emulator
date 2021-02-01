#include <iostream>
#include <unistd.h>
#include "gbmemory.h"
#include "oscillator.h"
#include "cpu.h"
using namespace std;

#define MEM_FROM     0x0100
#define MEM_TO       0x010a

#define IO_REGISTER_ADDRESS  0xff00

int main ()
{   
   Memory* memory = Memory::getInstance();
   CPU cpu;

   Oscillator::beginOscillator();

   for (int i = 0; i < 0x200; i++)
   {
      if (i>=0xc0 and i<0x100) continue;

      unsigned long t1 = Oscillator::getClockCycles();
      cpu.executeInstruction(i%100, 0x00, 0x00, i >= 0x100, false);
      unsigned long t2 = Oscillator::getClockCycles();

      cout << Memory::getHex(i) << ": " << (t2-t1) << endl;

      memory->changeCpuHalt(false);
      memory->changeCpuStop(false);
   }
}