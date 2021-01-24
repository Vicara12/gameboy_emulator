
#include "cpu.h"

CPU::CPU () :
   clock(0),
   verbose_debug(false),
   disp_reg_debug(false)
{
   memory = Memory::getInstance();

   initInstructionVectors();
}

void CPU::changeDebugMode (bool enable,
                           bool registers_and_flags,
                           bool verbose,
                           bool display_registers)
{
   memory->changeDebugMode(enable, registers_and_flags);
   debug_mode = enable;
   verbose_debug = verbose;
}

void CPU::displayInstructionSet (bool verbose)
{
   for (int i = 0; i < 0x100; i++)
   {
      std::cout << Memory::getHex(i) << "  " << instruction_set[i]->name();
      if (verbose) std::cout << "\t" << instruction_set[i]->description();
      std::cout << std::endl;
   }

   std::cout << "\n\n";

   for (int i = 0; i < 0x100; i++)
   {
      std::cout << Memory::getHex(i) << "  " << CB_subset[i]->name();
      if (verbose) std::cout << "\t" << CB_subset[i]->description();
      std::cout << std::endl;
   }
}

void CPU::disasemble (uint16_t from, uint16_t to, bool verbose) const
{
   for (uint16_t pc = from; pc <= to;)
   {
      uint16_t opcode = memory->readMem(pc, true);
      bool cb_instr = opcode == 0xcb;

      if (cb_instr)
         opcode = memory->readMem(++pc, true);
      
      Instruction* instr = cb_instr ? instruction_set[opcode] : CB_subset[opcode];

      std::cout << "pc " << Memory::getHex(pc);
      std::cout << ": " << Memory::getHex(opcode);
      if (cb_instr) std::cout << " (CB)";
      if (verbose)
      {
         std::cout << "(" << instr->getByteSize() << ", ";
         if (instr->extraCycleTime() != 0)
            std::cout << instr->extraCycleTime() << "\\";
         std::cout << instr->getCycleLenght() << ")";
      }
      std::cout << "\t" << instr->name();

      if (verbose)
      {
         if (instr->getByteSize() > 1 and not cb_instr)
         {
            std::cout << "\t(" << Memory::getHex(memory->readMem(pc+1, true));
            if (instr->getByteSize() > 2)
               std::cout << " | " << Memory::getHex(memory->readMem(pc+2, true));
            std::cout << ")  ";
         }

         std::cout << instr->description();
      }

      std::cout << std::endl;

      pc += instr->getByteSize();
   }
}

void CPU::executeInstruction (uint8_t opcode,
                              uint8_t first_byte,
                              uint8_t second_byte,
                              bool CB,
                              bool ignore_CPU_disable)
{
   // check if CPU is halted or stopped
   if (not ignore_CPU_disable and (memory->cpuHalted() or memory->cpuStopped()))
   {
      clock++;

      if (debug_mode)
      {
         std::cout << "cpu is ";
         std::cout << (memory->cpuStopped() ? "stopped" : "halted");
         std::cout << ", skipping instruction execution" << std::endl;
         std::cout << "clk: " << clock << std::endl;
      }
      
      return;
   }

   // check if interrupt status must change
   if (memory->checkInterruptChange() ==
                     Memory::InterruptionStatusChange::ENABLE)
      memory->changeIntEnabled(true);

   else if (memory->checkInterruptChange() ==
                     Memory::InterruptionStatusChange::DISABLE)
      memory->changeIntEnabled(false);
   
   // get the instruction
   Instruction* instruction = CB ? CB_subset[opcode] : instruction_set[opcode];
   
   // increment PC
   uint16_t pc = memory->readReg(Memory::Register::PC);
   pc += instruction->getByteSize();
   memory->writeReg(Memory::Register::PC, pc);

   // if in debug mode, output debug info
   if (debug_mode)
   {
      std::cout << "clk : " << clock << std::endl;
      std::cout << (CB ? "exCB: " : "ex:   ");
      std::cout << Memory::getHex(opcode) << "  ";
      std::cout << instruction->name();
      if (verbose_debug) std::cout << "\t" << instruction->description() << "\n";
   }

   instruction->execute(first_byte, second_byte);

   // increment clock
   if (instruction->longPathTaken())
      clock += instruction->extraCycleTime();
   else
      clock += instruction->getCycleLenght();
}

void CPU::enableCPU ()
{
   if (memory->cpuStopped())     memory->changeCpuStop(false);
   else if (memory->cpuHalted()) memory->changeCpuHalt(false);
}

void CPU::displayInstructionInfo (uint8_t opcode, bool cb_subset, bool verbose)
{
   // get pointer to instruction
   Instruction* instruction = (cb_subset ?   CB_subset[opcode] :
                                             instruction_set[opcode]);

   std::cout << Memory::getHex(opcode);

   if (cb_subset)
      std::cout << " (CB)";

   // display size and time duration of instruction
   std::cout << " " << instruction->getByteSize();
   std::cout << " " << (instruction->getCycleLenght()*4);

   if (instruction->getCycleLenght()*4 < 10)
      std::cout << " ";

   std::cout << "   " << instruction->name();

   if (verbose)
      std::cout << "  " << instruction->description();
   
   std::cout << std::endl;
}