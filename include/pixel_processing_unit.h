#ifndef PIXEL_PROCESSING_UNIT_H_
#define PIXEL_PROCESSING_UNIT_H_


class PixelProcessingUnit
{
public:

    static PixelProcessingUnit* getInstance ();

    void getScreenPixels (int pixels[160][144]);

    bool newFrameAvailable () const;

    // must be called at every emulation cycle
    void actualize ();

private:

    PixelProcessingUnit ();

    // returns wether the display is active (0xFF40 register)
    bool lcdIsOn ();


    int screen_pixels[160][144];

    bool new_frame; // has a new frame been rendered since the last
                    // call of getScreenPixels
    
    static PixelProcessingUnit* instance;
};


#endif