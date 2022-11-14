/**
 * LegoEsp32SunTracker is a simple sun tracker built with an ESP32 and Lego Technic components.
 * See README.md for details.
 */
#include "LegoEsp32SunTracker.h"


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

	//	Check the MQTT connection, and reconnect if needed.
	if( !mqttClient.connected() )
		mqttMultiConnect( 3 );
	// The MQTT client loop() function facilitates the receiving of messages and maintains the connection to the broker.
	mqttClient.loop();

	unsigned long time = millis();
	if( lastTelemetryPollTime == 0 || ( ( time > telemetryPollInterval ) && ( time - telemetryPollInterval ) > lastTelemetryPollTime ) )
	{
		readTelemetry();
		moveArm();
		printTelemetry();
		lastTelemetryPollTime = millis();
	}

	time = millis();
	if( lastPublishTime == 0 || ( ( time > publishInterval ) && ( time - publishInterval ) > lastPublishTime ) )
	{
		publishTelemetry();
		lastPublishTime = millis();
	}
} // End of loop() function.
