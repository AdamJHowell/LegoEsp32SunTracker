#include "LegoEsp32SunTracker.h"

void setup()
{
	// Start the Serial communication to send messages to the computer.
	Serial.begin( 115200 );
	if( !Serial )
		delay( 500 );

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

	azimuthSpeed = 50;
} // End of setup() function.


void azimuthDemo()
{
	digitalWrite( MCU_LED, HIGH );
	Serial.printf( "LED is %d\n", digitalRead( MCU_LED ) );

	// Ramp up from 90 to 180.
	for( azimuthSpeed = 90; azimuthSpeed < 170; azimuthSpeed += 1 )
	{
		azimuthServo.writeMicroseconds( azimuthSpeed );
		delay( 20 );
	}
	Serial.printf( "Servo at %d\n", azimuthSpeed );
	delay( 3000 );
	digitalWrite( MCU_LED, LOW );
	Serial.printf( "LED is %d\n", digitalRead( MCU_LED ) );

	// Ramp down from 180 to 90.
	for( azimuthSpeed = 170; azimuthSpeed > 90; azimuthSpeed -= 1 )
	{
		azimuthServo.writeMicroseconds( azimuthSpeed );
		delay( 20 );
	}
	Serial.printf( "Servo at %d\n", azimuthSpeed );
	delay( 3000 );
	digitalWrite( MCU_LED, HIGH );
	Serial.printf( "LED is %d\n", digitalRead( MCU_LED ) );

	// Ramp down from 90 to 0.
	for( azimuthSpeed = 90; azimuthSpeed > 10; azimuthSpeed -= 1 )
	{
		azimuthServo.writeMicroseconds( azimuthSpeed );
		delay( 20 );
	}
	Serial.printf( "Servo at %d\n", azimuthSpeed );
	delay( 3000 );
	digitalWrite( MCU_LED, LOW );
	Serial.printf( "LED is %d\n", digitalRead( MCU_LED ) );

	// Ramp up from 0 to 90.
	for( azimuthSpeed = 10; azimuthSpeed < 90; azimuthSpeed += 1 )
	{
		azimuthServo.writeMicroseconds( azimuthSpeed );
		delay( 20 );
	}
	Serial.printf( "Servo at %d\n", azimuthSpeed );
	delay( 3000 );
} // End of azimuthDemo() function.


void loop()
{
	ArduinoOTA.handle();
	if( !mqttClient.connected() )
		mqttMultiConnect( 5 );
	// The loop() function facilitates the receiving of messages and maintains the connection to the broker.
	mqttClient.loop();

	unsigned long time = millis();
	if( lastTelemetryPollTime == 0 || ( ( time > telemetryPollInterval ) && ( time - telemetryPollInterval ) > lastTelemetryPollTime ) )
	{
		readTelemetry();
		printTelemetry();
		lastTelemetryPollTime = millis();
		azimuthSpeed = -azimuthSpeed;
		setAzimuthSpeed( azimuthSpeed );
	}

	time = millis();
	if( lastPublishTime == 0 || ( ( time > publishInterval ) && ( time - publishInterval ) > lastPublishTime ) )
	{
		publishTelemetry();
		lastPublishTime = millis();
		if( altitudePosition < 90 )
			altitudePosition += 45;
		else
			altitudePosition = 0;
		setAltitude( altitudePosition );
	}
} // End of loop() function.
