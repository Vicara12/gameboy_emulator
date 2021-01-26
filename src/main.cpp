#include <iostream>
#include "gbmemory.h"
#include "cpu.h"
using namespace std;

#define INSTRUCTION  0xf3
#define MEM_FROM     0xff00
#define MEM_TO       0xff20

int main ()
{
   CPU cpu;
   Memory* memory = Memory::getInstance();

   cout << endl;

   memory->writeReg(Memory::Register::PC, 0x0100);

   memory->writeReg(Memory::Register::C, 0x12);
   memory->writeReg(Memory::Register::A, 0x04);

   memory->writeReg(Memory::Register::SP, 0x0102);


   for (int i = MEM_FROM; i < MEM_TO; i++)
      memory->writeMem(i, (i*3)%0x100);


   memory->writeFlag(Memory::Flag::N_f, true);
   memory->writeFlag(Memory::Flag::C_f, true);
   memory->writeFlag(Memory::Flag::Z_f, true);
   memory->writeFlag(Memory::Flag::H_f, true);

   memory->displayMemoryChunk(MEM_FROM, MEM_TO);

   cout << endl;

   memory->outputMemoryStatus(true);
   cout << "CLK:  " << cpu.getCLK() << endl;

   cout << endl;

   cpu.executeInstruction(INSTRUCTION, 0x0a, 0x42, false, false);

   cout << endl;

   memory->outputMemoryStatus(true);
   cout << "CLK:  " << cpu.getCLK() << endl;

   memory->displayMemoryChunk(MEM_FROM, MEM_TO);
}