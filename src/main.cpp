#include "app.h"

#include <CmdCallback.hpp>
#include <CmdParser.hpp>
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789

#include "fonts.h"
#include "primitives.h"

// See README.md for information on the credentials
#include "secrets.h"

// initialize ST7789 TFT library with hardware SPI module (e.g. use hardware SPI pins)
// ST7789 TFT module connections
#define TFT_DC D1  // TFT DC  pin is connected to NodeMCU pin D1 (GPIO5)
#define TFT_RST D2 // TFT RST pin is connected to NodeMCU pin D2 (GPIO4)
#define TFT_CS -1  // TFT CS  pin is connected to NodeMCU pin D8 (GPIO15)

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

// command parser stuff
CmdCallback<8> cmdCallbackQueue;
CmdParser myParser;

int rotation = 0;
bool connecting = true;
bool waiting = false;

void setup(void)
{
  Serial.begin(115200);

  // if the display has CS pin try with SPI_MODE0
  tft.init(240, 240, SPI_MODE3); // Init ST7789 display 240x240 pixel
  tft.setSPISpeed(40000000);

  tft.setRotation(rotation);
  tft.fillScreen(ST77XX_BLACK);
  tft.setTextWrap(false);

  myParser.setOptIgnoreQuote(false); // default

  ////
  // Set command seperator.
  myParser.setOptSeperator(','); // default

  cmdCallbackQueue.addCmd("F", &functFill);
  cmdCallbackQueue.addCmd("S", &functString);
  cmdCallbackQueue.addCmd("SX", &functStringExtended);
  cmdCallbackQueue.addCmd("R", &functRectangle);
  cmdCallbackQueue.addCmd("C", &functCircle);
  cmdCallbackQueue.addCmd("T", &functTriangle);
  cmdCallbackQueue.addCmd("P", &functPixel);
  cmdCallbackQueue.addCmd("L", &functLine);

  // Enable debugging messages sent to serial output

#ifdef DEBUG
  client.enableDebuggingMessages();
#endif

  // Enable the web updater. User and password default to values of MQTTUsername and
  // MQTTPassword. These can be overridden with enableHTTPWebUpdater("user", "password").
  client.enableHTTPWebUpdater();

  client.enableLastWillMessage(MQTT_LWT_TOPIC, "Offline"); // You can activate the retain flag by setting the third parameter to true

  client.setMaxPacketSize(300);
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

    tft.drawCircle(120, 120, lastRadius, ST77XX_BLACK);
    if (waiting)
    {
      tft.drawCircle(120, 120, 110 - connectingRadius, ST77XX_ORANGE);
      lastRadius = 110 - connectingRadius;
    }
    else
    {
      tft.drawCircle(120, 120, connectingRadius, ST77XX_WHITE);
      lastRadius = connectingRadius;
    }

    delay(70);
  }
}

void onConnectionEstablished()
{
  // Subscribe to MQTT_TXT_TOPIC and display received message to Serial
  client.subscribe(MQTT_TXT_TOPIC, [](const String &payload)
                   {
    if (waiting)
    {
      tft.fillScreen(ST77XX_BLACK);
      waiting = false;
    }

    DEBUG_PRINTF("MQTT_TXT_TOPIC payload: %s\r\n",payload.c_str());
    
    char *p = (char *)payload.c_str();
    char *i;
    char *token = strtok_r(p,"|",&i);
    while (token != NULL)
    {
      if (myParser.parseCmd(token) != CMDPARSER_ERROR)
      {
        cmdCallbackQueue.processCmd(&myParser);
      }
      else
      {
        DEBUG_PRINTF("Command parse error of token %s\r\n.",token);
      }

      token = strtok_r(NULL,"|",&i);
    } });

  client.subscribe(MQTT_CMD_TOPIC, [](const String &payload)
                   {
                      if (waiting)
                      {
                        tft.fillScreen(ST77XX_BLACK);
                        waiting = false;
                      }
                    
                      DEBUG_PRINTF("MQTT_CMD_TOPIC payload: %s\r\n",payload.c_str());

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
                        case 254: // get device info
                        {
                          client.publish(MQTT_INFO_TOPIC + String("/display"),"{\"display\": {\"width\":" + String(tft.width()) + ",\"height\":" + String(tft.height()) + "}}");
                          client.publish(MQTT_INFO_TOPIC + String("/build"),"{\"build\": {\"date\":\"" + String(__DATE__) + "\",\"time\":\"" + __TIME__ + "\"}}");
                          client.publish(MQTT_INFO_TOPIC + String("/network"),"{\"network\": {\"ip\":\"" + WiFi.localIP().toString() + "\",\"mac\":\"" + WiFi.macAddress() + "\"}}");

                          char *list = getFontNamesAsJson();
                          client.publish(MQTT_INFO_TOPIC + String("/fonts"),"{\"fonts\":" + String(list) + "}");
                          free(list);
                        }
                          break;
                        default:
                          DEBUG_PRINTLN("  * Unhandled command");
                          break; 
                      } });

  client.publish(MQTT_STATE_TOPIC, "Online"); // You can activate the retain flag by setting the third parameter to true

  connecting = false;
  waiting = true;
}
