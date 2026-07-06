#include "app.h"

#include <CmdCallback.hpp>
#include <CmdParser.hpp>

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789


#include "gfxprimitivehandlers.h"
#include "fontfunctions.h"

// initialize ST7789 TFT library with hardware SPI module
// SCK (CLK) ---> NodeMCU pin D5 (GPIO14)
// MOSI(DIN) ---> NodeMCU pin D7 (GPIO13)
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// command parser stuff
CmdCallback<8> cmdCallbackP;
//CmdBuffer<512> myBuffer;
CmdParser     myParser;

int rotation = 0;
bool connecting = true;
bool waiting = false;

// if you dont want to include a secrets file then just uncomment the following lines and don't include secrets.h
/*
#include <EspMQTTClient.h>
EspMQTTClient client(
  "WIFI_SSID",
  "WIFI_PASSWORD",
  "MQTT_BROKER_IP",
  "MQTT_CLIENT_ID",
  "MQTT_PORT"
);
*/

#include "secrets.h"  // include EspMQTTClient client() from secret file

void setup(void)
{
  Serial.begin(115200);

  // if the display has CS pin try with SPI_MODE0
  tft.init(240, 240, SPI_MODE3);    // Init ST7789 display 240x240 pixel
  tft.setSPISpeed(40000000);

  //
  tft.setRotation(rotation);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(false);

  ////
  // Enable cmd strings with "" -> SET VAL "HALLO WORLD"
  myParser.setOptIgnoreQuote(false); // default

  ////
  // Set command seperator.
  myParser.setOptSeperator(','); // default

  cmdCallbackP.addCmd("F", &functFill);
  cmdCallbackP.addCmd("S", &functString);
  cmdCallbackP.addCmd("SX", &functStringExtended);
  cmdCallbackP.addCmd("R", &functRectangle);
  cmdCallbackP.addCmd("C", &functCircle);
  cmdCallbackP.addCmd("T", &functTriangle);
  cmdCallbackP.addCmd("P", &functPixel);
  cmdCallbackP.addCmd("L", &functCircle);

  client.enableDebuggingMessages(); // Enable debugging messages sent to serial output
  client.enableHTTPWebUpdater(); // Enable the web updater. User and password default to values of MQTTUsername and MQTTPassword. These can be overrited with enableHTTPWebUpdater("user", "password").
  client.enableLastWillMessage("7133egyptian/out/display1/state", "Offline");  // You can activate the retain flag by setting the third parameter to true
}

int connectingRadius = 1;
int lastRadius = 1;
void loop(void)
{
  client.loop();

  if (connecting || waiting)
  {
      
      connectingRadius += 8;
      if (connectingRadius > 110)
      {
        connectingRadius = 0;
      }
      
      tft.drawCircle(120,120, lastRadius, ST77XX_BLACK);
      if (waiting)
      {
        tft.drawCircle(120,120, 110-connectingRadius, ST77XX_ORANGE);
        lastRadius = 110-connectingRadius;
      }
      else
      {
        tft.drawCircle(120,120, connectingRadius, ST77XX_WHITE);
        lastRadius = connectingRadius;
      }

      delay(70);
  }
//  else
//  {
//    cmdCallback.updateCmdProcessing(&myParser, &myBuffer, &Serial);
//  }
}

void onConnectionEstablished()
{
  // Subscribe to "7133egyptian/in/display1" and display received message to Serial
  client.subscribe("7133egyptian/in/display1/lcd", [](const String & payload) {
    if (waiting)
    {
      tft.fillScreen(ST77XX_BLACK);
      waiting = false;
    }
    Serial.println(payload);

    char *p = (char *)payload.c_str();
    char *i;
    char *token = strtok_r(p,"|",&i);
    while (token != NULL)
    {
      if (myParser.parseCmd(token) != CMDPARSER_ERROR)
      {
        cmdCallbackP.processCmd(&myParser);
      }

      token = strtok_r(NULL,"|",&i);
    }
  });

  client.subscribe("7133egyptian/in/display1/command", [](const String & payload) {
    if (waiting)
    {
      tft.fillScreen(ST77XX_BLACK);
      waiting = false;
    }
    Serial.println(payload);

    int cmd = payload.toInt();
    switch (cmd)
    {
      case 255: // clear display and home cursor
        tft.fillScreen(ST77XX_BLACK);
        tft.setCursor(0,0);
        break;
      case 0: // toggle rotation
      case 1: // toggle rotation
      case 2: // toggle rotation
      case 3: // toggle rotation
        rotation = cmd;
        tft.setRotation(rotation);
        break;
      default:
        break; 
    }
  });
  
  // Publish a message to "mytopic/test"
  client.publish("7133egyptian/out/display1/state", "Online"); // You can activate the retain flag by setting the third parameter to true
  
  connecting = false;
  waiting = true;
  //tft.fillScreen(ST77XX_BLACK);
}


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

  uint16_t fg = (uint16_t) strtol(myParser->getCmdParam(1), 0, 16);
  
  tft.fillScreen(fg);
}

void functString(CmdParser *myParser) 
{ 
  DEBUG_DUMPPARAMS(myParser);

  //S,X,Y,FG,BG,S,MSG
  if (myParser->getParamCount() != 7)
  {
    DEBUG_PRINTLN("PARAMERROR");
    return;
  }

  int paramIndex = 1;

  uint16_t x = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t y = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t fg = (uint16_t) strtol(myParser->getCmdParam(paramIndex++), 0, 16);
  uint16_t bg = (uint16_t) strtol(myParser->getCmdParam(paramIndex++), 0, 16);
  int sz = atoi(myParser->getCmdParam(paramIndex++));
  char *msg = myParser->getCmdParam(paramIndex++);

  tft.setTextSize(sz);
  tft.setCursor(x, y);
  tft.setTextColor(fg,bg);
  tft.print(msg);
}

