#include "oscillator.h"

 
ChronoTiming Oscillator::start;
ChronoTiming Oscillator::pause_init;
std::chrono::duration<float> Oscillator::duration;
bool Oscillator::timer_stopped;
float Oscillator::paused_s;



Oscillator::Oscillator () {}


void Oscillator::beginOscillator ()
{
   start = std::chrono::high_resolution_clock::now();
   timer_stopped = false;
   paused_s = 0;
}


void Oscillator::startOscillator ()
{
   if (not timer_stopped)
      return;
   
   // add the time the oscillator has been paused to the paused_ms offset
   duration = std::chrono::high_resolution_clock::now() - pause_init;
   paused_s += duration.count();

   timer_stopped = false;
}


void Oscillator::stopOscillator ()
{
   if (timer_stopped)
      return;
   
   // save the initial time of the stop so the time it has been paused can be
   // added to the offset when the oscillator is enabled again
   pause_init = std::chrono::high_resolution_clock::now();

   timer_stopped = true;
}


unsigned long Oscillator::getClockCycles ()
{
   return getTime() * GB_CLK_FREQ;
}


float Oscillator::getTime ()
{
   // seconds sice the beginOscillator method was called
   duration = std::chrono::high_resolution_clock::now() - start;
   // substract the seconds the oscillator has been stopped
   return duration.count() - paused_s;
}