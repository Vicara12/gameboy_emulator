
#include "shift_instructions.h"

Rotate::Rotate (bool right, bool through_c, Memory::Register reg, bool fast_a) :
      Instruction(fast_a ? 1 : 2,
                  1 + (not fast_a) + 2*Memory::registerIs16bits(reg),
                  "", ""),
      right_(right),
      through_c_(through_c),
      reg_(reg)
{
   if (fast_a)
   {
      Instruction::instr_name_ = ("R" + std::string(right_ ? "R" : "L") +
                                  (through_c_ ? "C" : "") + "A");
      Instruction::verbose_name_ = ("(rotate register A " +
                                 std::string(right_ ? "right" : "left") +
                                 (through_c_ ? "through carry" : "") + ")");
   }
   else
   {
      Instruction::instr_name_ = ("R" + std::string(right_ ? "R" : "L") +
                                  (through_c_ ? "C" : "") + "\t" +
                                  Memory::regString(reg_));
      Instruction::verbose_name_ = ("(rotate register " + Memory::regString(reg_)
                                 + std::string(right_ ? " right" : " left") +
                                 (through_c_ ? " through carry" : "") + ")");
   }
   
}

void Rotate::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint8_t val;

   // select value of val acording to source
   if (Memory::registerIs16bits(reg_))
   {
      uint16_t address = memory->readReg(Memory::Register::HL);
      val = memory->readMem(address);
   }
   else
      val = memory->readReg(reg_);

   // opposite bit: bit 0 if rotate left and bit 7 if rotate right

   // save opposite bit in case through_c is active
   bool bit = ((val & (right_ ? 0x80 : 0x01)) != 0);
   bool flag = memory->readFlag(Memory::Flag::C_f);

   // if through_c, actualize flag with the new bit
   memory->writeFlag(Memory::Flag::C_f, bit);

   if (right_) val = val >> 1;
   else        val = val << 1;

   // if through_c, set opposite bit to carry flag or shifted bit
   if ((flag and through_c_) or (not flag and bit))
      val = val & (right_ ? 0x80 : 0x01);

   // save value
   if (Memory::registerIs16bits(reg_))
      memory->writeMem(memory->readReg(reg_), val);
   else
      memory->writeReg(reg_, val);
}

// ~~~~~~~~~~~~~~~~~

Shift::Shift (bool right, Memory::Register reg, bool logical) :
      Instruction(2, 2 + 2*Memory::registerIs16bits(reg), "", ""),
      right_(right),
      logical_(logical),
      reg_(reg)
{
   
   Instruction::instr_name_ = ("S" + std::string(right_ ? "R" : "L") +
                              (logical_ ? "L" : "A") + "\t" +
                              Memory::regString(reg_));
   Instruction::verbose_name_ = ("(shift register " + Memory::regString(reg_)
                              + std::string(right_ ? " right" : " left") +
                              (logical_ ? " logical" : " arithmetic") + ")");
}

void Shift::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint8_t val;

   // select value of val acording to source
   if (Memory::registerIs16bits(reg_))
   {
      uint16_t address = memory->readReg(Memory::Register::HL);
      val = memory->readMem(address);
   }
   else
      val = memory->readReg(reg_);

   // opposite bit: bit 0 if rotate left and bit 7 if rotate right

   // save opposite bit in case through_c is active
   bool bit = ((val & (right_ ? 0x80 : 0x01)) != 0);
   bool flag = memory->readFlag(Memory::Flag::C_f);

   // if through_c, actualize flag with the new bit
   memory->writeFlag(Memory::Flag::C_f, bit);

   if (right_) val = val >> 1;
   else        val = val << 1;

   // if shift right arithmetic and bit 7 was an 1, set it again
   if (right_ and bit and not logical_)
      val = val & 0x80;

   // save value
   if (Memory::registerIs16bits(reg_))
      memory->writeMem(memory->readReg(reg_), val);
   else
      memory->writeReg(reg_, val);
   
   // write flag
   memory->writeFlag(Memory::Flag::C_f, flag);
}

// ~~~~~~~~~~~~~~~~~

Swap::Swap (Memory::Register reg) :
      Instruction(2, 2 + 2*Memory::registerIs16bits(reg),
                  "SWAP \t" + Memory::regString(reg),
                  "(swap nibbles of register " + Memory::regString(reg) + ")"),
      reg_(reg) {}

void Swap::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint8_t val;

   // select value of val acording to source
   if (Memory::registerIs16bits(reg_))
   {
      uint16_t address = memory->readReg(Memory::Register::HL);
      val = memory->readMem(address);
   }
   else
      val = memory->readReg(reg_);
   
   val = (val << 4) | (val >> 4);

   // save value
   if (Memory::registerIs16bits(reg_))
      memory->writeMem(memory->readReg(reg_), val);
   else
      memory->writeReg(reg_, val);

   // set flags
   memory->writeFlag(Memory::Flag::Z_f, val == 0);
   memory->writeFlag(Memory::Flag::N_f, 0);
   memory->writeFlag(Memory::Flag::H_f, 0);
   memory->writeFlag(Memory::Flag::C_f, 0);
}

// ~~~~~~~~~~~~~~~~~

Bit::Bit (Memory::Register reg, unsigned bit) :
      Instruction(2, 2 + 2*Memory::registerIs16bits(reg),
      "BIT \t" + std::to_string(bit) + ", " + Memory::regString(reg),
      "(set Z flag to bit " + std::to_string(reg) + " of register " +
         Memory::regString(reg) + ")"),
      reg_(reg),
      bit_(bit) {}

void Bit::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint8_t val;

   // select value of val acording to source
   if (Memory::registerIs16bits(reg_))
   {
      uint16_t address = memory->readReg(Memory::Register::HL);
      val = memory->readMem(address);
   }
   else
      val = memory->readReg(reg_);

   // set flags
   memory->writeFlag(Memory::Flag::Z_f, (val & (0x01 << bit_)) == 0);
   memory->writeFlag(Memory::Flag::N_f, 0);
   memory->writeFlag(Memory::Flag::H_f, 1);
}

// ~~~~~~~~~~~~~~~~~

SetResetBit::SetResetBit (Memory::Register reg, unsigned bit, bool set) :
      Instruction(2, 2 + 2*Memory::registerIs16bits(reg),
                  (set ? "SET" : "RES") + std::string(" \t") +
                  std::to_string(bit) + ", " + Memory::regString(reg),
                  "(" + std::string(set ? "set" : "reset") + " bit " +
                  std::to_string(reg) + " of register " +
                  Memory::regString(reg) + ")") ,
      reg_(reg),
      bit_(bit),
      set_(set) {}

void SetResetBit::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint8_t val;

   // select value of val acording to source
   if (Memory::registerIs16bits(reg_))
   {
      uint16_t address = memory->readReg(Memory::Register::HL);
      val = memory->readMem(address);
   }
   else
      val = memory->readReg(reg_);
   
   // set or reset bit
   if (set_)   val = val | (0x01 << bit_);
   else        val = val & ((0x01 << bit_) ^ 0xff);

   // save value
   if (Memory::registerIs16bits(reg_))
      memory->writeMem(memory->readReg(reg_), val);
   else
      memory->writeReg(reg_, val);
}