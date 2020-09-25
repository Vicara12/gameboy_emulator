#ifndef ARITHMETIC_INSTRUCTIONS_H_
#define ARITHMETIC_INSTRUCTIONS_H_

#include "instructions.h"

// increment instruction
class IncAI : public Instruction
{
public:

   IncAI (Memory::Register reg);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

private:

   Memory::Register reg_;
};

// ~~~~~~~~~~~~~~~~~

// decrement instruction
class DecAI : public Instruction
{
public:

   DecAI (Memory::Register reg);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

private:

   Memory::Register reg_;
};

// ~~~~~~~~~~~~~~~~~

// performs every 8bit arithmetical operation in the Operation enum
class ArithmeticOperationAI : public Instruction
{
public:

   enum Operation {ADD, ADC, SUB, SBC, AND, XOR, OR, CP};

   ArithmeticOperationAI (Memory::Register reg, Operation op, bool use_reg);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

private:

   Memory::Register reg_;
   Operation op_;
   bool use_reg_;
};


// ~~~~~~~~~~~~~~~~~

// correct register A binary encoded representation
class DecimalAdjustAI : public Instruction
{
public:

   DecimalAdjustAI ();
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

};

// ~~~~~~~~~~~~~~~~~

// flip bits of register A
class CompAAI : public Instruction
{
public:

   CompAAI ();
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

};

// ~~~~~~~~~~~~~~~~~

// reset or flip carry flag
class ChangeCFlagAI : public Instruction
{
public:

   ChangeCFlagAI (bool complement);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

private:

   bool complement_;
};

// ~~~~~~~~~~~~~~~~~

// inc dec 16 bit registers
class IncDec16BitAI : public Instruction
{
public:

   IncDec16BitAI (Memory::Register reg, bool inc);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

private:

   bool inc_;
   Memory::Register reg_;
};

// ~~~~~~~~~~~~~~~~~

// addition for 16 bit registers
class Add16BitsAI : public Instruction
{
public:

   Add16BitsAI (Memory::Register reg);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

private:

   Memory::Register reg_;
};

// ~~~~~~~~~~~~~~~~~

// addition for 16 bit registers
class ADDr8SPAI : public Instruction
{
public:

   ADDr8SPAI ();
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

};

#endif