#ifndef TIMER_H_
#define TIMER_H_

#include "gbmemory.h"


// timer register addresses
#define  DIV_ADDRESS 0Xff04
#define TIMA_ADDRESS 0xff05
#define  TMA_ADDRESS 0xff06
#define  TAC_ADDRESS 0xff07

/*

This class handles the timer and its registers. Not to be confused with
Oscillator, that handles time managing of the emulation.

*/
class Timer
{
public:

   Timer (unsigned long initial_clock);

   void actualizeTimerRegisters (unsigned long clk);

private:

   // returns true if the timer is active in the TAC register
   bool timerOn ();

   // returns the power of two that divides the frequncy for the source
   int clockSource ();

   bool timer_stopped;
   Memory *memory;
   unsigned int last_clock;
};

#endif