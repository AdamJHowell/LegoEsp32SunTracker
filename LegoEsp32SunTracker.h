/**
 * Created by Adam on 2022-10-08.
 */

#ifndef LEGO_ESP32_SUN_TRACKER_H
#define LEGO_ESP32_SUN_TRACKER_H

#ifdef ESP8266
// These headers are installed when the ESP8266 is installed in board manager.
#include "ESP8266WiFi.h" // ESP8266 WiFi support.  https://github.com/esp8266/Arduino/tree/master/libraries/ESP8266WiFi
#include <ESP8266mDNS.h> // OTA - mDNSResponder (Multicast DNS) for the ESP8266 family.
#elif ESP32
// These headers are installed when the ESP32 is installed in board manager.
#include "WiFi.h"		// ESP32 Wifi support.  https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/src/WiFi.h
#include <ESPmDNS.h> // OTA - Multicast DNS for the ESP32.
#else
#include "WiFi.h" // Arduino Wifi support.  This header is part of the standard library.  https://www.arduino.cc/en/Reference/WiFi
#endif
#include <Arduino.h>		  // The built-in Arduino library.
#include <ESP32Servo.h>	  // The servo library to use.
#include "privateInfo.h"  // I use this file to hide my network information from random people on GitHub.
#include <WiFiUdp.h>		  // OTA
#include <ArduinoOTA.h>	  // OTA - The Arduino OTA library.  Specific version of this are installed along with specific boards in board manager.
#include <ArduinoJson.h>  // A JSON processing library.  Author: Benoît Blanchon  https://arduinojson.org/
#include <PubSubClient.h> // PubSub is the MQTT API maintained by Nick O'Leary: https://github.com/knolleary/pubsubclient
#include <WiFiClient.h>	  // Provides the WiFiClient class needed for MQTT.


/**
 * Function declarations.
 */
void onReceiveCallback( char *topic, byte *payload, unsigned int length );
void configureOTA();
void wifiMultiConnect();
int checkForSSID( const char *ssidName );
int mqttMultiConnect( int maxAttempts );
void setAltitude( int angle );
void setAzimuthSpeed( int speed );
void azimuthDemo();
void publishStats();
void readTelemetry();  // Not yet implemented.
void printTelemetry(); // Not yet implemented.
void publishTelemetry();


/**
 * Class objects.
 */
struct WiFiClient espClient;			  // Create a WiFiClient to connect to the local network.
PubSubClient mqttClient( espClient ); // Create a PubSub MQTT client object that uses the WiFiClient.
Servo azimuthServo;						  // A Servo class object for controlling the azimuth direction and speed.
Servo altitudeServo;						  // A Servo class object for controlling the altitude position.


/**
 * Global servo parameters.
 * Depending on your servo make, the pulse width minimum and maximum may vary.
 * Adjust these to be as close to the mechanical limits of each servo, without hitting those actual limits.
 * For continuous rotation servos, use the declared minimum and maximum values from the specification sheet.
 * If unknown, 500 μsec is the most common minimum, and 2500 μsec is the most common maximum.
 */
int azimuthPosition = 90;	// Holds the current position of the azimuth servo.  The default of 1500 is motionless.
int azimuthServoPin = 23;	// The GPIO which the azimuth servo connects to.
int altitudePosition = 90; // Holds the current position of the azimuth servo.  The default of 90 is directly up.
int altitudeServoPin = 22; // The GPIO which the altitude servo connects to.
int minPulseWidth = 500;	// The minimum pulse width to use with servos.
int maxPulseWidth = 2500;	// The maximum pulse width to use with servos.


/**
 * Global Network constants.
 * If you do not use the privateInfo.h file to hold your network information, you will need to set these four consts to suit your needs.
 */
//const char *wifiSsidArray[2] = { "HomeWiFi", "WorkWiFi" };
//const char *wifiPassArray[2] = { "pass1", "pass2" };
//const char *mqttBrokerArray[2] = { "192.168.1.5", "10.10.10.27" };
//const int mqttPortArray[2] = { 1883, 1883 };


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


/**
 * Global variables.
 */
char ipAddress[16];								  // A character array to hold the IP address.
char macAddress[18];								  // A character array to hold the MAC address, and append a dash and 3 numbers.
long rssi;											  // A global to hold the Received Signal Strength Indicator.
unsigned int networkIndex = 2112;			  // An unsigned integer to hold the correct index for the network arrays: wifiSsidArray[], wifiPassArray[], mqttBrokerArray[], and mqttPortArray[].
unsigned int wifiConnectionTimeout = 10000; // Set the Wi-Fi connection timeout to 10 seconds.
unsigned int mqttReconnectInterval = 3000;  // Set the delay between MQTT broker connection attempts to 3 seconds.
unsigned int telemetryPollInterval = 10000; // How long to wait between sensor polling.
unsigned int publishInterval = 60000;		  // How long to wait between MQTT publishes.
unsigned int callbackCount = 0;				  // The number of times a callback was received.
unsigned int MCU_LED = 2;						  // The GPIO which the onboard LED is connected to.
unsigned long publishCount = 0;				  // A counter of how many times the stats have been published.
unsigned long lastTelemetryPollTime = 0;	  // The last time sensors were polled.
unsigned long lastPublishTime = 0;			  // The last time a MQTT publish was performed.

#endif // LEGO_ESP32_SUN_TRACKER_H
