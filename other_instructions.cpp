
#include "other_instructions.h"

NOP::NOP () :
      Instruction(1, 1) {}

void NOP::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   //
}

// ~~~~~~~~~~~~~~~~~

Halt::Halt () :
      Instruction(1, 1) {}

void Halt::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   memory->changeCpuHalt(true);
}

// ~~~~~~~~~~~~~~~~~

Stop::Stop () :
      Instruction(1, 1) {}

void Stop::execute (uint8_t inst_first_byte, uint8_t inst_second_byte)
{
   memory->changeCpuStop(true);
}

// ~~~~~~~~~~~~~~~~~

InterruptionED::InterruptionED (bool enable) :
      Instruction(1, 1),
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
                  call ? 3 : 1 + (source == r8) + 2*(source == a16) ),
      call_(call),
      relative_(relative),
      alternative_path_taken(false),
      alternative_path_cpu_cycles(call ? 6 : 1 + 2*(source == r8) + 3*(source == a16)),
      source_(source),
      type_(type) {}

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
      int value;
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
      Instruction(1, 2),
      reti_(reti),
      jump(false),
      jump_cpu_cycles(source == INCONDITIONAL ? 16 : 20),
      source_(source),
      type_(type) {}


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
      uint16_t address;

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