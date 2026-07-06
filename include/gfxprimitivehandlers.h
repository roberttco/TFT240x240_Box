#pragma once

#include "app.h"
#include <CmdParser.hpp>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

extern Adafruit_ST7789 tft;

#ifdef DEBUG
void dumpParams(CmdParser *myParser);
#endif

void functFill(CmdParser *myParser);
void functString(CmdParser *myParser);
void functStringExtended(CmdParser *myParser);
void functRectangle(CmdParser *myParser);
void functCircle(CmdParser *myParser);
void functTriangle(CmdParser *myParser);
void functPixel(CmdParser *myParser);
void functLine(CmdParser *myParser);
