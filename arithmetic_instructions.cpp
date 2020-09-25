#include "arithmetic_instructions.h"


IncAI::IncAI (Memory::Register reg) :
      Instruction(1, 1, "INC \t" + Memory::regString(reg),
                  "(increment register " + Memory::regString(reg)),
      reg_(reg) {}

void IncAI::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint8_t reg_val = Memory::registerIs16bits(reg_) ?
                           memory->readMem(memory->readReg(reg_)) :
                           memory->readReg(reg_);

   memory->writeReg(reg_, ++reg_val);

   // write flags
   memory->writeFlag(Memory::Flag::Z_f, reg_val == 0);
   memory->writeFlag(Memory::Flag::N_f, false);
   memory->writeFlag(Memory::Flag::H_f, (reg_val & 0x0f) == 0);
}

// ~~~~~~~~~~~~~~~~~

DecAI::DecAI (Memory::Register reg) :
      Instruction(1, 1, "DEC \t" + Memory::regString(reg),
                  "(decrement register " + Memory::regString(reg)),
      reg_(reg) {}

void DecAI::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint8_t reg_val = Memory::registerIs16bits(reg_) ?
                           memory->readMem(memory->readReg(reg_)) :
                           memory->readReg(reg_);
   memory->writeReg(reg_, --reg_val);

   // write flags
   memory->writeFlag(Memory::Flag::Z_f, reg_val == 0);
   memory->writeFlag(Memory::Flag::N_f, true);
   memory->writeFlag(Memory::Flag::H_f, (reg_val & 0x0f) == 0x0f);
}

// ~~~~~~~~~~~~~~~~~

ArithmeticOperationAI::ArithmeticOperationAI (Memory::Register reg,
                                              Operation op,
                                              bool use_reg) :
      Instruction(1 + (not use_reg), 1 + (not use_reg) +
                        Memory::registerIs16bits(reg_), "", ""),
      reg_(reg),
      op_(op),
      use_reg_(use_reg)
{
   std::string opname, long_opname;

   switch (op_)
   {
      case ADD: {opname = "ADD"; long_opname = "add"; break;}
      case ADC: {opname = "ADC"; long_opname = "add"; break;}
      case SUB: {opname = "SUB"; long_opname = "substract"; break;}
      case SBC: {opname = "SBC"; long_opname = "substract"; break;}
      case AND: {opname = "AND"; long_opname = "bitwise AND between"; break;}
      case XOR: {opname = "XOR"; long_opname = "bitwise XOR between"; break;}
      case OR: {opname = "OR"; long_opname = "bitwise OR between"; break;}
      case CP: {opname = "CP"; long_opname = "compare reg A and"; break;}
   }

   Instruction::instr_name_ = opname + " \t" +
                        ((op_ == ADD or op == ADC or op_ == SBC) ? "A, " : "") +
                        (use_reg ? Memory::regString(reg_) : "r8");
   
   Instruction::verbose_name_ = "(" + long_opname + " registers A and " +
                                 (use_reg ? Memory::regString(reg_) : "d8") +
                                 (op_ == ADC or op_ == SBC ? " with carry" : "")
                                 + ")"; 
}

void ArithmeticOperationAI::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint8_t reg_a = memory->readReg(Memory::Register::A);
   uint8_t original_reg_a = reg_a;
   uint8_t value;
   bool carry = memory->readFlag(Memory::Flag::C_f);

   // get the value
   if (not use_reg_)
      value = inst_first_byte;
   else if (Memory::registerIs16bits(reg_))
      value = memory->readMem(memory->readReg(reg_));
   else
      value = memory->readReg(reg_);
   
   // perform the operation
   switch (op_)
   {
      case ADD: {reg_a = reg_a + value; break;}
      case ADC: {reg_a = reg_a + value + carry; break;}
      case SUB: {reg_a = reg_a - value; break;}
      case SBC: {reg_a = reg_a - value - carry; break;}
      case AND: {reg_a = reg_a & value; break;}
      case XOR: {reg_a = reg_a ^ value; break;}
      case OR: {reg_a = reg_a | value; break;}
      case CP: {reg_a = reg_a - value; break;}
   }

   // save results
   if (op_ != CP) memory->writeReg(Memory::Register::A, reg_a);

   // set flags
   memory->writeFlag(Memory::Flag::Z_f, reg_a == 0);
   memory->writeFlag(Memory::Flag::N_f, op_ == SUB or op_ == SBC or op_ == CP);
   
   bool h_flag;
   if (op_ == AND)
      h_flag = true;
   else if (op_ == XOR or op_ == OR)
      h_flag = false;
   else if (op_ == SUB or op_ == SBC or op_ == CP)
      h_flag = ((reg_a & 0x0f) - (value & 0x0f) - (op_ == SBC)) > 0x0f;
   else
      h_flag = ((reg_a & 0x0f) + (value & 0x0f) + (op_ == ADC)) > 0x0f;
   memory->writeFlag(Memory::Flag::H_f, h_flag);

   bool c_flag = false;
   if (op_ == SUB or op_ == SBC or op_ == CP)
      h_flag = reg_a > original_reg_a;
   else if (op_ == ADD or op_ == ADC)
      h_flag = reg_a < original_reg_a;
   memory->writeFlag(Memory::Flag::C_f, c_flag);
}

