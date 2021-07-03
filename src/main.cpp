#include "lcd.h"
#include "pixel_processing_unit.h"
#include <unistd.h>



int main ()
{
   PixelProcessingUnit::getInstance()->test();
}


/*
int main ()
{
   LCD lcd;

   PixelProcessingUnit::getInstance()->actualize();

   while (lcd.refresh())
   {
      usleep(1e6/57);
   }
}


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
   memory->writeMem(TMA_ADDRESS, 0xf0);
   memory->writeMem(TAC_ADDRESS, 0x01);

   // load a sample program in memory
   memory->writeMem(0x0000, 0x00, true);
   memory->writeMem(0x0001, 0x00, true);
   memory->writeMem(0x0002, 0x00, true);
   memory->writeMem(0x0003, 0x00, true);
   // jp to 00
   memory->writeMem(0x0004, 0xc3, true);
   memory->writeMem(0x0005, 0x00, true);
   memory->writeMem(0x0006, 0x00, true);

   cpu.changeDebugMode(true);



   for (int i = 0; i < 0x200; i++)
   {
      cout << "\n~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n";

      memory->outputMemoryStatus();

      if (i == 0x1000)
         memory->writeMem(TAC_ADDRESS, 0x05);

      cout << "i: " << Memory::getHex(i) << " | ";
      cout << "PC: " << Memory::getHex(memory->readReg(Memory::Register::PC),4) << " | ";
      cout << "clk: " << cpu.getCLK() << " | ";
      cout << "DIV: " << Memory::getHex(memory->readMem(DIV_ADDRESS, true), 2) << " | ";
      cout << "TIMA: " << Memory::getHex(memory->readMem(TIMA_ADDRESS, true), 2) << endl;

      cpu.executeCPUCycle();
   }
}
*/