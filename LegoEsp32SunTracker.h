/**
 * Created by Adam on 2022-10-08.
 */

#ifndef LEGO_ESP32_SUN_TRACKER_H
#define LEGO_ESP32_SUN_TRACKER_H

#ifdef ESP8266
// These headers are installed when the ESP8266 is installed in board manager.
#include <ESP8266WiFi.h> // ESP8266 WiFi support.  https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi
#include <ESP8266mDNS.h> // OTA - mDNSResponder (Multicast DNS) for the ESP8266 family.
#elif ESP32
// These headers are installed when the ESP32 is installed in board manager.
#include <WiFi.h>		// ESP32 Wifi support.  https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/src/WiFi.h
#include <ESPmDNS.h> // OTA - Multicast DNS for the ESP32.
#else
#include <WiFi.h> // Arduino Wi-Fi support.  This header is part of the standard library.  https://www.arduino.cc/en/Reference/WiFi
#endif
#include <WiFiUdp.h>		  // OTA - The Arduino OTA library.  Specific version of this are installed along with specific boards in board manager.
#include <ArduinoOTA.h>	  // OTA - The Arduino OTA library.  Specific version of this are installed along with specific boards in board manager.
#include <ArduinoJson.h>  // A JSON processing library.  Author: Benoît Blanchon  https://arduinojson.org/
#include <PubSubClient.h> // PubSub is the MQTT API maintained by Nick O'Leary: https://github.com/knolleary/pubsubclient
#include "privateInfo.h"  // I use this file to hide my network information from random people on GitHub.
#include "ServoFunctions.h"


/**
 * Function declarations.
 */
// NetworkFunctions.ino
void onReceiveCallback( char *topic, byte *payload, unsigned int length );
void configureOTA();
int checkForSSID( const char *ssidName );
void wifiMultiConnect();
int mqttMultiConnect( int maxAttempts );
void publishStats();
void publishTelemetry();
void lookupWifiCode( int code, char *buffer );
void lookupMQTTCode( int code, char *buffer );
// LegoEsp32SunTracker.ino
void readTelemetry();  // Not yet implemented.
void printTelemetry(); // Not yet implemented.


/**
 * Class objects.
 */
struct WiFiClient espClient;			  // Create a WiFiClient to connect to the local network.
PubSubClient mqttClient( espClient ); // Create a PubSub MQTT client object that uses the WiFiClient.


/**
 * Global Network constants.
 * If you do not use the privateInfo.h file to hold your network information, you will need to set these five lines to suit your needs.
 */
//#define ELEMENT_COUNT 2
//const char *wifiSsidArray[ELEMENT_COUNT] = { "HomeWiFi", "WorkWiFi" };
//const char *wifiPassArray[ELEMENT_COUNT] = { "pass1", "pass2" };
//const char *mqttBrokerArray[ELEMENT_COUNT] = { "192.168.1.5", "10.10.10.27" };
//const int mqttPortArray[ELEMENT_COUNT] = { 1883, 1883 };


/**
 * Other global constants.
 */
const char *HOST_NAME = "LegoEsp32SunTracker";				  // The hostname used for OTA access.
const char *SKETCH_NAME = "LegoEsp32SunTracker.ino";		  // The name used when publishing stats.
const char *NOTES = "The ESP32 sun tracker";					  // The hostname used for OTA access.
const char *MQTT_STATS_TOPIC = "sunTracker/stats";			  // The topic this device will publish to upon connection to the broker.
const char *MQTT_COMMAND_TOPIC = "sunTracker/commands";	  // The command topic this device will subscribe to.
const char *sketchTopic = "sunTracker/sketch";				  // The topic used to publish the sketch name (__FILE__).
const char *macTopic = "sunTracker/mac";						  // The topic used to publish the MAC address.
const char *ipTopic = "sunTracker/ip";							  // The topic used to publish the IP address.
const char *rssiTopic = "sunTracker/rssi";					  // The topic used to publish the Wi-Fi Received Signal Strength Indicator.
const char *publishCountTopic = "sunTracker/publishCount"; // The topic used to publish the loop count.
const char *notesTopic = "sunTracker/notes";					  // The topic used to publish notes relevant to this project.
const unsigned long JSON_DOC_SIZE = 512;						  // The ArduinoJson document size, and size of some buffers.
const int upperLeftGPIO = 36;
const int upperRightGPIO = 39;
const int lowerLeftGPIO = 34;
const int lowerRightGPIO = 35;
int upperLeftValue = 0;
int upperRightValue = 0;
int lowerLeftValue = 0;
int lowerRightValue = 0;


/**
 * Global variables.
 */
char ipAddress[16];									 // A character array to hold the IP address.
char macAddress[18];									 // A character array to hold the MAC address, and append a dash and 3 numbers.
long rssi = 0;											 // A global to hold the Received Signal Strength Indicator.
unsigned int networkIndex = 2112;				 // An unsigned integer to hold the correct index for the network arrays: wifiSsidArray[], wifiPassArray[], mqttBrokerArray[], and mqttPortArray[].
unsigned int wifiConnectionTimeout = 10000;	 // Set the Wi-Fi connection timeout to 10 seconds.
unsigned int mqttReconnectInterval = 3000;	 // Set the minimum time between sequential MQTT broker connection attempts to 3 seconds.
unsigned int mqttReconnectCooldown = 20000;	 // Set the minimum time between calls to mqttMultiConnect() to 20 seconds.
unsigned int telemetryPollInterval = 50;		 // How long to wait between sensor polling.
unsigned int telemetryProcessInterval = 1000; // How long to wait between sensor processing.
unsigned int publishInterval = 60000;			 // How long to wait between MQTT publishes.
unsigned int callbackCount = 0;					 // The number of times a callback was received.
unsigned int MCU_LED = 2;							 // The GPIO which the onboard LED is connected to.
unsigned long publishCount = 0;					 // A counter of how many times the stats have been published.
unsigned long lastTelemetryPollTime = 0;		 // The last time sensors were polled.
unsigned long lastTelemetryProcessTime = 0;	 // The last time sensor data was acted on.
unsigned long lastPublishTime = 0;				 // The last time a MQTT publish was performed.
unsigned long lastMqttConnectionTime = 0;		 // The last time a MQTT connection was attempted.

#endif // LEGO_ESP32_SUN_TRACKER_H