// ~~~~~~~~~~~~~~~~~

DecimalAdjustAI::DecimalAdjustAI () :
      Instruction(1, 1, "DAA \t", "(decimal adjust register A)") {}

void DecimalAdjustAI::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint8_t reg_a = memory->readReg(Memory::Register::A);
   bool c_flag_set = false;
   bool c_flag = memory->readFlag(Memory::Flag::C_f);
   bool n_flag = memory->readFlag(Memory::Flag::N_f);
   bool h_flag = memory->readFlag(Memory::Flag::H_f);

   if (n_flag)
   {
      if (c_flag) reg_a -= 0x60;
      if (h_flag) reg_a -= 0x06;
   }
   else
   {
      if (c_flag or reg_a > 0x99)
      {
         reg_a += 0x60;
         c_flag_set = true;
      }
      
      if (h_flag or (reg_a & 0x0f) > 0x09)
         reg_a += 0x06;
   }
   

   memory->writeReg(Memory::Register::A, reg_a);
   memory->writeFlag(Memory::Flag::Z_f, reg_a == 0);
   memory->writeFlag(Memory::Flag::H_f, 0);
   memory->writeFlag(Memory::Flag::C_f, c_flag_set);
}

// ~~~~~~~~~~~~~~~~~

CompAAI::CompAAI () :
      Instruction(1, 1, "CPL \t", "(flip bits of register A)") {}

void CompAAI::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint8_t reg_a = memory->readReg(Memory::Register::A);

   memory->writeReg(Memory::Register::A, reg_a ^ 0xff);
   memory->writeFlag(Memory::Flag::N_f, 1);
   memory->writeFlag(Memory::Flag::H_f, 1);
}

// ~~~~~~~~~~~~~~~~~

ChangeCFlagAI::ChangeCFlagAI (bool complement) :
      Instruction(1, 1, (complement ? "CCF \t" : "SCF \t"),
                  "(" + std::string(complement ? "flip" : "set") + "carry flag)"),
      complement_ (complement) {}

void ChangeCFlagAI::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   bool c_flag = memory->readFlag(Memory::Flag::C_f);

   memory->writeReg(Memory::Register::A, complement_ ? not c_flag : true);
   memory->writeFlag(Memory::Flag::N_f, 0);
   memory->writeFlag(Memory::Flag::H_f, 0);
}
// ~~~~~~~~~~~~~~~~~

IncDec16BitAI::IncDec16BitAI (Memory::Register reg, bool inc) :
      Instruction(1, 2, (inc ? "INC \t" : "DEC \t") +
                     Memory::regString(reg, false),
                  "(" + std::string(inc ? "increment" : "decrement") +
                     " register " + Memory::regString(reg, false)),
      inc_ (inc),
      reg_ (reg) {}

void IncDec16BitAI::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint16_t reg = memory->readReg(reg_);

   memory->writeReg(reg_, inc_ ? reg++ : reg--);
}

// ~~~~~~~~~~~~~~~~~

Add16BitsAI::Add16BitsAI (Memory::Register reg) :
      Instruction(1, 2, "ADD \tHL, " + Memory::regString(reg, false),
                  "(add registers HL and " + Memory::regString(reg, false)),
      reg_ (reg) {}

void Add16BitsAI::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint16_t reg = memory->readReg(reg_);
   uint16_t hl = memory->readReg(Memory::Register::HL);

   memory->writeReg(Memory::Register::HL, reg + hl);
   memory->writeFlag(Memory::Flag::N_f, 0);
   memory->writeFlag(Memory::Flag::H_f, ((reg & 0xff) + (hl & 0xff)) > 0xff);
   memory->writeFlag(Memory::Flag::C_f, (unsigned(reg) + unsigned(hl)) > 0xffff);
}

// ~~~~~~~~~~~~~~~~~

ADDr8SPAI::ADDr8SPAI () :
      Instruction(2, 4, "ADD \tSP, r8",
                  "(add signed r8 value to SP register)") {}

void ADDr8SPAI::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   uint16_t sp = memory->readReg(Memory::Register::SP);
   int8_t r8 = int8_t(inst_first_byte);

   memory->writeReg(Memory::Register::SP, sp + r8);
   memory->writeFlag(Memory::Flag::Z_f, 0);
   memory->writeFlag(Memory::Flag::N_f, 0);
   memory->writeFlag(Memory::Flag::H_f, ((sp & 0xff) + r8) > 0xff);
   memory->writeFlag(Memory::Flag::C_f, int(sp) + r8 > 0xffff);
}