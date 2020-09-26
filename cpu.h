#ifndef CPU_H_
#define CPU_H_

#include "memory.h"
#include "instructions.h"
#include "load_instructions.h"
#include "arithmetic_instructions.h"
#include "other_instructions.h"
#include "shift_instructions.h"

class CPU
{
public:

   CPU ();

   void displayInstructionSet (bool verbose);

private:

   void initInstructionVectors ();


   unsigned long clock;

   // instruction for each address
   Instruction* instruction_set [0x100];

   // subset for the instruction 0xCB
   Instruction* CB_subset [0x100];

};

#endif