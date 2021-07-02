#include "lcd.h"


LCD::LCD () :
    ppu(PixelProcessingUnit::getInstance())
{
    init(DISPLAY_WIDTH, DISPLAY_HEIGHT);

    color[0] = sf::Color(0xff,0xff,0xff);
    color[1] = sf::Color(0x9a,0x9a,0x9a);
    color[2] = sf::Color(0x54,0x54,0x54);
    color[3] = sf::Color(0x00,0x00,0x00);
}


bool LCD::refresh ()
{
    ppu->getScreenPixels(pixels);

    window->clear();

    // draw tiles
    for (int i = 0; i < DISPLAY_WIDTH; i++)
        for (int j = 0; j < DISPLAY_HEIGHT; j++)
        {
            tile_grid[i][j]->setFillColor(color[pixels[i][j]]);
            window->draw(*tile_grid[i][j]);
        }

    window->display();

    sf::Event event;
    while (window->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window->close();
            return false;
        }
    }

    return true;
}


void LCD::init (unsigned width, unsigned height)
{
    window = new sf::RenderWindow;

    window->create(sf::VideoMode(DISPLAY_WIDTH*PXSIZE,
                                 DISPLAY_HEIGHT*PXSIZE), "GAME BOI");

    window->setVerticalSyncEnabled(true);

    tile_grid = std::vector<std::vector<sf::RectangleShape*>>(width, std::vector<sf::RectangleShape*>(height));

    // init ground tiles
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            tile_grid[i][j] = new sf::RectangleShape(sf::Vector2f(PXSIZE,
                                                                  PXSIZE));
            tile_grid[i][j]->setPosition(sf::Vector2f(float(i*PXSIZE),
                                                      float(j*PXSIZE)));
        }
    }
}


LCD::~LCD ()
{
    for (int i = 0; i < tile_grid.size(); i++)
        for (int j = 0; j < tile_grid[0].size(); j++)
            delete tile_grid[i][j];
}