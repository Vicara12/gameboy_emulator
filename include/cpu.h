#ifndef CPU_H_
#define CPU_H_

#include <cstdint>

#include "gbmemory.h"
#include "instructions.h"


class CPU
{
public:

   CPU ();

   // if debug mode is enabled, the access to r/w to memory will be displayed
   // to screen as well as interrupt e/d
   // if register_and_flags is also true this will also display information
   // for the register and flags access
   void changeDebugMode (bool enable,
                         bool registers_and_flags = false,
                         bool verbose = false,
                         bool display_registers = false);

   // displays the entire instruction set and shift subset on screen
   void displayInstructionSet (bool verbose);

   // displays detailed information about the selected instruction
   void displayInstructionInfo (uint8_t opcode, bool cb_subset, bool verbose);

   void disasemble (uint16_t from, uint16_t to, bool verbose) const;

   void executeInstruction (uint8_t opcode,
                            uint8_t first_byte,
                            uint8_t second_byte,
                            bool CB_subset,
                            bool ignore_CPU_disable = false);
   
   // disable halt or stop
   void enableCPU();

private:

   void initInstructionVectors ();

   unsigned long clock;
   Memory* memory;
   bool debug_mode, verbose_debug, disp_reg_debug;

   // instruction for each address
   Instruction* instruction_set [0x100];

   // subset for the instruction 0xCB
   Instruction* CB_subset [0x100];

};

#endif