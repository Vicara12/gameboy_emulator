#include <iostream>
#include "gbmemory.h"
#include "cpu.h"
using namespace std;

int main ()
{
   CPU cpu;

   cout << endl;

   Memory::getInstance()->writeReg(Memory::Register::A, 0xffff);

   Memory::getInstance()->writeFlag(Memory::Flag::C_f, true);

   Memory::getInstance()->displayMemoryChunk(0, 0x100);

   cout << endl;

   Memory::getInstance()->outputMemoryStatus(false);
}