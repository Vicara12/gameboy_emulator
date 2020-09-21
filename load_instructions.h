#ifndef LOAD_INSTRUCTIONS_H_
#define LOAD_INSTRUCTIONS_H_

#include "instructions.h"

// load from reg to reg or from 8bit to reg
class NormalLD : public Instruction
{
public:

   NormalLD (Memory::Register reg_write,
             Memory::Register reg_read,
             bool immediate_8bit = false);
   
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

private:

   Memory::Register reg_w, reg_r;
   bool immediate_8bit_;
};

// ~~~~~~~~~~~~~~~~~

// load to reg from (16bit) or the other way arround
class DoubleByteLD : public Instruction
{
public:

   DoubleByteLD (bool reg_at_read, bool register_a = true);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

private:

   bool reg_read, reg_a;
};

// ~~~~~~~~~~~~~~~~~

// load operations with HL increment or decrement
class HLIterateLD : public Instruction
{
public:

   HLIterateLD (bool hl_read, bool hl_increment);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

private:

   bool hl_r, hl_inc;
};

// ~~~~~~~~~~~~~~~~~

// instructions that operate over the address 0xFF00 (LDH and LD (C))
class FF00LD : public Instruction
{
public:

   FF00LD (bool register_a_read, bool use_register_c);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

private:

   bool reg_a_read, use_c;
};

// ~~~~~~~~~~~~~~~~~

// instructions that load a 16bit value into register
class Number16bitToRegLD : public Instruction
{
public:

   Number16bitToRegLD (Memory::Register reg);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

private:

   Memory::Register reg_;
};

// ~~~~~~~~~~~~~~~~~

// instruction 0xF8: LD HL, SP+r8
class InstructionF8LD : public Instruction
{
public:

   InstructionF8LD ();
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);
};

// ~~~~~~~~~~~~~~~~~

// push instruction
class PushLD : public Instruction
{
public:

   PushLD (Memory::Register reg);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

private:

   Memory::Register reg_;
};

// ~~~~~~~~~~~~~~~~~

// pop instruction
class PopLD : public Instruction
{
public:

   PopLD (Memory::Register reg);
   void execute (uint8_t inst_first_byte, uint8_t inst_second_byte);

private:

   Memory::Register reg_;
};


#endif