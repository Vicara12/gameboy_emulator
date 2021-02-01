#include "padinput.h"


Memory *PadInput::memory = Memory::getInstance();

PadInput::PadInput () {}



void PadInput::processInput ()
{
   uint8_t io_register = memory->readMem(IO_REGISTER_ADDRESS, true);

   bool set_interrupt = false;

   // clear previous readings
   io_register |= 0x0f;


   // if P14 is low check for directional keys
   if ((io_register & 0x10) == 0)
   {
      if (sf::Keyboard::isKeyPressed(RIGHT_BUTTON))
      {
         io_register &= 0xfe; // clear first bit
         set_interrupt = true;
      }
      if (sf::Keyboard::isKeyPressed(LEFT_BUTTON))
      {
         io_register &= 0xfd; // clear second bit
         set_interrupt = true;
      }
      if (sf::Keyboard::isKeyPressed(UP_BUTTON))
      {
         io_register &= 0xfb; // clear third bit
         set_interrupt = true;
      }
      if (sf::Keyboard::isKeyPressed(DOWN_BUTTON))
      {
         io_register &= 0xf7; // clear fourth bit
         set_interrupt = true;
      }
   }

   // if P15 is low check for action keys
   if ((io_register & 0x20) == 0)
   {
      if (sf::Keyboard::isKeyPressed(A_BUTTON))
      {
         io_register &= 0xfe; // clear first bit
         set_interrupt = true;
      }
      if (sf::Keyboard::isKeyPressed(B_BUTTON))
      {
         io_register &= 0xfd; // clear second bit
         set_interrupt = true;
      }
      if (sf::Keyboard::isKeyPressed(SELECT_BUTTON))
      {
         io_register &= 0xfb; // clear third bit
         set_interrupt = true;
      }
      if (sf::Keyboard::isKeyPressed(START_BUTTON))
      {
         io_register &= 0xf7; // clear fourth bit
         set_interrupt = true;
      }
   }

   memory->writeMem(IO_REGISTER_ADDRESS, io_register, true);

   if (set_interrupt)
      Interrupts::activateIFBit(5);
}