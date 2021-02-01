#ifndef TIMER_H_
#define TIMER_H_

#include "gbmemory.h"
#include <chrono>


// timer register addresses
#define  DIV_ADDRESS 0Xff04
#define TIMA_ADDRESS 0xff05
#define  TMA_ADDRESS 0xff06
#define  TAC_ADDRESS 0xff07

/*

This class handles the timer and its registers. Not to be confused with
timing, that regulates the speed of the emulation.

*/
class Timer
{
public:

   // this method must be called at power up, it sets up registers
   static void beginTimer ();

   // starts and stops internal clock of the system (timer must be inited first)
   static void startTimer ();
   static void stopTimer ();

   // this method must be called periodically
   // if parameters are true this also actualizes the content of the timer
   // registers, otherwise it only increments the cpu clock
   static void actualizeTime (bool actualize_registers);

   // returns the value of the internal CPU clock
   static unsigned long getClock ();

private:

   Timer ();

   static void actualizeTimerRegisters ();


   static bool timer_stopped;
   static Memory *memory;
   static clock_t clk;  // real program cpu clock
   static unsigned long gb_cpu_clk; // emulated gameboy's cpu clock
   static unsigned long last_gb_clk_TIMA_actualization; // value of gb_cpu_clk
                                                // the last time the TIMA reg
                                                // was actualized
};

#endif