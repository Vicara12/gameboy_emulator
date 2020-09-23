#ifndef INSTRUCTIONS_H_
#define INSTRUCTIONS_H_

#include "memory.h"
#include <exception>
#include <cstdint>


// abstract base class for all the instruction types
class Instruction
{
public:

   Instruction (unsigned byte_lenght, unsigned instruction_cycles);
   virtual void execute (uint8_t inst_first_byte, uint8_t inst_second_byte) = 0;
   //virtual void name () = 0;
   unsigned getByteSize () const;
   unsigned getCycleLenght () const;

   // exclusive for instructions with variable cycle duration
   // (must override if that's the case)
   virtual unsigned extraCycleTime () const;
   virtual bool longPathTaken () const;

protected:

   unsigned instr_cycles;        // number of instruction cycles to execute
   unsigned instr_byte_lenght;   // lenght of the instruction in bytes
   Memory* memory;               // pointer to the memory class
};

// ~~~~~~~~~~~~~~~~~

class InvalidInstruction : public Instruction
{
public:

   InvalidInstruction ();
   virtual void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);
};

struct InstructionDoesNotExist : public std::exception
{
  const char* what () const throw ()
  {
    return "the decoded instruction doesn't exists";
  }
};

#endif