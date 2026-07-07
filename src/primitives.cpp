#include "app.h"
#include <CmdParser.hpp>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

#include "fonts.h"

extern Adafruit_ST7789 tft;

// primitive functions
#ifdef DEBUG
void dumpParams(CmdParser *myParser)
{
  size_t count = myParser->getParamCount();

  DEBUG_PRINTLN("Parameter debug:");

  DEBUG_PRINT("Command: ");
  DEBUG_PRINTLN(myParser->getCommand());

  DEBUG_PRINT("Parameter count: ");
  DEBUG_PRINTLN(count);

  for (size_t i = 0; i < count; i++)
  {
    DEBUG_PRINT("  Param ");
    DEBUG_PRINT(i);
    DEBUG_PRINT(": ");
    DEBUG_PRINTLN(myParser->getCmdParam(i));
  }
}
#endif

void functFill(CmdParser *myParser)
{
  DEBUG_DUMPPARAMS(myParser);

  // F,COLOR
  if (myParser->getParamCount() != 2)
  {
    DEBUG_PRINTLN("PARAMERROR");
    return;
  }

  uint16_t fg = (uint16_t)strtol(myParser->getCmdParam(1), 0, 16);

  tft.fillScreen(fg);
}

void functString(CmdParser *myParser)
{
  DEBUG_DUMPPARAMS(myParser);

  // S,X,Y,FG,BG,S,MSG
  if (myParser->getParamCount() != 7)
  {
    DEBUG_PRINTLN("PARAMERROR");
    return;
  }

  int paramIndex = 1;

  uint16_t x = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t y = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t fg = (uint16_t)strtol(myParser->getCmdParam(paramIndex++), 0, 16);
  uint16_t bg = (uint16_t)strtol(myParser->getCmdParam(paramIndex++), 0, 16);
  int sz = atoi(myParser->getCmdParam(paramIndex++));
  char *msg = myParser->getCmdParam(paramIndex++);

  tft.setTextSize(sz);
  tft.setCursor(x, y);
  tft.setTextColor(fg, bg);
  tft.print(msg);
}

void functStringExtended(CmdParser *myParser)
{
  DEBUG_DUMPPARAMS(myParser);

  // SX,X,Y,FG,BG,FONT,S,MSG
  if (myParser->getParamCount() != 8)
  {
    DEBUG_PRINTLN("PARAMERROR");
    return;
  }

  int paramIndex = 1;

  uint16_t x = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t y = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t fg = (uint16_t)strtol(myParser->getCmdParam(paramIndex++), 0, 16);
  uint16_t bg = (uint16_t)strtol(myParser->getCmdParam(paramIndex++), 0, 16);
  char *fontName = myParser->getCmdParam(paramIndex++);
  int sz = atoi(myParser->getCmdParam(paramIndex++));
  char *msg = myParser->getCmdParam(paramIndex++);

  DEBUG_PRINTF("x: %d, y: %d\r\nfg: 0x%06X, bg: 0x%06X\r\nfont: %s\r\nsize: %d\r\nmsg: %s\r\n", x, y, fg, bg, fontName, sz, msg);

  GFXfont *f = GetFont(fontName);
  if (f != NULL)
  {
    DEBUG_PRINT("Using font: ");
    DEBUG_PRINTLN(fontName);
    // since non-system fonts don't draw in the background color,
    // replicate that behavior by filling the bounding box enclosing
    // the string with the background color.

    tft.setFont(f);
    tft.setTextSize(sz);
    int16_t xbb, ybb;
    uint16_t wbb, hbb;
    tft.getTextBounds(msg, x, y, &xbb, &ybb, &wbb, &hbb);
    tft.fillRect(xbb, ybb, wbb, hbb, bg);
  }
  else
  {
    DEBUG_PRINTLN("Failed to get font.");
  }

  tft.setTextSize(sz);
  tft.setCursor(x, y);
  tft.setTextColor(fg, bg);
  tft.print(msg);

  // always revert to the system font
  tft.setFont();
}

