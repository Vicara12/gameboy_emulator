#ifndef PIXEL_PROCESSING_UNIT_H_
#define PIXEL_PROCESSING_UNIT_H_

#include "gbmemory.h"
#include "interrupts.h"

#define OAM_SEARCH_CLK      20
#define PIXEL_TRANSFER_CLK  43
#define HBLANK_CLK          51
#define SCREEN_LINES        144
#define VBLANK_LINES        10

enum LCDstate {HBlank, VBlank, SearchingOAM, LCDtransfer};


class PixelProcessingUnit
{
public:

    static PixelProcessingUnit* getInstance ();

    void getScreenPixels (int pixels[160][144]);

    bool newFrameAvailable () const;

    // must be called at every emulation cycle
    void actualize (unsigned long current_clk);

    void test ();

private:

    PixelProcessingUnit ();

    // returns the clokc at which the next LCD state begins
    unsigned long getClkNextLCDstateChange ();

    // returns the state on wich the LCD is ins
    LCDstate getCurrentLCDstate ();

    void processNextLine ();

    // returns wether the display is active (0xFF40 register)
    bool lcdIsOn ();

    // base address for the map containing the tile info for the window
    uint16_t windowTileMapBaseAddress ();
    // base address for the map containing the tile info for the bg
    uint16_t bgTileMapBaseAddress ();
    // base address for the map containing the bg and window tiles
    uint16_t bgWindowTileDataBaseAddress ();

    bool windowIsEnabled ();
    // if true, sprites are 8x8 pixels in size, otherwise they are 8x16
    bool spritesAre8x8pixels ();
    // returns wether sprites are enabled
    bool spriteDisplayEnable ();
    // returns wether the background should be filled with white
    bool whiteBGactivated ();

    // position of the screen relative to the 32x32 tile map
    uint8_t getSCY ();
    uint8_t getSCX ();

    // number of the line being drawn relative to the LCD
    uint8_t getLY ();
    void writeLY (uint8_t new_ly);

    // value that LY is compared to
    uint8_t getLYC ();

    // Y and X-7 positions of the window, position of Wx=7 and WY=0 puts the
    // screen on the upper-left corner
    uint8_t getWY ();
    uint8_t getWX ();

    bool LYCcoincidenceInterruptSet ();
    bool OAMreadInterruptSet ();
    bool VBlankInterruptSet ();
    bool HBlankInterruptSet ();
    // coincidence flag must be active when LYC == LY
    void writeCoincidenceFlag (bool new_value);
    void writeModeFlag (uint new_value);


    unsigned long clk_last_action; // clk at whitch the frame started being rendered
    unsigned long current_clk;
    unsigned long clk_next_action;
    const unsigned long clk_per_frame;
    const unsigned long clk_per_line;
    int screen_pixels[160][144];
    bool new_frame; // has a new frame been rendered since the last
                    // call of getScreenPixels
    bool prev_lcd_state_on;
    bool OAM_interrupt_flag_set, HBlank_interrupt_set;
    bool line_pixels_drawn;
    static PixelProcessingUnit* instance;
    Memory *memory;
    Interrupts interrupts;
};


#endif