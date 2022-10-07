#include <Arduino.h>
#include <ESP32Servo.h>

Servo azimuthServo;
int azimuthPosition = 90;
int servoPin = 23;


void setup()
{
	Serial.begin( 115200 );
	// Allow allocation of all timers
	ESP32PWM::allocateTimer( 0 );
	ESP32PWM::allocateTimer( 1 );
	ESP32PWM::allocateTimer( 2 );
	ESP32PWM::allocateTimer( 3 );
	azimuthServo.setPeriodHertz( 50 );    // standard 50 hz servo
	azimuthServo.attach( servoPin, 500, 2500 ); // attaches the servo on pin 23 to the servo object
	azimuthServo.write( azimuthPosition );
	// using default min/max of 1000us and 2000us
	// different servos may require different min/max settings
	// for an accurate 0 to 180 sweep
	pinMode( 2, OUTPUT );
}


void loop()
{
	digitalWrite( 2, HIGH );
	Serial.printf( "LED is %d\n", digitalRead( 2 ));

	// Ramp up from 90 to 180.
	for( azimuthPosition = 90; azimuthPosition < 170; azimuthPosition += 1 )
	{
		azimuthServo.write( azimuthPosition );
		delay( 20 );
	}
	Serial.printf( "Servo at %d\n", azimuthPosition );
	delay( 3000 );
	digitalWrite( 2, LOW );
	Serial.printf( "LED is %d\n", digitalRead( 2 ));

	// Ramp down from 180 to 90.
	for( azimuthPosition = 170; azimuthPosition > 90; azimuthPosition -= 1 )
	{
		azimuthServo.write( azimuthPosition );
		delay( 20 );
	}
	Serial.printf( "Servo at %d\n", azimuthPosition );
	delay( 3000 );
	digitalWrite( 2, HIGH );
	Serial.printf( "LED is %d\n", digitalRead( 2 ));

	// Ramp down from 90 to 0.
	for( azimuthPosition = 90; azimuthPosition > 10; azimuthPosition -= 1 )
	{
		azimuthServo.write( azimuthPosition );
		delay( 20 );
	}
	Serial.printf( "Servo at %d\n", azimuthPosition );
	delay( 3000 );
	digitalWrite( 2, LOW );
	Serial.printf( "LED is %d\n", digitalRead( 2 ));

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
