/*
 * Servo specification:
 * Color: green
 * Torsion: 0.7kg
 * Operating voltage: 4.8-6V
 * Use Angle: 360 degrees
 * PWM signal: 500-2500
 * Wire length: 25CM
 * Terminal type: DuPont terminal
 * Gear reduction ratio: 1:140
 * Product speed: 160/ min (6V)
 */

#include <ESP32Servo.h>

Servo azimuthServo;  // create servo object to control a servo
// 16 servo objects can be created on the ESP32

int pos = 90;    // variable to store the servo position
// Recommended PWM GPIO pins on the ESP32 include 2,4,12-19,21-23,25-27,32-33
// Possible PWM GPIO pins on the ESP32-S2: 0(used by on-board button),1-17,18(used by on-board LED),19-21,26,33-42
#if defined(ARDUINO_ESP32S2_DEV)
int servoPin = 23;
#else
int servoPin = 23;
#endif

void setup()
{
	Serial.begin( 115200 );
	// Allow allocation of all timers
	ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
	ESP32PWM::allocateTimer(3);
	azimuthServo.setPeriodHertz(50);    // standard 50 hz servo
	azimuthServo.attach(servoPin, 500, 2500); // attaches the servo on pin 23 to the servo object
	azimuthServo.write( pos );
	// using default min/max of 1000us and 2000us
	// different servos may require different min/max settings
	// for an accurate 0 to 180 sweep
	pinMode( 2, OUTPUT );
}

void loop()
{
	digitalWrite( 2, HIGH );
	Serial.printf( "LED is %d\n", digitalRead( 2 ) );

	// Ramp up from 90 to 180.
	for( pos = 90; pos < 170; pos += 1 )
	{
		azimuthServo.write( pos );
		delay( 20 );
	}
	Serial.printf( "Servo at %d\n", pos );
	delay( 3000 );
	digitalWrite( 2, LOW );
	Serial.printf( "LED is %d\n", digitalRead( 2 ) );

	// Ramp down from 180 to 90.
	for( pos = 170; pos > 90; pos -= 1 )
	{
		azimuthServo.write( pos );
		delay( 20 );
	}
	Serial.printf( "Servo at %d\n", pos );
	delay( 3000 );
	digitalWrite( 2, HIGH );
	Serial.printf( "LED is %d\n", digitalRead( 2 ) );

	// Ramp down from 90 to 0.
	for( pos = 90; pos > 10; pos -= 1 )
	{
		azimuthServo.write( pos );
		delay( 20 );
	}
	Serial.printf( "Servo at %d\n", pos );
	delay( 3000 );
	digitalWrite( 2, LOW );
	Serial.printf( "LED is %d\n", digitalRead( 2 ) );

	// Ramp up from 0 to 90.
	for( pos = 10; pos < 90; pos += 1 )
	{
		azimuthServo.write( pos );
		delay( 20 );
	}
	Serial.printf( "Servo at %d\n", pos );
	delay( 3000 );

	Serial.println( "End of loop.\n" );
}