void functStringExtended(CmdParser *myParser) 
{ 
  DEBUG_DUMPPARAMS(myParser);

  //SX,X,Y,FG,BG,FONT,S,MSG
  if (myParser->getParamCount() != 8)
  {
    DEBUG_PRINTLN("PARAMERROR");
    return;
  }

  int paramIndex = 1;

  uint16_t x = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t y = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t fg = (uint16_t) strtol(myParser->getCmdParam(paramIndex++), 0, 16);
  uint16_t bg = (uint16_t) strtol(myParser->getCmdParam(paramIndex++), 0, 16);
  char *fontName = myParser->getCmdParam(paramIndex++);
  int sz = atoi(myParser->getCmdParam(paramIndex++));
  char *msg = myParser->getCmdParam(paramIndex++);

  GFXfont *f = GetFont(fontName);
  if (f != NULL)
  {
    DEBUG_PRINT("Using font: ");
    DEBUG_PRINTLN(fontName);
    // since non-system fonts dont draw in teh background color,
    // replicat that behavior by filling the bounding box enclosing
    // the string with the background color.
    
    tft.setFont(f);
    tft.setTextSize(sz);
    int16_t  xbb, ybb;
    uint16_t wbb, hbb;
    tft.getTextBounds(msg, x, y, &xbb, &ybb, &wbb, &hbb);
    tft.fillRect(xbb,ybb,wbb,hbb,bg);
  }

  tft.setTextSize(sz);
  tft.setCursor(x, y);
  tft.setTextColor(fg,bg);
  tft.print(msg);

  // always revert to the system font
  tft.setFont();
}

void functRectangle(CmdParser *myParser) 
{ 
  DEBUG_DUMPPARAMS(myParser);

  //R,X,Y,W,H,FG,R,F,RR
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
  
  uint16_t fg = (uint16_t) strtol(myParser->getCmdParam(paramIndex++), 0, 16);

  bool rounded = myParser->getCmdParam(paramIndex++)[0] == '1' ? true : false;
  bool filled = myParser->getCmdParam(paramIndex++)[0] == '1' ? true : false;
  
  int radius = 0;
  if (myParser->getParamCount() == 9)
  {
    radius = atoi(myParser->getCmdParam(paramIndex++));
  }
  
  if (!rounded && !filled)
  {
    tft.drawRect(x,y,w,h,fg);
  }
  
  if (!rounded && filled)
  {
    tft.fillRect(x,y,w,h,fg);
  }
  
  if(rounded && !filled)
  {
    tft.drawRoundRect(x,y,w,h,radius,fg);
  }

  if (rounded && filled)
  {
    tft.fillRoundRect(x,y,w,h,radius,fg);
  }
}

void functCircle(CmdParser *myParser) 
{ 
  DEBUG_DUMPPARAMS(myParser);

  //C,XC,YC,FG,R,F
  if (myParser->getParamCount() != 6)
  {
    DEBUG_PRINTLN("PARAMERROR");
    return;
  }

  int paramIndex = 1;
  
  uint16_t xc = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t yc = atoi(myParser->getCmdParam(paramIndex++));

  uint16_t fg = (uint16_t) strtol(myParser->getCmdParam(paramIndex++), 0, 16);

  uint16_t radius = atoi(myParser->getCmdParam(paramIndex++));

  bool filled = myParser->getCmdParam(paramIndex++)[0] == '1' ? true : false;
  
  if (!filled)
  {
    tft.drawCircle(xc,yc,radius,fg);
  }
  else
  {
    tft.fillCircle(xc,yc,radius,fg);
  }
}

void functTriangle(CmdParser *myParser) 
{ 
  DEBUG_DUMPPARAMS(myParser);

  //T,X1,Y1,X2,Y2,X3,Y3,FG,F
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
  
  uint16_t fg = (uint16_t) strtol(myParser->getCmdParam(paramIndex++), 0, 16);

  bool filled = myParser->getCmdParam(paramIndex++)[0] == '1' ? true : false;
  
  if (filled)
  {
    tft.fillTriangle(x1,y1,x2,y2,x3,y3,fg);
  }
  else
  {
    tft.drawTriangle(x1,y1,x2,y2,x3,y3,fg);
  }
}


void functPixel(CmdParser *myParser) 
{ 
  DEBUG_DUMPPARAMS(myParser);

  //P,X,Y,FG
  if (myParser->getParamCount() != 4)
  {
    DEBUG_PRINTLN("PARAMERROR");
    return;
  }

  int paramIndex = 1;
  
  uint16_t x = atoi(myParser->getCmdParam(paramIndex++));
  uint16_t y = atoi(myParser->getCmdParam(paramIndex++));

  uint16_t fg = (uint16_t) strtol(myParser->getCmdParam(paramIndex++), 0, 16);

  tft.drawPixel(x,y,fg);
}

void functLine(CmdParser *myParser) 
{ 
  DEBUG_DUMPPARAMS(myParser);

  //P,X1,Y1,X2,Y2,FG
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
  uint16_t fg = (uint16_t) strtol(myParser->getCmdParam(paramIndex++), 0, 16);

  tft.drawLine(x1,y1,x2,y2,fg);
}