void functRectangle(CmdParser *myParser)
{
  DEBUG_DUMPPARAMS(myParser);

  // R,X,Y,W,H,FG,R,F,RR
  if (myParser->getParamCount() < 8 || myParser->getParamCount() > 9)
  {
    DEBUG_PRINTLN("PARAMERROR");
    return;
  }

  int paramIndex = 1;

  uint16_t x = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t y = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t w = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t h = atoi(myParser->getCmdParam(paramIndex++));

  uint16_t fg = (uint16_t)strtol(myParser->getCmdParam(paramIndex++), 0, 16);

  bool rounded = myParser->getCmdParam(paramIndex++)[0] == '1' ? true : false;
  bool filled = myParser->getCmdParam(paramIndex++)[0] == '1' ? true : false;

  int radius = 0;
  if (myParser->getParamCount() == 9)
  {
    radius = atoi(myParser->getCmdParam(paramIndex++));
  }

  if (!rounded && !filled)
  {
    tft.drawRect(x, y, w, h, fg);
  }

  if (!rounded && filled)
  {
    tft.fillRect(x, y, w, h, fg);
  }

  if (rounded && !filled)
  {
    tft.drawRoundRect(x, y, w, h, radius, fg);
  }

  if (rounded && filled)
  {
    tft.fillRoundRect(x, y, w, h, radius, fg);
  }
}

void functCircle(CmdParser *myParser)
{
  DEBUG_DUMPPARAMS(myParser);

  // C,XC,YC,FG,R,F
  if (myParser->getParamCount() != 6)
  {
    DEBUG_PRINTLN("PARAMERROR");
    return;
  }

  int paramIndex = 1;

  uint16_t xc = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t yc = atoi(myParser->getCmdParam(paramIndex++));

  uint16_t fg = (uint16_t)strtol(myParser->getCmdParam(paramIndex++), 0, 16);

  uint16_t radius = atoi(myParser->getCmdParam(paramIndex++));

  bool filled = myParser->getCmdParam(paramIndex++)[0] == '1' ? true : false;

  if (!filled)
  {
    tft.drawCircle(xc, yc, radius, fg);
  }
  else
  {
    tft.fillCircle(xc, yc, radius, fg);
  }
}

void functTriangle(CmdParser *myParser)
{
  DEBUG_DUMPPARAMS(myParser);

  // T,X1,Y1,X2,Y2,X3,Y3,FG,F
  if (myParser->getParamCount() != 9)
  {
    DEBUG_PRINTLN("PARAMERROR");
    return;
  }

  int paramIndex = 1;

  uint16_t x1 = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t y1 = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t x2 = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t y2 = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t x3 = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t y3 = atoi(myParser->getCmdParam(paramIndex++));

  uint16_t fg = (uint16_t)strtol(myParser->getCmdParam(paramIndex++), 0, 16);

  bool filled = myParser->getCmdParam(paramIndex++)[0] == '1' ? true : false;

  if (filled)
  {
    tft.fillTriangle(x1, y1, x2, y2, x3, y3, fg);
  }
  else
  {
    tft.drawTriangle(x1, y1, x2, y2, x3, y3, fg);
  }
}

void functPixel(CmdParser *myParser)
{
  DEBUG_DUMPPARAMS(myParser);

  // P,X,Y,FG
  if (myParser->getParamCount() != 4)
  {
    DEBUG_PRINTLN("PARAMERROR");
    return;
  }

  int paramIndex = 1;

  uint16_t x = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t y = atoi(myParser->getCmdParam(paramIndex++));

  uint16_t fg = (uint16_t)strtol(myParser->getCmdParam(paramIndex++), 0, 16);

  tft.drawPixel(x, y, fg);
}

void functLine(CmdParser *myParser)
{
  DEBUG_DUMPPARAMS(myParser);

  // L,X1,Y1,X2,Y2,FG
  if (myParser->getParamCount() != 4)
  {
    DEBUG_PRINTLN("PARAMERROR");
    return;
  }

  int paramIndex = 1;

  uint16_t x1 = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t y1 = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t x2 = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t y2 = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t fg = (uint16_t)strtol(myParser->getCmdParam(paramIndex++), 0, 16);

  tft.drawLine(x1, y1, x2, y2, fg);
}
