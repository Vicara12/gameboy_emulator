#include "gbmemory.h"
#include "bootrom.h"

#include <fstream>

Memory* Memory::instance = new Memory();

Memory* Memory::getInstance ()
{
   return instance;
}

bool Memory::registerIs16bits (Register reg)
{
   if (reg == AF or reg == BC or reg == DE or reg == HL or
       reg == SP or reg == PC)
      return true;
   else
      return false;
}

Memory::Memory () :
      debug_mode(false),
      debug_flags_regs(false),
      cpu_halted(false),
      cpu_stopped(false),
      cpu_int_enabled(true),
      cpu_int_status_cahnge_needed(NONE)
{
   // copy the contents of the startup program to main memory (0x00 to 0xff)
   for (int i = 0; i < sizeof(BOOT_ROM_CONTENT); i++)
   {
      internal_mem[i] = BOOT_ROM_CONTENT[i];
   }
}

void Memory::changeDebugMode (bool debug, bool show_flags_and_regs)
{
   debug_mode = debug;
   debug_flags_regs = show_flags_and_regs;
}

void Memory::loadROM (std::string rom_file_name)
{
   // 0x8000 is 32kB, the size of the ROM and RAM
   uint8_t main_ROM_content [0x8000];
   std::ifstream ROM_file(rom_file_name, std::ios::in | std::ios::binary);

   if (not ROM_file) throw CanNotOpenFile();
   ROM_file.read((char *) main_ROM_content, sizeof(main_ROM_content));
   ROM_file.close();
   if (not ROM_file.good()) throw CanNotOpenFile();

   // addresses from 0x000 to 0xff hold the boot up code
   for (int i = 0; i < sizeof(main_ROM_content); i++)
      internal_mem[i] = main_ROM_content[i];
}

std::string Memory::getHex (int n, int min_size)
{
   char buff[32];
   buff[0] = '0';
   buff[1] = 'x';
   int lenght = 2;

   // if the number is 0, fill the string with zeros
   if (n == 0)
   {
      for (int i = 0; i < min_size; i++)
         buff[2+i] = '0';
      
      buff[3+min_size] = 0;
      return std::string(buff);
   }

   // write the numbers backwards
   while (n > 0)
   {
      int digit = n%16;
      n /= 16;

      if (digit < 10) buff[lenght] = char(digit) + '0';
      else buff[lenght] = char(digit-10) + 'a';

      lenght++;
   }

   // add all the necessary zeros
   while (lenght < min_size+2)
   {
      buff[lenght] = '0';
      lenght++;
   }

   for (int i = 2; i < 2 + (lenght-2)/2; i++)
   {
      char temp = buff[i];
      buff[i] = buff[lenght - i + 1];
      buff[lenght - i + 1] = temp;
   }

   buff[lenght] = 0;

   return std::string(buff);
}

uint8_t Memory::readMem (uint16_t address, bool silent) const
{
   if (debug_mode and not silent)
      std::cout << "r m: [" << getHex(address) << "]" << std::endl;

   return internal_mem[address];
}

void Memory::writeMem (uint16_t address, uint8_t value, bool silent)
{
   if (debug_mode and not silent)
      std::cout << "W m: [" << getHex(address) << "] <- " << value << std::endl;

   internal_mem[address] = value;
}

uint16_t Memory::readReg (Register reg, bool silent) const
{
   if (debug_mode and debug_flags_regs and not silent)
      std::cout << "r r: " << regString(reg) << std::endl;

   switch (reg)
   {
      case A: return (0xff00 & reg_af) >> 8;
      case F: return 0x00ff & reg_af;
      case B: return (0xff00 & reg_bc) >> 8;
      case C: return 0x00ff & reg_bc;
      case D: return (0xff00 & reg_de) >> 8;
      case E: return 0x00ff & reg_de;
      case H: return (0xff00 & reg_hl) >> 8;
      case L: return 0x00ff & reg_hl;
      case AF: return reg_af;
      case BC: return reg_bc;
      case DE: return reg_de;
      case HL: return reg_hl;
      case SP: return reg_sp;
      case PC: return reg_pc;
   }
   return 0xffff;
}

void Memory::writeReg (Register reg, uint16_t value, bool silent)
{
   if (debug_mode and debug_flags_regs and not silent)
      std::cout << "w r: " << regString(reg) << " <- " << getHex(value) << std::endl;

   switch (reg)
   {
      case A: {reg_af = (reg_af & 0x00ff) | (value << 8); break;}
      case F: {reg_af = (reg_af & 0xff00) | (value & 0x00ff); break;}
      case B: {reg_bc = (reg_bc & 0x00ff) | (value << 8); break;}
      case C: {reg_bc = (reg_bc & 0xff00) | (value & 0x00ff); break;}
      case D: {reg_de = (reg_de & 0x00ff) | (value << 8); break;}
      case E: {reg_de = (reg_de & 0xff00) | (value & 0x00ff); break;}
      case H: {reg_hl = (reg_hl & 0x00ff) | (value << 8); break;}
      case L: {reg_hl = (reg_hl & 0xff00) | (value & 0x00ff); break;}
      case AF: {reg_af = value; break;}
      case BC: {reg_bc = value; break;}
      case DE: {reg_de = value; break;}
      case HL: {reg_hl = value; break;}
      case SP: {reg_sp = value; break;}
      case PC: {reg_pc = value; break;}
   }
}

