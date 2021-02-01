#include "instructions.h"


Instruction::Instruction (unsigned byte_lenght,
                          unsigned instruction_cycles,
                          std::string instr_name,
                          std::string verbose_name) :
      instr_name_(instr_name),
      verbose_name_(verbose_name),
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


std::string Instruction::name () const
{
   return instr_name_;
}


std::string Instruction::description () const
{
   return verbose_name_;
}


unsigned Instruction::extraCycleTime () const
{
   return 0;
}


bool Instruction::longPathTaken () const
{
   return false;
}


bool Instruction::invalidInstruction () const
{
   return false;
}

// ~~~~~~~~~~~~~~~~~

InvalidInstruction::InvalidInstruction () :
      Instruction(1,1, "INV   ", "(invalid operation code)") {}


void InvalidInstruction::execute (uint8_t inst_first_byte,
                                  uint8_t inst_second_byte)
{
   throw InstructionDoesNotExist();
}


bool InvalidInstruction::invalidInstruction () const
{
   return true;
}