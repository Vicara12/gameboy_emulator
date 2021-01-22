#ifndef SHIFT_INSTRUCTIONS_H_
#define SHIFT_INSTRUCTIONS_H_

#include "instructions.h"

// rotate register
class Rotate : public Instruction
{
public:

   Rotate (bool right, bool through_c, Memory::Register reg_, bool fast_a);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

private:

   bool right_, through_c_;
   Memory::Register reg_;
};

// ~~~~~~~~~~~~~~~~~

// shift register
class Shift : public Instruction
{
public:

   Shift (bool right, Memory::Register reg_, bool logical);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

private:

   bool right_, logical_;
   Memory::Register reg_;
};

// ~~~~~~~~~~~~~~~~~

// swap nibbles of the register
class Swap : public Instruction
{
public:

   Swap (Memory::Register reg);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

private:

   Memory::Register reg_;
};

// ~~~~~~~~~~~~~~~~~

// set Z flag if nth bit of the register is zero
class Bit : public Instruction
{
public:

   Bit (Memory::Register reg, unsigned bit);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

private:

   Memory::Register reg_;
   unsigned bit_;
};

// ~~~~~~~~~~~~~~~~~

// set or reset nth bit of the selected register
class SetResetBit : public Instruction
{
public:

   SetResetBit (Memory::Register reg, unsigned bit, bool set);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

private:

   Memory::Register reg_;
   unsigned bit_;
   bool set_;
};

#endif