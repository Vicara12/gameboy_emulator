
#include "cpu.h"

CPU::CPU () :
   clock(0)
{
   initInstructionVectors();
}

void CPU::displayInstructionSet (bool verbose)
{
   for (int i = 0; i < 0x100; i++)
      std::cout << Memory::getHex(i) << "  " << instruction_set[i]->name(verbose) << std::endl;

   std::cout << "\n\n";

   for (int i = 0; i < 0x100; i++)
      std::cout << Memory::getHex(i) << "  " << CB_subset[i]->name(verbose) << std::endl;

}