#include "LegoEsp32SunTracker.h"


/**
 * @brief The altitude servo is a full-rotation (360°) servo that needs to be constrained to 90°.
 * It is not a continuous-rotation servo.
 * The allowed range will be 0°-90°.
 * @param angle the angle to set the servo to.
 * Nothing in this function is blocking.
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
	//	Serial.printf( "Altitude position: %d, pulse width: %d\n\n", altitudePosition, pulseWidth );
} // End of setAltitude() function.


/**
 * @brief The azimuth servo is a continuous-rotation servo.
 * @param speed the speed to set the servo to.
 * Positive azimuth values rotate the device clockwise (to the right).
 * Nothing in this function is blocking.
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
	//	Serial.printf( "Azimuth speed: %d, pulse width: %d\n\n", speed, pulseWidth );
} // End of setAzimuthSpeed() function.


/**
 * @brief A simple function that demonstrates movement on the altitude axis.
 * Nothing in this function is blocking.
 */
void altitudeDemo()
{
	if( altitudePosition < 90 )
		altitudePosition += 45;
	else
		altitudePosition = 0;
	setAltitude( altitudePosition );
} // End of altitudeDemo() function.


/**
 * @brief A simple function that demonstrates movement on the azimuth axis.
 * Nothing in this function is blocking.
 */
void azimuthDemo()
{
	if( azimuthSpeed >= 15 )
		azimuthSpeed = -15;
	else
		azimuthSpeed += 15;
	setAzimuthSpeed( azimuthSpeed );
} // End of azimuthDemo() function.


/**
 * @brief This function uses the analog readings to determine how the servos should be moved.
 * Nothing in this function is blocking.
 */
void moveArm()
{
	int upperSum = upperLeftValue + upperRightValue;
	int lowerSum = lowerLeftValue + lowerRightValue;
	int leftSum = upperLeftValue + lowerLeftValue;
	int rightSum = upperRightValue + lowerRightValue;

	//	int tempAlt = map( upperSum - lowerSum, -8190, 8190, 0, 90 );
	//	int tempAz = map( leftSum - rightSum, -8190, 8190, -100, 100 );
	//	Serial.printf( "Altitude servo suggestion: %d\n", tempAlt );
	//	Serial.printf( "Azimuth servo suggestion : %d\n", tempAz );


	//	╭───────────────────╮
	//	│  Adjust altitude  │
	//	╰───────────────────╯
	if( abs( upperSum - lowerSum ) > 200 )
	{
		if( upperSum > lowerSum )
			altitudePosition += 5;
		else
			altitudePosition -= 5;
	}
	else if( abs( upperSum - lowerSum ) > 100 )
	{
		if( upperSum > lowerSum )
			altitudePosition += 2;
		else
			altitudePosition -= 2;
	}
	else if( abs( upperSum - lowerSum ) > 50 )
	{
		if( upperSum > lowerSum )
			altitudePosition++;
		else
			altitudePosition--;
	}
	altitudePosition = constrain( altitudePosition, 0, 90 );
	setAltitude( altitudePosition );


	//	╭───────────────╮
	//	│  Set azimuth  │
	//	╰───────────────╯
	if( abs( leftSum - rightSum ) > 1000 )
	{
		if( leftSum > rightSum )
			azimuthSpeed = -100;
		else
			azimuthSpeed = 100;
	}
	else if( abs( leftSum - rightSum ) > 800 )
	{
		if( leftSum > rightSum )
			azimuthSpeed = -50;
		else
			azimuthSpeed = 50;
	}
	else if( abs( leftSum - rightSum ) > 100 )
	{
		if( leftSum > rightSum )
			azimuthSpeed = -30;
		else
			azimuthSpeed = 30;
	}
	else if( abs( leftSum - rightSum ) > 50 )
	{
		if( leftSum > rightSum )
			azimuthSpeed = -15;
		else
			azimuthSpeed = 15;
	}
	else
		azimuthSpeed = 0;
	setAzimuthSpeed( azimuthSpeed );
} // End of moveArm() function.
