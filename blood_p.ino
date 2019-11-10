#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

Adafruit_BMP085 bmp;
/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "manasi"
#define WLAN_PASS       "manasi123"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "manasi27"
#define AIO_KEY         "b95ff393f6a84df5a28019a4ae7473d9"

/************ Global State (you don't need to change this!) ******************/
WiFiClient client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);
/****************************** Feeds ***************************************/

// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish flfeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/BMP_180");
Adafruit_MQTT_Publish flfeed1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/BMP_pressure");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe dash = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/dashboards/blood_pressure");

/*************************** Sketch Code ************************************/

void MQTT_connect();

void setup() {
  Serial.begin(9600);
  if (!bmp.begin()) {
  Serial.println("Could not find a valid BMP085/BMP180 sensor, check wiring!");
  while (1) {}
  }

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  mqtt.subscribe(&dash);
  
}

uint32_t x=0;
  
void loop() {
    
  MQTT_connect();

  Serial.print("Temperature = ");
  Serial.print(bmp.readTemperature());
  Serial.println(" *C");
  int temp = bmp.readTemperature();
    
  Serial.print("Pressure = ");
  Serial.print(bmp.readPressure());
  Serial.println(" Pa");
  int pressure = bmp.readPressure();  
  
  Serial.println();
  delay(5000);

  Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &dash) {
      Serial.print(F("Got: "));
      //Serial.println((char *)onoffbutton.lastread);
      char * v = (char *)dash.lastread;
      Serial.print(v);
      Serial.println(" ");

    }
  }
  
flfeed.publish(temp);
flfeed1.publish(pressure);
}


void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(500);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
