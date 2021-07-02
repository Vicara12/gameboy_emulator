#ifndef LCD_h_
#define LCD_h_

#include <vector>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include "pixel_processing_unit.h"

#define DISPLAY_HEIGHT  144
#define DISPLAY_WIDTH   160
#define PXSIZE          6 // pixel size


class LCD
{
public:

    LCD ();
    ~LCD ();

    // returns false if the user wants to close the screen
    // check if the PPU has a new frame before calling this method
    bool refresh ();

private:

    void init (unsigned width, unsigned height);

    sf::RenderWindow *window;
    std::vector<std::vector<sf::RectangleShape*>> tile_grid;
    sf::Color color[4]; // color palette
    int pixels[DISPLAY_WIDTH][DISPLAY_HEIGHT];
    PixelProcessingUnit *ppu;
};

#endif