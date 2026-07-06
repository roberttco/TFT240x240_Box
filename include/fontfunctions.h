#pragma once

#include "app.h"
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

#include <Fonts/FreeMono9pt7b.h>
#include <Fonts/FreeSans9pt7b.h>
#include "fonts/futuricondensedlight60pt7b.h"
#include "fonts/futuricondensedlight40pt7b.h"
#include "fonts/Futuri_Condensed_Regular80pt7b.h"
#include "fonts/Varela_Regular10pt7b.h"
#include "fonts/Varela_Regular20pt7b.h"
#include <fonts/FreeMono24pt7b.h>

extern Adafruit_ST7789 tft;

bool CompareFontNames(const char *name, char *compare);
GFXfont *GetFont(char *name);
