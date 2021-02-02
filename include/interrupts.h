#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#include <cstdint>

#include "gbmemory.h"


/*

INTERRUPT LIST ACCORDING TO PRIORITY:

   1: Vertical blanking
   2: LCDC Status interupt
   3: Timer overflow
   4: Serial transfer completion
   5: P10-P13 input signal low

*/


#define IE_ADDRESS   0xffff
#define IF_ADDRESS   0xff0f


class Interrupts
{
public:

   Interrupts ();

   // Returns the address of the interruption that is active (following
   // the priority criteria) and clears the if flag if there is an
   // interrupts pending to be treated or 0x0000 otherwise.
   // This method takes into consideration the IME and IE flags. Also,
   // if there are various interrupts pending, they will be returned
   // one after the other in descending priority order.
   uint16_t interruptActive ();

   // returns true if there is an enabled interrupt pending (even with the IME
   // deactivated)
   bool haltExit () const;

   // sets the corresponding bit of the IF flag to 1
   static void activateIFBit (int priority);

private:

   // returns the starting address of the interrupt
   uint16_t interruptAddress (int priority) const;

   // check and set the status of the IE flag for a given interrupt
   bool getIEStatus (int priority) const;
   void setIEStatus (int priority, bool new_value);

   // check and set the status of the IF flag for a given interrupt
   bool getIFStatus (int priority) const;
   void setIFStatus (int priority, bool new_value);


   Memory* memory;
};


#endif