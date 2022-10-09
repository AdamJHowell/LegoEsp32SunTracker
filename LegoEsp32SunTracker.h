/**
 * Created by Adam on 2022-10-08.
 */

#ifndef LEGO_ESP32_SUN_TRACKER_H
#define LEGO_ESP32_SUN_TRACKER_H

#include <Arduino.h>		  // The built-in Arduino library.
#include <ESP32Servo.h>	  // The servo library to use.
#include "privateInfo.h"  // I use this file to hide my network information from random people on GitHub.
#include <WiFiUdp.h>		  // OTA
#include <ArduinoOTA.h>	  // OTA - The Arduino OTA library.  Specific version of this are installed along with specific boards in board manager.
#include <ArduinoJson.h>  // A JSON processing library.  Author: Benoît Blanchon  https://arduinojson.org/
#include <PubSubClient.h> // PubSub is the MQTT API maintained by Nick O'Leary: https://github.com/knolleary/pubsubclient
#include <WiFiClient.h>	  // Provides the WiFiClient class needed for MQTT.


/**
 * Servo parameters.
 * Depending on your servo make, the pulse width min and max may vary.
 * Adjust these to be as small/large as possible without hitting the hard stop for max range.
 */
int azimuthPosition = 90;	 // Holds the current position of the azimuth servo.  The default of 1500 is motionless.
int azimuthServoPin = 23;	 // The GPIO which the azimuth servo connects to.
int elevationPosition = 90; // Holds the current position of the azimuth servo.  The default of 90 is directly up.
int elevationServoPin = 22; // The GPIO which the elevation servo connects to.
int minPulseWidth = 500;	 // The minimum pulse width to use with servos.
int maxPulseWidth = 2500;	 // The maximum pulse width to use with servos.


/**
 * Network data
 * If you do not use the networkVariables.h file to hold your network information, you will need to set these four consts to suit your needs.
 */
//const char *wifiSsid = "nunya";
//const char *wifiPassword = "nunya";
//const char *mqttBroker = "127.0.0.1";
//const int mqttPort = 1883;
const char *HOST_NAME = "LegoEsp32SunTracker";				  // The hostname used for OTA access.
const char *SKETCH_NAME = "LegoEsp32SunTracker.ino";		  // The name used when publishing stats.
const char *NOTES = "The ESP32 sun tracker";					  // The hostname used for OTA access.
const char *MQTT_STATS_TOPIC = "sunTracker/stats";			  // The topic this device will publish to upon connection to the broker.
const char *MQTT_COMMAND_TOPIC = "sunTracker/commands";	  // The command topic this device will subscribe to.
const char *sketchTopic = "sunTracker/sketch";				  // The topic used to publish the sketch name (__FILE__).
const char *macTopic = "sunTracker/mac";						  // The topic used to publish the MAC address.
const char *ipTopic = "sunTracker/ip";							  // The topic used to publish the IP address.
const char *rssiTopic = "sunTracker/rssi";					  // The topic used to publish the WiFi Received Signal Strength Indicator.
const char *publishCountTopic = "sunTracker/publishCount"; // The topic used to publish the loop count.
const char *notesTopic = "sunTracker/notes";					  // The topic used to publish notes relevant to this project.
const unsigned long JSON_DOC_SIZE = 512;						  // The ArduinoJson document size, and size of some buffers.
char ipAddress[16];													  // A character array to hold the IP address.
char macAddress[18];													  // A character array to hold the MAC address, and append a dash and 3 numbers.
long rssi;																  // A global to hold the Received Signal Strength Indicator.
unsigned int networkIndex = 2112;								  // An unsigned integer to hold the correct index for the network arrays: wifiSsidArray[], wifiPassArray[], mqttBrokerArray[], and mqttPortArray[].
unsigned int wifiConnectionTimeout = 10000;					  // Set the Wi-Fi connection timeout to 10 seconds.
unsigned int mqttReconnectInterval = 3000;					  // Set the delay between MQTT broker connection attempts to 3 seconds.
unsigned int sensorPollDelay = 10000;							  // How long to wait between sensor polling.
unsigned int publishDelay = 60000;								  // How long to wait between MQTT publishes.
unsigned int callbackCount = 0;									  // The number of times a callback was received.
unsigned long publishCount = 0;									  // A counter of how many times the stats have been published.
unsigned long lastPollTime = 0;									  // The last time sensors were polled.
unsigned long lastPublishTime = 0;								  // The last time a MQTT publish was performed.


/**
 * Class objects.
 */
struct WiFiClient espClient;			  // Create a WiFiClient to connect to the local network.
PubSubClient mqttClient( espClient ); // Create a PubSub MQTT client object that uses the WiFiClient.
Servo azimuthServo;						  // A Servo class object for controlling the azimuth direction and speed.
Servo elevationServo;					  // A Servo class object for controlling the elevation position.


/**
 * Function declarations.
 */
void onReceiveCallback( char *topic, byte *payload, unsigned int length );
void configureOTA();
void wifiMultiConnect();
int checkForSSID( const char *ssidName );
int mqttMultiConnect( int maxAttempts );
void publishStats();
void readTelemetry();
void printTelemetry();

#endif // LEGO_ESP32_SUN_TRACKER_H
