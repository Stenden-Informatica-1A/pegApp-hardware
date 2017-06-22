/***************************************************
  ESP8266MOD - Adafruit
  Wasknijper-programm(Sunny)
  Author: Maryn Kaspers
  Studentnr: 345334
  Class: INF1A
  College: Stenden Hogeschool Emmen
 ****************************************************/

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include "WiFiManager.h"

/************************* MQTT setup  *********************************/
#define SERVER      "broker.hivemq.com"
#define SERVERPORT  1883
#define USERNAME    "Wasknijper"
int battery = 0;
String batteryText = "";
String messageText = "";
String topic = WiFi.macAddress();

/************************* Global State  ******************************/
// Create an WiFiClient class to connect to the MQTT server.
WiFiClient client;
// Setup the MQTT client class by passing in the WiFiClient data.
Adafruit_MQTT_Client mqtt(&client, SERVER, SERVERPORT, USERNAME);

/**************************** Publish *********************************/

// een feed opzetten genaamd 'WasknijperSunny' om te publishen naar de topic met als naam het macadress.
Adafruit_MQTT_Publish WasknijperSunny = Adafruit_MQTT_Publish(&mqtt, topic.c_str());

///************************ Sketch Code *****************************/
void MQTT_connect();

void configModeCallback (WiFiManager *myWiFiManager) {
  Serial.println("Entered config mode");
  Serial.println(WiFi.softAPIP());
  //if you used auto generated SSID, print it
  Serial.println(myWiFiManager->getConfigPortalSSID());
}

//setup moisturesensor
int sensorPin = A0;// select the input pin for the potentiometer
int sensorValue = 0;

void setup()
{
  Serial.begin(115200);
  Serial.setTimeout(2000);
  delay(10);
  WiFiManager wifiManager;
  //reset settings - for testing
  //  wifiManager.resetSettings();

  //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
  wifiManager.setAPCallback(configModeCallback);

  //fetches ssid and pass and tries to connect
  //if it does not connect it starts an access point with the specified name
  //here
  //  "AutoConnectAP"
  //and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect())
  {
    Serial.println("failed to connect and hit timeout");
    //reset and try again, or maybe put it to deep sleep
    ESP.reset();
    delay(1000);
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  Serial.println(topic);

}

void loop()
{
  //Connecting with MQTT_connect()
  MQTT_connect();
  delay(2000);

  // Here we publish the sensor value(and the battery level optional)
  battery = 80;
  batteryText = "/" + String(battery);
  sensorValue = analogRead(sensorPin);
  messageText = String(sensorValue) + batteryText;

  Serial.print(F("\nSending WasknijperSunny sensorvalue and batterylevel: "));
  Serial.print(messageText);
  Serial.print("...");
  if (! WasknijperSunny.publish(messageText.c_str() ))
  {
    Serial.println(F("Failed!"));
  }
  else
  {
    Serial.println(F("Sent!"));
  }

  Serial.println("Going into deep sleep for 10 seconds");
  ESP.deepSleep(10e6, WAKE_RF_DEFAULT); // 20e6 is 20 microsecondsS
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect()
{
  int8_t ret;
  // Stop if already connected.
  if (mqtt.connected())
  {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0)  // connect will return 0 for connected
  {
    Serial.println(mqtt.connectErrorString(ret));
    Serial.println("Retrying MQTT connection in 5 seconds...");
    mqtt.disconnect();
    delay(5000);  // wait 5 seconds
    retries--;
    if (retries == 0)
    {
      // basically die and wait for WDT to reset me
      while (1);
    }
  }
  Serial.println("MQTT Connected!");
}
