#include <iostream>
#include "memory.h"
using namespace std;

int main ()
{
   string registers [] = {"B", "C", "D", "E", "H", "L", "HL","A"};

   for (int i = 0x80; i < 0x100; i++)
   {
      cout << "/* " << Memory::getHex(i) <<
      " */ , new SetResetBit(Memory::Register::" << registers[(i-0x80)%8] <<
      ", " << ((i-0x80)/8%8) << ", " << (i >= 0xc0 ? "true" : "false") <<
      ")" << endl;
   }
}