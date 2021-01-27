#include <iostream>
#include "gbmemory.h"
#include "cpu.h"
using namespace std;

#define INSTRUCTION  0xcf
#define MEM_FROM     0x0100
#define MEM_TO       0x010a

int main ()
{
   CPU cpu;
   Memory* memory = Memory::getInstance();

   cout << endl;

   memory->writeReg(Memory::Register::PC, 0x010a);

   memory->writeReg(Memory::Register::HL, 0x0107);
   memory->writeReg(Memory::Register::A, 0x42);

   memory->writeReg(Memory::Register::SP, 0x0105);

   memory->changeIntEnabled(false);


   for (int i = MEM_FROM; i < MEM_TO; i++)
      memory->writeMem(i, (i*3)%0x100);


   memory->writeFlag(Memory::Flag::N_f, false);
   memory->writeFlag(Memory::Flag::C_f, false);
   memory->writeFlag(Memory::Flag::Z_f, false);
   memory->writeFlag(Memory::Flag::H_f, false);

   memory->displayMemoryChunk(MEM_FROM, MEM_TO);

   cout << endl;

   memory->outputMemoryStatus(true);
   cout << "CLK:  " << cpu.getCLK() << endl;

   cout << endl;

   //cpu.executeInstruction(INSTRUCTION, 0x34, 0x12, false, false);

   //cpu.executeInstruction(0x00, 0, 0, false, false);

   cout << endl;

   memory->outputMemoryStatus(true);
   cout << "CLK:  " << cpu.getCLK() << endl;

   memory->displayMemoryChunk(MEM_FROM, MEM_TO);
}