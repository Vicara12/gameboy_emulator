#include "instructions.h"


Instruction::Instruction (unsigned byte_lenght, unsigned instruction_cycles) :
      instr_cycles(instruction_cycles),
      instr_byte_lenght(byte_lenght),
      memory(Memory::getInstance()) {}

unsigned Instruction::getByteSize () const
{
   return instr_byte_lenght;
}

unsigned Instruction::getCycleLenght () const
{
   return instr_cycles;
}

unsigned Instruction::extraCycleTime () const
{
   return 0;
}

bool Instruction::longPathTaken () const
{
   return false;
}

InvalidInstruction::InvalidInstruction () :
      Instruction(0,0) {}

void InvalidInstruction::execute (uint8_t inst_first_byte,
                                  uint8_t inst_second_byte)
{
   throw InstructionDoesNotExist();
}