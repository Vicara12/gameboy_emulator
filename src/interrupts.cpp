#include "interrupts.h"


Interrupts::Interrupts () :
   memory(Memory::getInstance())
{
   //
}


uint16_t Interrupts::interruptActive ()
{
   // check if IME (main interrupt enable) is allowed
   if (not memory->intEnabled())
      return 0x0000;

   for (int i = 1; i <= 5; i++)
   {
      // if interrupt is enabled and IF flag is set, clear IF flag and return
      // the address
      if (getIEStatus(i) and getIFStatus(i))
      {
         setIFStatus(i, false);

         return interruptAddress(i);
      }
   }

   return 0x0000;
}


uint16_t Interrupts::interruptAddress (int priotity) const
{
   switch (priotity)
   {
      case 1: {return 0x0040;}
      case 2: {return 0x0048;}
      case 3: {return 0x0050;}
      case 4: {return 0x0058;}
      case 5: {return 0x0060;}
      default:{return 0x0000;}
   }
}


bool Interrupts::getIEStatus (int priority) const
{
   uint8_t ie_register = memory->readMem(IE_ADDRESS, true);

   return (ie_register & (1 << (priority-1))) != 0;
}


void Interrupts::setIEStatus (int priority, bool new_value)
{
   uint8_t ie_register = memory->readMem(IE_ADDRESS, true);

   uint8_t mask = (1 << (priority-1));

   // if new written value is set bit
   if (new_value)
   {
      ie_register |= mask;

      memory->writeMem(IE_ADDRESS, ie_register, true);
   }
   // if new written value is clear bit
   else
   {
      ie_register &= (0xff ^ mask);

      memory->writeMem(IE_ADDRESS, ie_register, true);
   }
}


bool Interrupts::getIFStatus (int priority) const
{
   uint8_t if_register = memory->readMem(IF_ADDRESS, true);

   return (if_register & (1 << (priority-1))) != 0;
}


void Interrupts::setIFStatus (int priority, bool new_value)
{
   uint8_t if_register = memory->readMem(IF_ADDRESS, true);

   uint8_t mask = (1 << (priority-1));

   // if new written value is set bit
   if (new_value)
   {
      if_register |= mask;

      memory->writeMem(IF_ADDRESS, if_register, true);
   }
   // if new written value is clear bit
   else
   {
      if_register &= (0xff ^ mask);

      memory->writeMem(IF_ADDRESS, if_register, true);
   }
}


void Interrupts::activateIFBit (int priority)
{
   uint8_t if_register = Memory::getInstance()->readMem(IF_ADDRESS, true);

   uint8_t mask = (1 << (priority-1));

   if_register |= mask;

   Memory::getInstance()->writeMem(IF_ADDRESS, if_register, true);
}

bool Interrupts::haltExit () const
{
   for (int i = 1; i <= 5; i++)
   {
      // if IF flag is active and is enabled in the IE register return true
      if (getIEStatus(i) and getIFStatus(i))
         return true;
   }

   return false;
}