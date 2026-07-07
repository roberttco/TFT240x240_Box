Code for a utility TFT display controlled via MQTT.  This is targetd as a Wemos D1 mini connected to a 240x240 ST7789 display.  

# Credentials
Create the file include/secrets.h with the contents
```
#include <EspMQTTClient.h>
EspMQTTClient client(
  "WIFI_SSID",
  "WIFI_PASSWORD",
  "MQTT_BROKER_IP",
  "MQTT_CLIENT_ID",
  "MQTT_PORT"
);

#define MQTT_LWT_TOPIC "MQTT last will topic"
#define MQTT_TXT_TOPIC "MQTT topic for sending content"
#define MQTT_CMD_TOPIC "MQTT topic for sending display commands"
#define MQTT_INFO_TOPIC " MQTT topic where command output is sent"
#define MQTT_STATE_TOPIC "MQTT topic for receiving state form the display"
```

# MQTT API
## Status
Device online status is sent to via the MQTT_LWT_TOPIC and MQTT_STATE_TOPIC topics.  These are typically the same topic but receive the messages "Offline" and "Online", respectively.

## Box Commands

- 0, 1, 2, 3 - set display rotation to 0&deg;, 90&deg;, 180&deg; or 270&deg;, respectively.
- 254 - get device info as a JSON object.
- 255 - clear display to black.

### Device information (254)
The device informaiton is returned via MQTT to the topics rooted at _MQTT_INFO_TOPIC_ as JSON objects.
* `MQTT_INFO_TOPIC/display` The display dimensions.
* `MQTT_INFO_TOPIC/build` Firmware build date and time.
* `MQTT_INFO_TOPIC/network` Current network connection information (IP and MAC address).
* `MQTT_INFO_TOPIC/fonts` List of available fonts.

## Content Commands
- The display origin is at the top left corner.
- When a color argument is specified, the color is a RGB565 color value represented as a 4 digit hexadecimal number.
- When a font is specified, it is the name of the font as returned by command 4.  
- _All_ content command parameters are double quoted (e.g. "F","0x1234")

### Fill ("F","color")
Fill the entire display with the specified color.

### String ("S","X","Y","FG","BG","S","MSG")
Render the string __MSG__ using the last specified font at the coordinates specified by __X__ and __Y__.  The text will be rentered in the color specified by __FG__ on a __BG__ background.  the font will be scaled by __S__ times.

__Important__: this command will only draw the background when using _system_ fonts.  The preferred method for rendering strings is to use the __String Extended__ command.

### String Extended ("S","X","Y","FG","BG","FONT", "S","MSG")
Render the string __MSG__ using the font __FONT__ at the coordinates specified by __X__ and __Y__.  The text will be rentered in the color specified by __FG__ on a __BG__ background.  the font will be scaled by __S__ times.

### Rectangle ("R","X","Y","W","H","FG","R","F","RR")
Render a rectangle that is __W__ pixels wide and __H__ pixels high with the top left corner at the coordinates specified by __X__ and __Y__.  The rectangle will be rendered in the color __FG__.  The rectangle will be filled if __F__ is 1.  If __R__ is 1, the rectangle will be filled and __RR__ is required to specify the rounding radius (__RR__ is ignored/not required if __R__ is 0 )

### Circle ("C","XC","YC","FG","R","F")
Render an __FG__ colored circle whose center is __XC__,__YC__ with a radius __R__.  The circle will be filled if __F__ is 1.

### Triangle ("T","X1","Y1","X2","Y2","X3","Y3","FG","F")
Render a triangle whose three vertices are specified by __Xn__ and __Yn__.  The rectangle will be colored __FG__ and will be filled if __F__ is 1.

### Pixel ("P","X","Y","FG")
Draw a pixel at __X__,__Y__ in the color __FG__.

### Line ("L","X1","Y1","X2","Y2","FG")
Draw an __FG__ colored line between __X1__,__Y1__ and __X2__,__Y2__.

# Firmware updates
Firmware can be updated using the embedded web based updater.  Navigate to the devices IP address and upload the file.  There is a file system upload option also.  It is not used at this time.
 
# Future improvements
- Use a command structure that allows for rendering multiple shapes in one MQTT message
- Render compressed images sent via MQTT (or maybe downloaded via HTTP)
- Other stuff ...