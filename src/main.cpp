#include <iostream>
#include "gbmemory.h"
#include "interrupts.h"
using namespace std;

#define INSTRUCTION  0xcf
#define MEM_FROM     0x0100
#define MEM_TO       0x010a

#define IE_ADDRESS   0xffff
#define IF_ADDRESS   0xff0f

int main ()
{
   Interrupts inter;
   Memory* memory = Memory::getInstance();

   memory->changeIntEnabled(true);
   memory->writeMem(IE_ADDRESS, 0xf7, false);
   memory->writeMem(IF_ADDRESS, 0x00, false);

   cout << Memory::getHex(inter.interruptActive(), 4) << endl;
   cout << Memory::getHex(inter.interruptActive(), 4) << endl;

   cout << "--------\n";

   memory->writeMem(0xff0f, 0x02, false);

   inter.setIFStatus(1, true);

   inter.setIFStatus(4, true);

   inter.setIFStatus(5, true);

   cout << Memory::getHex(inter.interruptActive(), 4) << endl;
   cout << Memory::getHex(inter.interruptActive(), 4) << endl;
   cout << Memory::getHex(inter.interruptActive(), 4) << endl;
   cout << Memory::getHex(inter.interruptActive(), 4) << endl;

   memory->writeMem(IE_ADDRESS, 0xff, false);

   cout << Memory::getHex(inter.interruptActive(), 4) << endl;
   cout << Memory::getHex(inter.interruptActive(), 4) << endl;
   cout << Memory::getHex(inter.interruptActive(), 4) << endl;
   cout << Memory::getHex(inter.interruptActive(), 4) << endl;


   cout << Memory::getHex(memory->readMem(IF_ADDRESS), 4) << endl;
}