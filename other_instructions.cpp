
#include "other_instructions.h"

NOP::NOP () :
      Instruction(1, 1, "NOP \t", "(no operation instruction)") {}

void NOP::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   //
}

// ~~~~~~~~~~~~~~~~~

Halt::Halt () :
      Instruction(1, 1, "HALT \t", "(stop CPU until an interrupt is received)") {}

void Halt::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   memory->changeCpuHalt(true);
}

// ~~~~~~~~~~~~~~~~~

Stop::Stop () :
      Instruction(1, 1, "STOP \t", "(stop CPU until a joypad input)") {}

void Stop::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   memory->changeCpuStop(true);
}

// ~~~~~~~~~~~~~~~~~

InterruptionED::InterruptionED (bool enable) :
      Instruction(1, 1, (enable ? "EI" : "DI") + std::string(" \t"),
                  "(" + std::string(enable ? "enable" : "disable") + " interrupts)"),
      enable_(enable) {}

void InterruptionED::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   if (enable_)
      memory->changeInterruptChange(Memory::InterruptionStatusChange::ENABLE);
   else
      memory->changeInterruptChange(Memory::InterruptionStatusChange::DISABLE);
}

// ~~~~~~~~~~~~~~~~~

JumpCall::JumpCall (bool call, bool relative, Source source, Type type) :
      Instruction(1 + (source == r8) + 2*(source == a16),
                  call ? 3 : 1 + (source == r8) + 2*(source == a16), "", ""),
      call_(call),
      relative_(relative),
      alternative_path_taken(false),
      alternative_path_cpu_cycles(call ? 6 : 1 + 2*(source == r8) + 3*(source == a16)),
      source_(source),
      type_(type)
{
   std::string source_str, type_str, src_str, tp_str;

   switch (source_)
   {
      case r8: {source_str = "r8"; src_str = "r8"; break;}
      case a16: {source_str = "a16"; src_str = "a16"; break;}
      case HL: {source_str = "16bit value stored at addres HL"; src_str = "HL"; break;}
   }

   switch (type_)
   {
      case NZ: {type_str = "zero flag is clear"; tp_str = "NZ"; break;}
      case Z: {type_str = "zero flag is set"; tp_str = "Z"; break;}
      case NC: {type_str = "substraction flag is clear"; tp_str = "NC"; break;}
      case C: {type_str = "substraction flag is set"; tp_str = "C"; break;}
      case INCONDITIONAL: {type_str = "true"; tp_str = ""; break;}
   }

   if (call)
   {
      Instruction::instr_name_ = "CALL \t" + tp_str + ", " + src_str;
      Instruction::verbose_name_ = "(save PC in stack and set it to "
                                   + source_str + " if " + source_str + ")";
   }
   else
   {
      Instruction::instr_name_ = (relative ? "JR" : "JP") + std::string(" \t") +
                                 tp_str + ", " + src_str;
      Instruction::verbose_name_ = std::string("(") + 
                                 (relative ? "add to PC the value" : "set PC") +
                                 " at address " + source_str + " if " +
                                 source_str + ")";
   }
   
}

void JumpCall::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   bool jump = false;
   
   // decide if jump
   switch (type_)
   {
      case INCONDITIONAL: {jump = true; break;}
      case NZ: {jump = (memory->readFlag(Memory::Flag::Z_f) == false); break;}
      case Z: {jump = (memory->readFlag(Memory::Flag::Z_f) == true); break;}
      case NC: {jump = (memory->readFlag(Memory::Flag::C_f) == false); break;}
      case C: {jump = (memory->readFlag(Memory::Flag::C_f) == true); break;}
   }

   if (jump)
   {
      int value = 0;
      uint16_t pc = memory->readReg(Memory::Register::PC);

      // if the instruction is call, save pc to stack first
      if (call_)
      {
         uint16_t sp = memory->readReg(Memory::Register::SP);

         memory->writeMem(--sp, pc >> 8);
         memory->writeMem(--sp, pc);

         memory->writeReg(Memory::Register::SP, sp);
      }

      switch (source_)
      {
         case r8: {value = int8_t(inst_first_byte); break;}
         case a16: {value = uint16_t((inst_second_byte << 8) | inst_first_byte); break;}
         case HL: {value = memory->readReg(Memory::Register::HL); break;}
      }

      if (relative_) pc += value;
      else           pc = value;

      memory->writeReg(Memory::Register::PC, pc);
   }
   
   alternative_path_taken = jump;
}

