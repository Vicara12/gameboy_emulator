#ifndef PADINPUT_H_
#define PADINPUT_H_

#include <list>
#include <unistd.h>
#include <termios.h>
#include "gbmemory.h"
#include "interrupts.h"

/*

BUTTON-KEYBOARD MAPPING:

   UP   -> W
   DOWN -> S
   LEFT -> A
   RIGHT-> D
   A    -> O
   B    -> K
   SELECT -> N
   START  -> M

*/

#define     UP_BUTTON  'w'
#define   DOWN_BUTTON  's'
#define   LEFT_BUTTON  'a'
#define  RIGHT_BUTTON  'd'
#define      A_BUTTON  'o'
#define      B_BUTTON  'k'
#define SELECT_BUTTON  'n'
#define  START_BUTTON  'm'


class PadInput
{
public:

   PadInput();

   void initNoEchoMode ();
   void endNoEchoMode ();

   // automatically detects pressed buttons
   void processInput ();

private:

   // returns a list of pressed keys from the button set
   std::list<char> getPressedKeys ();


   struct termios initial_settings; // initial terminal configuration
   Memory* memory;
   Interrupts interrupts;
};


#endif