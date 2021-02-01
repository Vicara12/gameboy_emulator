#ifndef OSCILLATOR_H_
#define OSCILLATOR_H_

#include <chrono>


#define GB_CLK_FREQ 4194304

typedef std::chrono::_V2::system_clock::time_point    ChronoTiming;


/*

   This class handles the timming of the emulation. It is used to decide
   when to execute the next instruction, refresh screen pixels, drive the
   timer module, among other tasks.
   Not to be confused with the timer class, that handles the timer and its
   registers.

*/
class Oscillator
{
public:

   // This method should be called at power up. Inits and starts the oscilator
   // emulation.
   static void beginOscillator ();

   // Start and stop (but not reset) the oscilator.
   // In order to reset use beginTimming
   static void startOscillator ();
   static void stopOscillator ();

   // returns the clock cycles
   static unsigned long getClockCycles ();

   // returns the time in seconds that the oscillator has been active
   static float getTime ();

private:

   Oscillator ();

   static ChronoTiming start, pause_init;
   static std::chrono::duration<float> duration;
   static bool timer_stopped;
   static float paused_s;  // seconds the oscillator has been paused
};

#endif