#ifndef INSTRUCTION_SET_VEC_H_
#define INSTRUCTION_SET_VEC_H_

#include "instructions.h"
#include "load_instructions.h"
#include "arithmetic_instructions.h"
#include "other_instructions.h"
#include "shift_instructions.h"

// assign the instruction to every opcode
// (must be called before using any isntruction)
void initInstructionVectors ();

// instruction for each address
Instruction* instruction_set [0x100];

// subset for the instruction 0xCB
Instruction* CB_subset [0x100];

#endif