#include <cstdint>
#include "timer.h"
#include "interrupts.h"

bool Timer::timer_stopped = 0;
Memory *Timer::memory = Memory::getInstance();
clock_t Timer::clk = 0;
unsigned long Timer::gb_cpu_clk = 0;
unsigned long Timer::last_gb_clk_TIMA_actualization = 0;

Timer::Timer () {}

void Timer::beginTimer ()
{
   clk = clock();
   gb_cpu_clk = 0;
   last_gb_clk_TIMA_actualization = 0;
   timer_stopped = false;

   // setup GB timer registers
   uint8_t TMA_value = memory->readMem(TMA_ADDRESS, true);
   memory->writeMem(TIMA_ADDRESS, TMA_value, true);
   memory->writeMem(DIV_ADDRESS, 0, true);
}

void Timer::startTimer ()
{
   if (not timer_stopped)
      return;
   
   timer_stopped = false;

   clk = clock();
}

void Timer::stopTimer ()
{
   if (timer_stopped)
      return;
   
   // actualize value of gb clock before stopping
   actualizeTime(false);
   
   timer_stopped = true;
}

void Timer::actualizeTime (bool actualize_registers)
{
   // check if cpu is halted or stopped
   if (memory->cpuHalted() or memory->cpuStopped() or timer_stopped)
      return;

   //clock_t old_clk = clk;
   clk = clock();

   // convert from  program clock to gb clock
   //gb_cpu_clk += (clk - old_clk)/float(CLOCKS_PER_SEC/GB_CLK_FREQ);

   if (actualize_registers)
      actualizeTimerRegisters();
}

void Timer::actualizeTimerRegisters ()
{
   //
}

unsigned long Timer::getClock ()
{
   actualizeTime(false);

   return gb_cpu_clk;
}