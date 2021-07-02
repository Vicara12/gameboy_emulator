#include "pixel_processing_unit.h"

PixelProcessingUnit *PixelProcessingUnit::instance = new PixelProcessingUnit();


PixelProcessingUnit::PixelProcessingUnit () :
    new_frame(false)
{
    //
}


PixelProcessingUnit* PixelProcessingUnit::getInstance ()
{
    return instance;
}


void PixelProcessingUnit::getScreenPixels (int pixels[160][144])
{
    new_frame = false;

    for (int i = 0; i < 160; i++)
        for (int j = 0; j < 144; j++)
            pixels[i][j] = screen_pixels[i][j];
}


bool PixelProcessingUnit::newFrameAvailable () const
{
    return new_frame;
}


void PixelProcessingUnit::actualize ()
{
    for (int i = 0; i < 160; i++)
        for (int j = 0; j < 144; j++)
            screen_pixels[i][j] = (i + j)%4;
}