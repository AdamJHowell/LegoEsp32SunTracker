#include "LegoEsp32SunTracker.h"


/**
 * @brief The altitude servo is a full-rotation (360°) servo that needs to be constrained to 90°.
 * It is not a continuous-rotation servo.
 * The allowed range will be 0°-90°.
 * @param angle the angle to set the servo to.
 */
void setAltitude( int angle )
{
	// Enforce constraints.
	if( angle < 0 )
		angle = 0;
	if( angle > 90 )
		angle = 90;
	// Establish an adjusted maximum pulse width, to enforce a 0°-90° range.
	int pwmRange = maxPulseWidth - minPulseWidth;
	// The constrained maximum range should be the minimum value plus 1/4 of the range.
	int modifiedMaxPulseWidth = minPulseWidth + ( pwmRange / 4 );
	// Map the angle to a pulse width.  Note that with 90 coming before 0, the servo is effectively reversed.
	int pulseWidth = map( angle, 90, 0, minPulseWidth, modifiedMaxPulseWidth );
	// Set the servo to the resulting value.
	altitudeServo.writeMicroseconds( pulseWidth );
	Serial.printf( "Altitude position: %d\n", altitudePosition );
}


/**
 * @brief The azimuth servo is a continuous-rotation servo.
 * @param speed the speed to set the servo to.
 */
void setAzimuthSpeed( int speed )
{
	// Enforce constraints.
	if( speed > 100 )
		speed = 100;
	if( speed < -100 )
		speed = -100;
	// Map the speed to a pulse width.
	int pulseWidth = map( speed, -100, 100, minPulseWidth, maxPulseWidth );
	// Set the servo to the resulting value.
	azimuthServo.writeMicroseconds( pulseWidth );
	Serial.printf( "Azimuth speed: %d, adjusted speed: %d\n", speed, pulseWidth );
}