bool Memory::readFlag (Flag f, bool silent) const
{
   if (debug_mode and debug_flags_regs and not silent)
      std::cout << "r f: " << flagString(f) << std::endl;

   if       (f == Z_f) return (reg_af & 0x0080) != 0;
   else if  (f == N_f) return (reg_af & 0x0040) != 0;
   else if  (f == H_f) return (reg_af & 0x0020) != 0;
   else                return (reg_af & 0x0010) != 0;
}

void Memory::writeFlag (Flag f, bool value, bool silent)
{
   if (debug_mode and debug_flags_regs and not silent)
   {
      std::cout << "w f: " << flagString(f) << " <- ";
      std::cout << (value ? "set" : "clear") << std::endl;
   }

   uint16_t mask = 0;

   switch (f)
   {
      case Z_f: {mask = 0x0080; break;}
      case N_f: {mask = 0x0040; break;}
      case H_f: {mask = 0x0020; break;}
      case C_f: {mask = 0x0010; break;}
   }

   if (value)  reg_af = reg_af | mask;
   else        reg_af = reg_af & (mask ^ 0xffff);
}

bool Memory::cpuHalted () const
{
   return cpu_halted;
}

void Memory::changeCpuHalt (bool halted)
{
   if (debug_mode)
      std::cout << (halted ? "cpu halted" : "cpu continue (h)") << std::endl;

   cpu_halted = halted;
}

bool Memory::cpuStopped () const
{
   return cpu_stopped;
}

void Memory::changeCpuStop (bool stop)
{
   if (debug_mode)
      std::cout << (stop ? "cpu stopped" : "cpu continue (s)") << std::endl;

   cpu_stopped = stop;
}

bool Memory::intEnabled () const
{
   return cpu_int_enabled;
}

void Memory::changeIntEnabled (bool int_enabled)
{
   if (debug_mode)
      std::cout << (int_enabled ? "int en" : "int dis") << std::endl;
   cpu_int_enabled = int_enabled;
   cpu_int_status_cahnge_needed = NONE;
}

Memory::InterruptionStatusChange Memory::checkInterruptChange () const
{
   return cpu_int_status_cahnge_needed;
}
void Memory::changeInterruptChange (InterruptionStatusChange new_status)
{
   cpu_int_status_cahnge_needed = new_status;
}

std::string Memory::regString (Register reg, bool pointer)
{
   switch (reg)
   {
      case A: return "A";
      case F: return "F";
      case B: return "B";
      case C: return "C";
      case D: return "D";
      case E: return "E";
      case H: return "H";
      case L: return "L";
      case AF: return (pointer ? "(AF)" : "AF");
      case BC: return (pointer ? "(BC)" : "BC");
      case DE: return (pointer ? "(DE)" : "DE");
      case HL: return (pointer ? "(HL)" : "HL");
      case SP: return (pointer ? "(SP)" : "SP");
      case PC: return (pointer ? "(PC)" : "PC");
   }

   return "???";
}

std::string Memory::flagString (Flag flag)
{
   switch (flag)
   {
      case Z_f: return "Z";
      case N_f: return "N";
      case H_f: return "H";
      case C_f: return "C";
   }

   return "???";
}

void Memory::outputMemoryStatus (bool verbose) const
{
   if (verbose)
   {
      std::cout << "- Debug mode:  " << (debug_mode ? "on" : "off") << "\n";
      std::cout << "- Debug flags: " << (debug_flags_regs ? "on" :
                                                            "off") << "\n";
      std::cout << "- CPU halted:  " << (cpu_halted ? "yes" : "no") << "\n";
      std::cout << "- CPU stopped: " << (cpu_stopped ? "yes" : "no") << "\n";
      std::cout << "- CPU int:     " << (cpu_int_enabled ? "enabled" :
                                                            "disabled") << "\n";
   }

   std::cout << "Flags: Z(" << ((reg_af & 0x80) != 0) <<
                ") N(" << ((reg_af & 0x40) != 0) <<
                ") H(" << ((reg_af & 0x20) != 0) <<
                ") C(" << ((reg_af & 0x10) != 0) << ")" << std::endl;
   std::cout << "A: " << getHex(readReg(Register::A, true)) << "    ";
   std::cout << "F: " << getHex(readReg(Register::F, true)) << std::endl;
   std::cout << "B: " << getHex(readReg(Register::B, true)) << "    ";
   std::cout << "C: " << getHex(readReg(Register::C, true)) << std::endl;
   std::cout << "D: " << getHex(readReg(Register::D, true)) << "    ";
   std::cout << "E: " << getHex(readReg(Register::E, true)) << std::endl;
   std::cout << "H: " << getHex(readReg(Register::H, true)) << "    ";
   std::cout << "L: " << getHex(readReg(Register::L, true)) << std::endl;
   std::cout << "(HL): " << getHex(internal_mem[reg_hl]) << std::endl;
   std::cout << "PC:   " << getHex(reg_pc, 4) << std::endl;
   std::cout << "SP:   " << getHex(reg_sp, 4) << std::endl;
}

void Memory::displayMemoryChunk (int from, int to) const
{
   for (int i = from; i < to; i++)
      std::cout << "[" << getHex(i, 4) << "] = " <<
                   getHex(internal_mem[i]) << std::endl;
}