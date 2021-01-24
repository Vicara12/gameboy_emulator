#ifndef GBMEMORY_H_
#define GBMEMORY_H_

#include <cstdint>
#include <exception>
#include <iostream>

class Memory
{
public:

  enum Register {A, F, B, C, D, E, H, L, AF, BC, DE, HL, SP, PC};
  enum Flag {Z_f, N_f, H_f, C_f};
  enum InterruptionStatusChange {ENABLE, DISABLE, NONE};

  // returns a string with the number in hexadecimal, mainly for debugging
  static std::string getHex (int n, int min_size = 2);

  // returns a string with the letter of the register
  static std::string regString (Register reg, bool pointer = true);

  // returns a string with the code of the flag
  static std::string flagString (Flag flag);

  static bool registerIs16bits (Register reg);

  static Memory* getInstance();

  // debug mode indicates when a memory location is accessed and gives general
  // information on the operations performed in memory
  void changeDebugMode (bool debug, bool show_flags_and_regs);

  void loadROM (std::string rom_file_name);

  // if silent = true, the operation wont output in debug mode

  uint8_t readMem (uint16_t address, bool silent = false) const;
  void writeMem (uint16_t address, uint8_t value, bool silent = false);

  // if the register is 8 bit long, only the lower byte will be used
  uint16_t readReg (Register reg, bool silent = false) const;
  // if the register is 8 bit long, only the lower byte will be written
  void writeReg (Register reg, uint16_t value, bool silent = false);

  bool readFlag (Flag f, bool silent = false) const;
  void writeFlag (Flag f, bool value, bool silent = false);

  bool cpuHalted () const;
  void changeCpuHalt (bool halted);

  bool cpuStopped () const;
  void changeCpuStop (bool stop);

  // if a interruption change status was activated, it is deactivated on read
  bool intEnabled () const;
  void changeIntEnabled (bool int_enabled);

  // set or check if an interruption status (enabled/disabled) is needed
  // (useful because EI and DI instructions take action in the next instruction)
  InterruptionStatusChange checkInterruptChange () const;
  void changeInterruptChange (InterruptionStatusChange new_status);

  // displays all the registers
  void outputMemoryStatus (bool verbose = false) const;

  // displays the values in the internal memory (not including the value in "to")
  void displayMemoryChunk (int from, int to) const;

private:

   Memory();

   static Memory* instance;

   bool debug_mode, debug_flags_regs;

   // some variables that indicate the state of the CPU
   bool cpu_halted;
   bool cpu_stopped;
   bool cpu_int_enabled;
   InterruptionStatusChange cpu_int_status_cahnge_needed;
   
   // CPU registers;
   uint16_t reg_af;
   uint16_t reg_bc;
   uint16_t reg_de;
   uint16_t reg_hl;
   uint16_t reg_pc;
   uint16_t reg_sp;

  // internal memory array
   uint8_t internal_mem [0x10000];
};


// exceptions raised by the class

struct CanNotOpenFile : public std::exception
{
  const char* what () const throw ()
  {
    return "the file could not be found or openned";
  }
};

#endif