unsigned JumpCall::extraCycleTime () const
{
   return alternative_path_cpu_cycles;
}

bool JumpCall::longPathTaken () const
{
   return alternative_path_taken;
}

// ~~~~~~~~~~~~~~~~~

Return::Return (bool reti, Source source, Type type) :
      Instruction(1, 2, "", ""),
      reti_(reti),
      jump(false),
      jump_cpu_cycles(type == INCONDITIONAL ? 16 : 20),
      source_(source),
      type_(type)
{
   std::string source_str, type_str, cond_str;

   switch (source_)
   {
      case _00H: {source_str = "0x00"; break;}
      case _08H: {source_str = "0x08"; break;}
      case _10H: {source_str = "0x10"; break;}
      case _18H: {source_str = "0x18"; break;}
      case _20H: {source_str = "0x20"; break;}
      case _28H: {source_str = "0x28"; break;}
      case _30H: {source_str = "0x30"; break;}
      case _38H: {source_str = "0x30"; break;}
      case   SP: {source_str = "SP";   break;}
   }

   switch (type_)
   {
      case NZ: {cond_str = "NZ"; type_str = "zero flag is clear"; break;}
      case Z:  {cond_str = "Z";  type_str = "zero flag is set"; break;}
      case NC: {cond_str = "NC"; type_str = "substraction flag is clear"; break;}
      case C:  {cond_str = "C";  type_str = "substraction flag is set"; break;}
      case INCONDITIONAL: {type_str = "true"; break;}
   }

   if (reti_)
   {
      Instruction::instr_name_ = "RETI \t";
      Instruction::verbose_name_ = "(pop PC from the stack and activate interrupts)";
   }
   else if (type_ == INCONDITIONAL and source_ == SP)
   {
      Instruction::instr_name_ = "RET \t";
      Instruction::verbose_name_ = "(pop PC from stack)";
   }
   else if (type_ == INCONDITIONAL)
   {
      Instruction::instr_name_ = "RST \t" + source_str;
      Instruction::verbose_name_ = "(set PC to address "+ source_str +")";
   }
   else
   {
      Instruction::instr_name_ = "RET \t" + cond_str;
      Instruction::verbose_name_ = "(pop PC from stack if " + type_str + ")";
   }
}


void Return::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   jump = false;

   // decide if jump
   switch (type_)
   {
      case INCONDITIONAL: {jump = true; break;}
      case NZ: {jump = (memory->readFlag(Memory::Flag::Z_f) == false); break;}
      case Z: {jump = (memory->readFlag(Memory::Flag::Z_f) == true); break;}
      case NC: {jump = (memory->readFlag(Memory::Flag::C_f) == false); break;}
      case C: {jump = (memory->readFlag(Memory::Flag::C_f) == true); break;}
   }

   if (jump)
   {
      uint16_t address = 0;

      switch (source_)
      {
         case _00H: {address = 0x00; break;}
         case _08H: {address = 0x08; break;}
         case _10H: {address = 0x10; break;}
         case _18H: {address = 0x18; break;}
         case _20H: {address = 0x20; break;}
         case _28H: {address = 0x28; break;}
         case _30H: {address = 0x30; break;}
         case _38H: {address = 0x38; break;}
         // pop address from stack
         case SP:
         {
            uint16_t sp = memory->readReg(Memory::Register::SP);

            address = memory->readMem(sp--);
            address = (memory->readMem(sp--) << 8) | address;

            memory->writeReg(Memory::Register::SP, sp);
         }
      }

      memory->writeReg(Memory::Register::PC, address);

      // enable interrupts on next instruction if instruction is reti
      if (reti_)
         memory->changeInterruptChange(Memory::InterruptionStatusChange::ENABLE);
   }
}

unsigned Return::extraCycleTime () const
{
   return jump_cpu_cycles;
}

bool Return::longPathTaken () const
{
   return jump;
}