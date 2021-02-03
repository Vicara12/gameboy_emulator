#include <cstdint>
#include "timer.h"
#include "interrupts.h"
#include "oscillator.h"



Timer::Timer (unsigned long initial_clock) :
      memory(Memory::getInstance()),
      last_clock_tima(initial_clock),
      last_clock_div(initial_clock)
{
   timer_stopped = not timerOn();
}


void Timer::actualizeTimerRegisters (unsigned long clk)
{
   // div register is incremented even when the timer is stopped
   incrementDIVRegister(clk);

   bool timer_bit_on = timerOn();

   // if timer is stopped but bit changed to timer on
   if (timer_bit_on and timer_stopped)
   {
      timer_stopped = false;
      // for an explaination of the next line, check incrementDIVRegister
      last_clock_tima = clk - clk%clockSource();
   }
   // if timer is on but bit changed to stopped
   else if (not timer_bit_on and not timer_stopped)
   {
      timer_stopped = true;
      // write last value to timer before stopping
      incrementTIMARegister(clk);
   }
   // if timer didn't change state and is on
   else if (not timer_stopped)
      incrementTIMARegister(clk);
   // if timer is stopped don't do anything
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
      case 0: return 1024; // f/2^10  (  4096 kHz)
      case 1: return 16;   // f/2^4   (262144 kHz)
      case 2: return 64;   // f/2^6   ( 65536 kHz)
      case 3: return 256;  // f/2^8   ( 16384 kHz)
   }

   return -1;
}


void Timer::incrementDIVRegister (unsigned long clk)
{
   // check if div register has to be actualized
   if (clk - last_clock_div >= 512)
   {
      uint8_t div = memory->readMem(DIV_ADDRESS, true);

      div += (clk - last_clock_div)/512;
      // the atribute is not actualized with clk because there might be an
      // excess from 512 that must be added at the next increment, for example
      // if clk is 522 theoretically the value of clk that incremented the div
      // register is 512, not 522
      last_clock_div = clk - clk%512;

      memory->writeMem(DIV_ADDRESS, div, true);
   }
}


void Timer::incrementTIMARegister (unsigned long clk)
{
   int clock_source = clockSource();

   // check if TIMA register has to be actualized
   if (clk - last_clock_tima >= clock_source)
   {
      uint8_t tima = memory->readMem(TIMA_ADDRESS, true);

      uint8_t new_tima = tima + (clk - last_clock_tima)/clock_source;

      // check for register overflow
      if (new_tima < tima)
      {
         new_tima = memory->readMem(TMA_ADDRESS, true);

         Interrupts::activateIFBit(3);
      }

      // for an explaination of the next line, check incrementDIVRegister
      last_clock_tima = clk - clk%clock_source;

      memory->writeMem(TIMA_ADDRESS, new_tima, true);
   }
}