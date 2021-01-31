#ifndef PADINPUT_H_
#define PADINPUT_H_

#include <list>
#include <SFML/Window.hpp>
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

#define     UP_BUTTON  sf::Keyboard::W
#define   DOWN_BUTTON  sf::Keyboard::S
#define   LEFT_BUTTON  sf::Keyboard::A
#define  RIGHT_BUTTON  sf::Keyboard::D
#define      A_BUTTON  sf::Keyboard::O
#define      B_BUTTON  sf::Keyboard::K
#define SELECT_BUTTON  sf::Keyboard::N
#define  START_BUTTON  sf::Keyboard::M

#define IO_REGISTER_ADDRESS  0xff00


class PadInput
{
public:

   // automatically detects pressed buttons
   static void processInput ();

private:

   PadInput();

   static Memory* memory;
};


#endif