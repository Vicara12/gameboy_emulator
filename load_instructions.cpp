#include "load_instructions.h"

NormalLD::NormalLD (Memory::Register reg_write,
                    Memory::Register reg_read,
                    bool immediate_8bit) :
      Instruction(1 + immediate_8bit, 1),
      reg_w(reg_write),
      reg_r(reg_read),
      immediate_8bit_(immediate_8bit)
{
   // if an address must be loaded or an immediate val is used, increment cycles
   if (Memory::registerIs16bits(reg_r) or Memory::registerIs16bits(reg_w))
      instr_cycles++;
   if (immediate_8bit)
      instr_cycles++;
}

void NormalLD::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint8_t written_val;

   // select the source of the data to be written
   if (immediate_8bit_)
      written_val = inst_first_byte;
   else if (Memory::registerIs16bits(reg_r))
      written_val = memory->readMem(reg_r);
   else
      written_val = memory->readReg(reg_r);
   
   // select write to memory or to register
   if (Memory::registerIs16bits(reg_w))
      memory->writeMem(memory->readReg(reg_w), written_val);
   else
      memory->writeReg(reg_w, written_val);
}


// ~~~~~~~~~~~~~~~~~


DoubleByteLD::DoubleByteLD (bool reg_at_read, bool register_a) :
      Instruction(3, 4 + (not register_a)),
      reg_read(reg_at_read),
      reg_a(register_a) {}

void DoubleByteLD::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint16_t address = (inst_second_byte << 8) | inst_first_byte;

   Memory::Register r = reg_a ? Memory::Register::A : Memory::Register::SP;

   if (reg_read)
      memory->writeMem(address, memory->readReg(r));
   else
      memory->writeReg(r, memory->readMem(address));
}

// ~~~~~~~~~~~~~~~~~

HLIterateLD::HLIterateLD (bool hl_read, bool hl_increment) :
      Instruction(1, 2),
      hl_r(hl_read),
      hl_inc(hl_increment) {}

void HLIterateLD::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint16_t address = memory->readReg(Memory::Register::HL);

   // move value
   if (hl_r)
      memory->writeReg(Memory::Register::A, memory->readMem(address));
   else
      memory->writeMem(address, memory->readReg(Memory::Register::A));
   
   // iterate
   address += (hl_inc ? +1 : -1);
   memory->writeReg(Memory::Register::HL, address);
}

// ~~~~~~~~~~~~~~~~~

FF00LD::FF00LD (bool register_a_read, bool use_register_c) :
      Instruction(2, 2 + (not use_register_c)),
      reg_a_read(register_a_read),
      use_c(use_register_c) {}

void FF00LD::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint16_t address = 0xff00 + (use_c ? memory->readReg(Memory::Register::C) :
                                        inst_first_byte);
   
   if (reg_a_read)
      memory->writeMem(address, memory->readReg(Memory::Register::A));
   else
      memory->writeReg(Memory::Register::A, memory->readMem(address));
}

// ~~~~~~~~~~~~~~~~~

Number16bitToRegLD::Number16bitToRegLD (Memory::Register reg) :
      Instruction(3, 3),
      reg_(reg) {}

void Number16bitToRegLD::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint16_t value = (inst_second_byte << 8) | inst_first_byte;

   memory->writeReg(reg_, value);
}

// ~~~~~~~~~~~~~~~~~

InstructionF8LD::InstructionF8LD () :
      Instruction(2, 3) {}

void InstructionF8LD::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   memory->writeFlag(Memory::Flag::Z_f, false);
   memory->writeFlag(Memory::Flag::N_f, false);

   uint16_t prev_sp_val = memory->readReg(Memory::Register::SP);
   uint16_t value = memory->readReg(Memory::Register::SP) +
                                                      int8_t(inst_first_byte);

   bool c_flag;
   if (int8_t(inst_first_byte) > 0) c_flag = (prev_sp_val > value);
   else                             c_flag = (prev_sp_val < value);

   memory->writeReg(Memory::Register::HL, value);
   memory->writeFlag(Memory::Flag::H_f,
                     (prev_sp_val & 0xff00) != (value & 0xff00));
   memory->writeFlag(Memory::Flag::C_f, c_flag);
}

// ~~~~~~~~~~~~~~~~~

PushLD::PushLD (Memory::Register reg) :
      Instruction(1, 4),
      reg_(reg) {}

void PushLD::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint16_t sp = memory->readReg(Memory::Register::SP);
   uint16_t value = memory->readReg(reg_);

   memory->writeMem(sp--, value >> 8);
   memory->writeMem(sp--, value & 0x00ff);

   memory->writeReg(Memory::Register::SP, sp);
}

// ~~~~~~~~~~~~~~~~~

PopLD::PopLD (Memory::Register reg) :
      Instruction(1, 3),
      reg_(reg) {}

void PopLD::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint16_t sp = memory->readReg(Memory::Register::SP);
   uint16_t value = memory->readMem(++sp);
   value = (memory->readMem(++sp) << 8) | value;

   memory->writeReg(reg_, value);
   memory->writeReg(Memory::Register::SP, sp);
}