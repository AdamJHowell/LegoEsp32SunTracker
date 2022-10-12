#include "LegoEsp32SunTracker.h"

void setup()
{
	// Start the Serial communication to send messages to the computer.
	Serial.begin( 115200 );
	if( !Serial )
		delay( 500 );
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

#ifdef AJH_MULTI_CONNECT
	wifiMultiConnect();
	const char *mqttBroker = mqttBrokerArray[networkIndex];
	const int mqttPort = mqttPortArray[networkIndex];
#else
	wifiConnect();
#endif

	// Set the MQTT client parameters.
	mqttClient.setServer( mqttBroker, mqttPort );
	// Assign the onReceiveCallback() function to handle MQTT callbacks.
	mqttClient.setCallback( onReceiveCallback );
	Serial.print( "Using MQTT broker: " );
	Serial.println( mqttBroker );
	Serial.print( "Using MQTT port: " );
	Serial.println( mqttPort );

	configureOTA();

	Serial.println( "setup() has finished." );
} // End of setup() function.


void loop()
{
	//	ArduinoOTA.handle();
	if( !mqttClient.connected() )
	{
#ifdef AJH_MULTI_CONNECT
		mqttMultiConnect( 3 );
#else
		mqttConnect( 3 );
#endif
	}
	// The loop() function facilitates the receiving of messages and maintains the connection to the broker.
	mqttClient.loop();

	unsigned long time = millis();
	if( lastTelemetryPollTime == 0 || ( ( time > telemetryPollInterval ) && ( time - telemetryPollInterval ) > lastTelemetryPollTime ) )
	{
		readTelemetry();
		printTelemetry();
		lastTelemetryPollTime = millis();

		if( altitudePosition < 90 )
			altitudePosition += 45;
		else
			altitudePosition = 0;
		setAltitude( altitudePosition );
	}

	time = millis();
	if( lastPublishTime == 0 || ( ( time > publishInterval ) && ( time - publishInterval ) > lastPublishTime ) )
	{
		publishTelemetry();
		lastPublishTime = millis();

		if( azimuthSpeed >= 10 )
			azimuthSpeed = -10;
		else
			azimuthSpeed += 10;
		setAzimuthSpeed( azimuthSpeed );
	}
} // End of loop() function.
