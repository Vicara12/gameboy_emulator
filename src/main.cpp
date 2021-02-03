#include <iostream>
#include <unistd.h>
#include "gbmemory.h"
#include "oscillator.h"
#include "cpu.h"
using namespace std;

#define MEM_FROM     0x0100
#define MEM_TO       0x010a

// timer register addresses
#define  DIV_ADDRESS 0Xff04
#define TIMA_ADDRESS 0xff05
#define  TMA_ADDRESS 0xff06
#define  TAC_ADDRESS 0xff07

int main ()
{   
   Memory* memory = Memory::getInstance();
   CPU cpu;

   memory->writeReg(Memory::Register::PC, 0x0000, true);

   // setup timer registers
   

   // load a sample program in memory
   memory->writeMem(0x0000, 0x00, true);
   memory->writeMem(0x0001, 0x00, true);
   memory->writeMem(0x0002, 0x00, true);
   memory->writeMem(0x0003, 0x00, true);
   // jp to 00
   memory->writeMem(0x0004, 0xc3, true);
   memory->writeMem(0x0005, 0x00, true);
   memory->writeMem(0x0006, 0x00, true);

   for (int i = 0; i < 0x100; i++)
   {
      //
   }
}