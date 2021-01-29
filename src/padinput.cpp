#include "padinput.h"
#include <curses.h>

PadInput::PadInput () :
      memory(Memory::getInstance())
{
   //
}


void PadInput::initNoEchoMode ()
{
   // start by saving original terminal configuration
   tcgetattr(0, &initial_settings);

   // now generate and set the new configuration
   struct termios new_settings;

   new_settings = initial_settings;
   new_settings.c_lflag &= ~ICANON; // disable canonical mode
   new_settings.c_lflag &= ~ECHO;   // disable echo
   new_settings.c_cc[VMIN] = 0;
   new_settings.c_cc[VTIME] = 0;

   tcsetattr(0, TCSANOW, &new_settings);
}


void PadInput::endNoEchoMode ()
{
   // restore initial saved settings
   tcsetattr(0, TCSANOW, &initial_settings);
}


std::list<char> PadInput::getPressedKeys ()
{
   char c;
   std::list<char> pressed_keys;

   // in order to avoid arrow input, we must detect the char pattern 27, [ and
   // any other character (that input corresponds to an arrow), so two new
   // variables must be created to detect the pattern
   bool char27 = false; // escape character
   bool char91 = false; // [ character

   while ((c = getchar()))
   {
      // if character is part of an arrow sequence, clear sequence flags and
      // ignore it
      if (char91)
      {
         char27 = char91 = false;
         continue;
      }
      // escape key detected
      else if (c == 27)
         char27 = true;
      // [ character after escape (ignore next character)
      else if (c == 91 and char27)
         char91 = true;
      // pattern not found, clear flags
      else
         char27 = char91 = false;
      

      // at this point, there shouldn't be any more sequences, hece all the
      // characters that make it to this point are purely keyboard strokes

      // convert uppercase to lowercase
      if (c >= 'A' and c <= 'Z') c += 'a' - 'A';

      // if the input corresponds to any of the gb buttons, add it to the list
      if (c == UP_BUTTON or
          c == DOWN_BUTTON or
          c == LEFT_BUTTON or
          c == RIGHT_BUTTON or
          c == A_BUTTON or
          c == B_BUTTON or
          c == SELECT_BUTTON or
          c == START_BUTTON)
         pressed_keys.push_back(c);
   }

   return pressed_keys;
}