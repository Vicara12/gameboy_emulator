#include "pixel_processing_unit.h"


#include <iostream>

PixelProcessingUnit *PixelProcessingUnit::instance = new PixelProcessingUnit();


PixelProcessingUnit::PixelProcessingUnit () :
    clk_next_action(0),
    clk_per_frame((OAM_SEARCH_CLK+PIXEL_TRANSFER_CLK+HBLANK_CLK)*(SCREEN_LINES+VBLANK_LINES)),
    clk_per_line(OAM_SEARCH_CLK+PIXEL_TRANSFER_CLK+HBLANK_CLK),
    new_frame(false),
    prev_lcd_state_on(true),
    OAM_interrupt_flag_set(false),
    HBlank_interrupt_set(false),
    line_pixels_drawn(false),
    memory(Memory::getInstance())
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
            pixels[i][j] = prev_lcd_state_on ? screen_pixels[i][j] : 0;
}


bool PixelProcessingUnit::newFrameAvailable () const
{
    return new_frame;
}


void PixelProcessingUnit::actualize (unsigned long current_clk)
{
    bool lcdOn = lcdIsOn();
    this->current_clk = current_clk;

    // if lcd has been turned off, actualize frame
    if (prev_lcd_state_on and not lcdOn)
    {
        new_frame = true;
        prev_lcd_state_on = false;
    }
    // lcd on
    else if (lcdOn)
    {
        uint8_t current_lcd_state = getCurrentLCDstate();

        // actualize line number
        if (current_clk >= clk_next_action and current_lcd_state == SearchingOAM)
        {
            uint8_t line = current_clk%clk_per_frame/clk_per_line;
            writeLY(line);

            if (line == getLYC())
            {
                writeCoincidenceFlag(true);

                if (LYCcoincidenceInterruptSet())
                    interrupts.activateIFBit(2);
            }
            else
            {
                writeCoincidenceFlag(false);
            }

            if (line == 144)
            {
                new_frame = true;

                if (VBlankInterruptSet())
                    interrupts.activateIFBit(1);
            }
        }

        // actualize lcd state register
        writeModeFlag(current_lcd_state);

        // if in LCD transfer mode, draw every pixel in the line at once
        if (current_lcd_state == LCDtransfer)
        {
            if (not line_pixels_drawn)
            {
                line_pixels_drawn = true;
                processNextLine();
            }
        }
        else
        {
            line_pixels_drawn = false;
        }

        // actualize new action
        if (current_clk >= clk_next_action)
        {
            clk_last_action = clk_next_action;
            clk_next_action = getClkNextLCDstateChange();

            // check for OAM read or HBlank iterrupts
            if (current_lcd_state == HBlank and HBlankInterruptSet() or
                current_lcd_state == SearchingOAM and OAMreadInterruptSet())
                interrupts.activateIFBit(2);
        }
        
    }
    // if lcd is off, no work is done

    prev_lcd_state_on = lcdOn;
}


void PixelProcessingUnit::processNextLine ()
{
    // compute next pixels
    // write values to position registers
    // actualize last_frame_begin_clk

    // check for white background (bit 0 of FF40)
    // check for drawing window or sprites deactivated

    // set interrupts for VBlank

}

void PixelProcessingUnit::test ()
{
    for (int i = 0; i < 1*clk_per_frame; i++)
    {
        current_clk = i;

        unsigned long next_action = getClkNextLCDstateChange();

        if (current_clk >= next_action)
        {
            clk_last_action = next_action;
            next_action = getClkNextLCDstateChange();
        }
        
        std::cout << i << ": ";
        switch (getCurrentLCDstate())
        {
            case 0:
                std::cout << "HBlank";
                break;
            case 1:
                std::cout << "VBlank";
                break;
            case 2:
                std::cout << "OAM search";
                break;
            case 3:
                std::cout << "LCD transfer";
                break;
        }
        std::cout << "   next clk: " << next_action;

        if (i%5 == 0)
            std::cout << ""<< std::endl;
        else
            std::cout << " || \t";
    }
}


unsigned long PixelProcessingUnit::getClkNextLCDstateChange ()
{

    uint reminder = clk_last_action%clk_per_frame;

    if (reminder >= clk_per_line*SCREEN_LINES)
        return clk_last_action + clk_per_line*VBLANK_LINES;
    
    int clk_begin_line = clk_last_action - clk_last_action%clk_per_line;
    reminder %= clk_per_line;
    
    if (reminder < OAM_SEARCH_CLK)
        return clk_begin_line+OAM_SEARCH_CLK;
    if (reminder < (OAM_SEARCH_CLK+PIXEL_TRANSFER_CLK))
        return clk_begin_line+OAM_SEARCH_CLK+PIXEL_TRANSFER_CLK;
    return clk_begin_line+clk_per_line;
}


