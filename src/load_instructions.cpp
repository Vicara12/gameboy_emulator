#include "load_instructions.h"


NormalLD::NormalLD (Memory::Register reg_write,
                    Memory::Register reg_read,
                    bool immediate_8bit) :
      Instruction(1 + immediate_8bit, 1,
                  "LD    " + Memory::regString(reg_write,
                     not (reg_read == Memory::Register::HL and
                        reg_write == Memory::Register::SP)) +
                  ", " + (immediate_8bit ? "d8" : Memory::regString(reg_read,
                     not (reg_read == Memory::Register::HL and
                        reg_write == Memory::Register::SP))),
                  "(load value from register " + Memory::regString(reg_read) +
                  " to register " + Memory::regString(reg_write) + ")"),
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
   uint8_t written_val = 0;
   uint16_t written_val_0xf9 = 0;

   // special case, instruction 0xf9
   bool instr_0xf9 = (reg_r == Memory::Register::HL and
                      reg_w == Memory::Register::SP);

   // select the source of the data to be written
   if (immediate_8bit_)
      written_val = inst_first_byte;
   else if (Memory::registerIs16bits(reg_r))
   {
      if (instr_0xf9)
         written_val_0xf9 = memory->readReg(reg_r);
      else
         written_val = memory->readMem(memory->readReg(reg_r));
   }
   else
      written_val = memory->readReg(reg_r);
   
      
   // select write to memory or to register
   if (Memory::registerIs16bits(reg_w))
   {
      if (instr_0xf9)
         memory->writeReg(reg_w, written_val_0xf9);
      else
         memory->writeMem(memory->readReg(reg_w), written_val);
   }
   else
      memory->writeReg(reg_w, written_val);
}


// ~~~~~~~~~~~~~~~~~


DoubleByteLD::DoubleByteLD (bool reg_at_read, bool register_a) :
      Instruction(3, 4 + (not register_a), "", ""),
      reg_read(reg_at_read),
      reg_a(register_a)
{
   std::string reg_str = (register_a ? "A" : "SP");

   Instruction::instr_name_ = "LD    " + (reg_at_read ? "(a16)" : reg_str) + 
                              ", " + (reg_at_read ? reg_str : "(a16)");
   
   Instruction::verbose_name_ = "(load value at " +
                                (reg_at_read ? "register " + reg_str :
                                          "memory location indicated by a16") +
                                 " to " + (reg_at_read ? "memory location indicated by a16"
                                  : "register " + reg_str) + ")";
}


void DoubleByteLD::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint16_t address = (inst_second_byte << 8) | inst_first_byte;

   Memory::Register r = reg_a ? Memory::Register::A : Memory::Register::SP;

   if (reg_read)
   {
      // if SP is written to memory, writte LB at SP address and UB to address+1
      if (not reg_a)
      {
         uint16_t val = memory->readReg(r);
         memory->writeMem(address,   uint8_t(val));      // LB
         memory->writeMem(address+1, uint8_t(val >> 8)); // UB
      }
      else
         memory->writeMem(address, memory->readReg(r));
   }
   else
      memory->writeReg(r, memory->readMem(address));
}

// ~~~~~~~~~~~~~~~~~

HLIterateLD::HLIterateLD (bool hl_read, bool hl_increment) :
      Instruction(1, 2, "", ""),
      hl_r(hl_read),
      hl_inc(hl_increment)
{
   std::string hl_str = (hl_increment ? "(HL+)" : "(HL-)");

   Instruction::instr_name_ = "LD    " + (hl_read ? "A" : hl_str) + ", " +
                              (hl_read ? hl_str :  "A");
   Instruction::verbose_name_ = "(load value from " +
                                std::string(hl_read ? "(HL)" : "A") +
                                " to " + (hl_read ? "(HL)" : "A") + " and then "
                                + (hl_increment ? "increment" : "decrement") +
                                "HL)";
}


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
      Instruction(2, 2 + (not use_register_c), "", ""),
      reg_a_read(register_a_read),
      use_c(use_register_c)
{
   std::string reg_str = (use_register_c ? "(C)" : "(a8)");
   std::string verbose_str = (use_register_c ? "0xFF00 plus register C" :
                                               "0xFF00 plus a8 value");

   Instruction::instr_name_ = "LD" + std::string(use_register_c ? " " : "H") +
                              "   " + (register_a_read ? reg_str : "A") + ", " +
                              (register_a_read ? "A" : reg_str);
   Instruction::verbose_name_ = "(load value from " +
                                (register_a_read ? "register A" : verbose_str) +
                                " to " +
                                (register_a_read ? verbose_str : "register A") +
                                ")";
}


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
      Instruction(3, 3, "LD    " + Memory::regString(reg, false) + ", d16",
                  "(load value from a16 to register " + 
                  Memory::regString(reg) + ")"),
      reg_(reg) {}


void Number16bitToRegLD::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint16_t value = (inst_second_byte << 8) | inst_first_byte;

   memory->writeReg(reg_, value);
}

// ~~~~~~~~~~~~~~~~~

InstructionF8LD::InstructionF8LD () :
      Instruction(2, 3, "LD    HL, SP+r8",
                  "(load value of SP plus r8 signed value into HL register)") {}


void InstructionF8LD::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint16_t sp = memory->readReg(Memory::Register::SP);
   int8_t r8 = int8_t(inst_first_byte);

   memory->writeReg(Memory::Register::HL, sp + r8);
   memory->writeFlag(Memory::Flag::Z_f, 0);
   memory->writeFlag(Memory::Flag::N_f, 0);

   bool h_flag;
   if (r8 < 0)
      h_flag = (int(sp & 0x0fff) + r8 < 0);
   else
      h_flag = ((sp & 0x0fff) + r8 > 0x0fff);
   memory->writeFlag(Memory::Flag::H_f, h_flag);

   memory->writeFlag(Memory::Flag::C_f, uint32_t(int(sp) + r8) & 0xffff0000);
}

// ~~~~~~~~~~~~~~~~~

PushLD::PushLD (Memory::Register reg) :
      Instruction(1, 4,"PUSH  " + Memory::regString(reg, false),
                  "(push the value of " + Memory::regString(reg) + "into stack)"),
      reg_(reg) {}


void PushLD::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint16_t sp = memory->readReg(Memory::Register::SP);
   uint16_t value = memory->readReg(reg_);

   memory->writeMem(--sp, value >> 8);
   memory->writeMem(--sp, value & 0x00ff);

   memory->writeReg(Memory::Register::SP, sp);
}

// ~~~~~~~~~~~~~~~~~

PopLD::PopLD (Memory::Register reg) :
      Instruction(1, 3, "POP   " + Memory::regString(reg, false),
                  "(pop the value of " + Memory::regString(reg) + "from stack)"),
      reg_(reg) {}


void PopLD::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint16_t sp = memory->readReg(Memory::Register::SP);
   uint16_t value = memory->readMem(sp++);
   value = (memory->readMem(sp++) << 8) | value;

   memory->writeReg(reg_, value);
   memory->writeReg(Memory::Register::SP, sp);
}