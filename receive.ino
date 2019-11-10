
#include <SoftwareSerial.h>
SoftwareSerial s(D6,D5);
#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

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
Adafruit_MQTT_Publish flfeed = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/ecg");

// Setup a feed called 'onoff' for subscribing to changes.
Adafruit_MQTT_Subscribe dash = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/dashboards/measureecg");

/*************************** Sketch Code ************************************/

void MQTT_connect();

void setup() {
  Serial.begin(9600);
  s.begin(9600);
  while (!Serial) continue;
  // put your setup code here, to run once:

  
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

  StaticJsonBuffer<1000> jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(s);
 
  if (root == JsonObject::invalid())
  {
    return;
  }
  //Print the data in the serial monitor
  Serial.println("JSON received and parsed");
  root.prettyPrintTo(Serial);
  Serial.println("");
  Serial.print(" ECG Reading ");
  int data1=root["ecg"];
  Serial.println(data1);


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
  
//const * v= flowRate;
flfeed.publish(data1);

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
