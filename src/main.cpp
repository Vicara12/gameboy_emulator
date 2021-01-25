#include <iostream>
#include "gbmemory.h"
#include "cpu.h"
using namespace std;

#define INSTRUCTION  0xf6
#define MEM_FROM     0x100
#define MEM_TO       0x10a

int main ()
{
   CPU cpu;
   Memory* memory = Memory::getInstance();

   cout << endl;

   memory->writeReg(Memory::Register::PC, 0x0100);

   memory->writeReg(Memory::Register::B, 0x00);
   memory->writeReg(Memory::Register::E, 0x42);

   memory->writeReg(Memory::Register::HL, 0x0105);

/*
   for (int i = 0x50; i < 0x200; i++)
      memory->writeMem(i, i+0x11);
*/
   memory->writeMem(0x105, 0x00);

   memory->writeFlag(Memory::Flag::N_f, true);
   memory->writeFlag(Memory::Flag::C_f, true);
   memory->writeFlag(Memory::Flag::Z_f, true);
   memory->writeFlag(Memory::Flag::H_f, true);

   memory->displayMemoryChunk(MEM_FROM, MEM_TO);

   cout << endl;

   memory->outputMemoryStatus(true);
   cout << "CLK:  " << cpu.getCLK() << endl;

   cout << endl;

   cpu.executeInstruction(INSTRUCTION, 0xfe, 0x01, true, false);

   cout << endl;

   memory->outputMemoryStatus(true);
   cout << "CLK:  " << cpu.getCLK() << endl;

   memory->displayMemoryChunk(MEM_FROM, MEM_TO);
}