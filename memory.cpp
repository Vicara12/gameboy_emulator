#include "memory.h"
#include "bootrom.h"

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

std::string Memory::getHex (int n)
{
   char buff[32];
   buff[0] = '0';
   buff[1] = 'x';
   int lenght = 2;

   if (n == 0) return "0x00";

   while (n > 0)
   {
      int digit = n%16;
      n /= 16;

      if (digit < 10) buff[lenght] = char(digit) + '0';
      else buff[lenght] = char(digit-10) + 'a';

      lenght++;
   }

   if (lenght == 3)
   {
      buff[lenght] = '0';
      lenght++;
   }

   for (int i = 2; i < 2 + (lenght-2)/2; i++)
   {
      //std::cout << "swap" << i << " " << lenght - i + 1 << std::endl;
      char temp = buff[i];
      buff[i] = buff[lenght - i + 1];
      buff[lenght - i + 1] = temp;
   }

   buff[lenght] = 0;

   return std::string(buff);
}

uint8_t Memory::readMem (uint16_t address) const
{
   return internal_mem[address];
}

void Memory::writeMem (uint16_t address, uint8_t value)
{
   internal_mem[address] = value;
}

uint16_t Memory::readReg (Register reg) const
{
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

void Memory::writeReg (Register reg, uint16_t value)
{
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

bool Memory::readFlag (Flag f) const
{
   if       (f == Z_f) return (reg_af & 0x0080) != 0;
   else if  (f == N_f) return (reg_af & 0x0040) != 0;
   else if  (f == H_f) return (reg_af & 0x0020) != 0;
   else                return (reg_af & 0x0010) != 0;
}

void Memory::writeFlag (Flag f, bool value)
{
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
   cpu_halted = halted;
}

bool Memory::cpuStopped () const
{
   return cpu_stopped;
}

void Memory::changeCpuStop (bool stop)
{
   cpu_stopped = stop;
}

bool Memory::intEnabled () const
{
   return cpu_int_enabled;
}

void Memory::changeIntEnabled (bool int_enabled)
{
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