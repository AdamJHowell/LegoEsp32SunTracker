/**
 * LegoEsp32SunTracker is a simple sun tracker built with an ESP32 and Lego Technic components.
 * See README.md for details.
 */
#include "LegoEsp32SunTracker.h"


// ToDo: Move the altitude proportional to the upperSum/lowerSum delta.
// ToDo: Move the azimuth proportional to the leftSum/RightSum delta.


/**
 * @brief readTelemetry() will gather basic device information and store those values in global variables.
 */
void readTelemetry()
{
	rssi = WiFi.RSSI();
	upperLeftValue = analogRead( upperLeftGPIO );
	upperRightValue = analogRead( upperRightGPIO );
	lowerLeftValue = analogRead( lowerLeftGPIO );
	lowerRightValue = analogRead( lowerRightGPIO );
} // End of readTelemetry() function.


/**
 * @brief printTelemetry() will print basic device information to the serial port.
 */
void printTelemetry()
{
	Serial.println();
	Serial.printf( "MAC address: %s\n", macAddress );
	int wifiStatusCode = WiFi.status();
	char buffer[29];
	lookupWifiCode( wifiStatusCode, buffer );
	Serial.printf( "Wi-Fi status text: %s\n", buffer );
	Serial.printf( "Wi-Fi status code: %d\n", wifiStatusCode );
	if( wifiStatusCode == 3 )
	{
		Serial.printf( "IP address: %s\n", ipAddress );
		Serial.printf( "RSSI: %ld\n", rssi );
	}
	if( networkIndex != 2112 )
		Serial.printf( "Broker: %s:%d\n", mqttBrokerArray[networkIndex], mqttPortArray[networkIndex] );
	int mqttStateCode = mqttClient.state();
	lookupMQTTCode( mqttStateCode, buffer );
	Serial.printf( "MQTT state: %s\n", buffer );
	Serial.printf( "ESP32 hostname: %s\n", HOST_NAME );
	Serial.printf( "Sketch file name: %s\n", __FILE__ );
	Serial.printf( "Notes: %s\n", NOTES );
	Serial.printf( "Publish count: %ld\n", publishCount );
	Serial.printf( "Altitude servo position: %d\n", altitudePosition );
	Serial.printf( "Azimuth servo speed: %d\n", azimuthSpeed );
	Serial.println();
	int upperSum = upperLeftValue + upperRightValue;
	int lowerSum = lowerLeftValue + lowerRightValue;
	int leftSum = upperLeftValue + lowerLeftValue;
	int rightSum = upperRightValue + lowerRightValue;
	Serial.println( "Analog readings:" );
	Serial.println( "/-------------\\" );
	Serial.printf( "| %4d | %4d | = %4d\n", upperLeftValue, upperRightValue, upperSum );
	Serial.printf( "---------------\n" );
	Serial.printf( "| %4d | %4d | = %4d\n", lowerLeftValue, lowerRightValue, lowerSum );
	Serial.printf( "\\-------------/\n" );
	Serial.println( "    |      |" );
	Serial.printf( "  %4d | %4d\n", leftSum, rightSum );
	Serial.println();

	if( abs( upperSum - lowerSum ) > 50 )
	{
		if( upperSum > lowerSum )
			Serial.println( "Move up!" );
		else
			Serial.println( "Move down!" );
	}
	else
		Serial.println( "Altitude hold!" );

	if( abs( leftSum - rightSum ) > 50 )
	{
		if( leftSum > rightSum )
			Serial.println( "Move left!" );
		else
			Serial.println( "Move right!" );
	}
	else
		Serial.println( "Azimuth hold!" );
} // End of printTelemetry() function.


void setup()
{
	delay( 1000 );
	// Start the Serial communication to send messages to the computer.
	Serial.begin( 115200 );
	if( !Serial )
		delay( 1000 );
	Serial.println( "\n\nsetup() is beginning." );

	// Allocate all timers.
	ESP32PWM::allocateTimer( 0 );
	ESP32PWM::allocateTimer( 1 );
	ESP32PWM::allocateTimer( 2 );
	ESP32PWM::allocateTimer( 3 );
	// Standard 50 hz servo.
	azimuthServo.setPeriodHertz( 50 );
	// Connects the servo object to GPIO 23, sets 500 μsec as the minimum pulse width and 2500 μsec as the maximum pulse width.
	azimuthServo.attach( azimuthServoPin, minPulseWidth, maxPulseWidth );
	setAzimuthSpeed( azimuthSpeed );
	// Standard 50 hz servo.
	altitudeServo.setPeriodHertz( 50 );
	// Connects the servo object to GPIO 22, sets 500 μsec as the minimum pulse width and 2500 μsec as the maximum pulse width.
	altitudeServo.attach( altitudeServoPin, minPulseWidth, maxPulseWidth );
	setAltitude( altitudePosition );
	// Set GPIO 2 (MCU_LED) as an output.
	pinMode( MCU_LED, OUTPUT );
	// Turn the LED on.
	digitalWrite( MCU_LED, HIGH );

	// Set the MAC address variable to its value.
	snprintf( macAddress, 18, "%s", WiFi.macAddress().c_str() );

	wifiMultiConnect();

	configureOTA();

	Serial.println( "setup() has finished.\n" );
} // End of setup() function.


void loop()
{
	// Process OTA requests.
	ArduinoOTA.handle();
	unsigned long time = millis();
	// Poll the first time.  Avoid subtraction overflow.  Poll every interval.
	if( lastTelemetryPollTime == 0 || ( ( time > telemetryPollInterval ) && ( time - telemetryPollInterval ) > lastTelemetryPollTime ) )
	{
		readTelemetry();
		lastTelemetryPollTime = millis();
	}

	//	Check the MQTT connection, and reconnect if needed.
	if( !mqttClient.connected() )
		mqttMultiConnect( 1 );
	// The MQTT client loop() function facilitates the receiving of messages and maintains the connection to the broker.
	mqttClient.loop();

	time = millis();
	// Process the first time.  Avoid subtraction overflow.  Process every interval.
	if( lastTelemetryProcessTime == 0 || ( ( time > telemetryProcessInterval ) && ( time - telemetryProcessInterval ) > lastTelemetryProcessTime ) )
	{
		moveArm();
		printTelemetry();
		lastTelemetryProcessTime = millis();
	}

	time = millis();
	// Publish the first time.  Avoid subtraction overflow.  Publish every interval.
	if( lastPublishTime == 0 || ( ( time > publishInterval ) && ( time - publishInterval ) > lastPublishTime ) )
	{
		publishTelemetry();
		lastPublishTime = millis();
	}
} // End of loop() function.
