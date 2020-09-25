#include <iostream>
#include "memory.h"
#include "instruction_set_vec.h"
using namespace std;

int main ()
{
   initInstructionVectors();
   
   for (int i = 0; i < 0x100; i++)
      cout << Memory::getHex(i) << " -> " << instruction_set[i]->name(true) << endl;
}