LCDstate PixelProcessingUnit::getCurrentLCDstate ()
{
    // get clk relative to the beggining of the current frame
    uint reminder = current_clk%clk_per_frame;

    if (reminder >= clk_per_line*SCREEN_LINES)
        return VBlank;
    
    // get clk relative to the beggining of the current line
    reminder %= clk_per_line;

    if (reminder < OAM_SEARCH_CLK)
        return SearchingOAM;
    if (reminder < (OAM_SEARCH_CLK+PIXEL_TRANSFER_CLK))
        return LCDtransfer;
    return HBlank;
}


bool PixelProcessingUnit::lcdIsOn ()
{
    uint8_t reg_value = memory->readMem(0xff40, true);
    return reg_value&0x80;
}


uint16_t PixelProcessingUnit::windowTileMapBaseAddress ()
{
    uint8_t reg_value = memory->readMem(0xff40, true);
    return reg_value&0x40 ? 0x9C00 : 0x9800;
}


uint16_t PixelProcessingUnit::bgWindowTileDataBaseAddress ()
{
    uint8_t reg_value = memory->readMem(0xff40, true);
    return reg_value&0x10 ? 0x8000 : 0x8800;
}


uint16_t PixelProcessingUnit::bgTileMapBaseAddress ()
{
    uint8_t reg_value = memory->readMem(0xff40, true);
    return reg_value&0x04 ? 0x9C00 : 0x9800;
}


bool PixelProcessingUnit::windowIsEnabled ()
{
    uint8_t reg_value = memory->readMem(0xff40, true);
    return reg_value&0x20;
}


bool PixelProcessingUnit::spritesAre8x8pixels ()
{
    uint8_t reg_value = memory->readMem(0xff40, true);
    return reg_value&0x04;
}


bool PixelProcessingUnit::spriteDisplayEnable ()
{
    uint8_t reg_value = memory->readMem(0xff40, true);
    return reg_value&0x02;
}


bool PixelProcessingUnit::whiteBGactivated ()
{
    uint8_t reg_value = memory->readMem(0xff40, true);
    return not reg_value&0x01;
}


uint8_t PixelProcessingUnit::getSCY ()
{
    return memory->readMem(0xff42, true);
}


uint8_t PixelProcessingUnit::getSCX ()
{
    return memory->readMem(0xff43, true);
}


uint8_t PixelProcessingUnit::getLY ()
{
    return memory->readMem(0xff44, true);
}


void PixelProcessingUnit::writeLY (uint8_t new_ly)
{
    memory->writeMem(0xff44, new_ly, true);
}


uint8_t PixelProcessingUnit::getLYC ()
{
    return memory->readMem(0xff45, true);
}


uint8_t PixelProcessingUnit::getWY ()
{
    return memory->readMem(0xff4A, true);
}


uint8_t PixelProcessingUnit::getWX ()
{
    return memory->readMem(0xff4B, true);
}


bool PixelProcessingUnit::LYCcoincidenceInterruptSet ()
{
    uint8_t reg_value = memory->readMem(0xff41, true);
    return not reg_value&0x40;
}


bool PixelProcessingUnit::OAMreadInterruptSet ()
{
    uint8_t reg_value = memory->readMem(0xff41, true);
    return not reg_value&0x20;
}


bool PixelProcessingUnit::VBlankInterruptSet ()
{
    uint8_t reg_value = memory->readMem(0xff41, true);
    return not reg_value&0x10;
}


bool PixelProcessingUnit::HBlankInterruptSet ()
{
    uint8_t reg_value = memory->readMem(0xff41, true);
    return not reg_value&0x08;
}


void PixelProcessingUnit::writeCoincidenceFlag (bool new_value)
{
    uint8_t reg_value = memory->readMem(0xff41, true);
    reg_value = new_value ? reg_value|0x04 : reg_value&(~0x04);
    memory->writeMem(0xff41, reg_value, true);
}


void PixelProcessingUnit::writeModeFlag (uint new_value)
{
    uint8_t reg_value = memory->readMem(0xff41, true);
    reg_value = (reg_value&(~0x03))|(new_value&0x03);
    memory->writeMem(0xff41, reg_value, true);
}