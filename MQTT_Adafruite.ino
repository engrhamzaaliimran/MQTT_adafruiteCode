/***************************************************
  This code was originally written for ESP8266
  I have modified it to work with ESP32
  This code is not using FreeRTOS
  WiFi library is an alternative to ESP8266WiFi library for ESP32 board. 
  All functions calls are the almost the same
 ****************************************************/
#include <WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include "DHT.h"        // including the library of DHT11 temperature and humidity sensor
#define DHTTYPE DHT11   // DHT 11

#define dht_dpin 5
DHT dht(dht_dpin, DHTTYPE);

/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "hotspot"
#define WLAN_PASS       "hamza455w"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "engrhamzaaliimran"
#define AIO_KEY         "aio_lJsL55X5Vu5JosvDfENxLPFcV8Bg"

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Publish photocell = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/Temp");

Adafruit_MQTT_Publish photocell2 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/humidity");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();

void setup()
{
  Serial.begin(115200);
  delay(10);

  dht.begin();
  Serial.println("Humidity and temperature\n\n");
  delay(700);
  Serial.println(F("Adafruit MQTT demo App"));

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

}



void loop() 
{
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  Serial.print("Current humidity = ");
  Serial.print(h);
  Serial.print("%      ");
  Serial.print("temperature = ");
  Serial.print(t);
  //Serial.print("...");
  if (! photocell.publish(t))
  {
    Serial.println(F("Failed"));
  }
  else
  {
    Serial.println(F("   Uploaded temp"));
  }
  if (! photocell2.publish(h))
  {
    Serial.println(F("Failed"));
  } else
  {
    Serial.println(F("Uploaded humidty"));
  }
  delay(5000);

}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
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
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0) {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
