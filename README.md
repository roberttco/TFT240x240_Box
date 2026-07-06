Code for a utility TFT display controlled via MQTT.  This is targetd as a Wemos D1 mini connected to a 240x240 ST7789 display.  

*Credentials
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
```

Alternately, you can edit main.cpp to include the same syntax.  Just remember to not commit that to Github lest people know your WiFi credentials.
 
