#include <cstdint>
#include "timer.h"
#include "interrupts.h"
#include "oscillator.h"



Timer::Timer (unsigned long initial_clock) :
      memory(Memory::getInstance()),
      last_clock(initial_clock)
{
   timer_stopped = not timerOn();
}


void Timer::actualizeTimerRegisters (unsigned long clk)
{
   //
}


bool Timer::timerOn ()
{
   // bit 3 of the TAC register indicates if the clock is on
   return memory->readMem(TAC_ADDRESS, true) & 0x04;
}


int Timer::clockSource ()
{
   uint8_t source = memory->readMem(TAC_ADDRESS, true) & 0x03;

   switch (source)
   {
      case 0: return 10; // f/2^10  (  4096 kHz)
      case 1: return 4;  // f/2^4   (262144 kHz)
      case 2: return 6;  // f/2^6   ( 65536 kHz)
      case 3: return 8;  // f/2^8   ( 16384 kHz)
   }

   return -1;
}