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
	azimuthServo.write( azimuthPosition );
	// Standard 50 hz servo.
	elevationServo.setPeriodHertz( 50 );
	// Connects the servo object to GPIO 22, sets 500 μsec as the minimum pulse width and 2500 μsec as the maximum pulse width.
	elevationServo.attach( elevationServoPin, minPulseWidth, maxPulseWidth );
	elevationServo.write( elevationPosition );
	// Set GPIO 2 (MCU_LED) as an output.
	pinMode( MCU_LED, OUTPUT );
	// Turn the LED on.
	digitalWrite( MCU_LED, HIGH );

	wifiMultiConnect();
	configureOTA();
}


void loop()
{
	digitalWrite( MCU_LED, HIGH );
	Serial.printf( "LED is %d\n", digitalRead( MCU_LED ) );

	// Ramp up from 90 to 180.
	for( azimuthPosition = 90; azimuthPosition < 170; azimuthPosition += 1 )
	{
		azimuthServo.write( azimuthPosition );
		delay( 20 );
	}
	Serial.printf( "Servo at %d\n", azimuthPosition );
	delay( 3000 );
	digitalWrite( MCU_LED, LOW );
	Serial.printf( "LED is %d\n", digitalRead( MCU_LED ) );

	// Ramp down from 180 to 90.
	for( azimuthPosition = 170; azimuthPosition > 90; azimuthPosition -= 1 )
	{
		azimuthServo.write( azimuthPosition );
		delay( 20 );
	}
	Serial.printf( "Servo at %d\n", azimuthPosition );
	delay( 3000 );
	digitalWrite( MCU_LED, HIGH );
	Serial.printf( "LED is %d\n", digitalRead( MCU_LED ) );

	// Ramp down from 90 to 0.
	for( azimuthPosition = 90; azimuthPosition > 10; azimuthPosition -= 1 )
	{
		azimuthServo.write( azimuthPosition );
		delay( 20 );
	}
	Serial.printf( "Servo at %d\n", azimuthPosition );
	delay( 3000 );
	digitalWrite( MCU_LED, LOW );
	Serial.printf( "LED is %d\n", digitalRead( MCU_LED ) );

	// Ramp up from 0 to 90.
	for( azimuthPosition = 10; azimuthPosition < 90; azimuthPosition += 1 )
	{
		azimuthServo.write( azimuthPosition );
		delay( 20 );
	}
	Serial.printf( "Servo at %d\n", azimuthPosition );
	delay( 3000 );

	Serial.println( "End of loop.\n" );
}
