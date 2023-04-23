#include "LegoEsp32SunTracker.h"


/**
 * @brief The altitude servo is a continuous-rotation servo that needs to be constrained to keep the arm from rotating beyond 90°.
 * The allowed range will be from 0° to 'altitudeMaxAngle'°.
 * @param angle the angle to set the servo to.
 * Nothing in this function is blocking.
 */
void setAltitude( int angle )
{
	if( digitalRead( inclinationStopPin ) == 1 && digitalRead( declinationStopPin ) == 1 )
	{
		// Enforce constraints.
		if( angle < 0 )
			angle = 0;
		if( angle > altitudeMaxAngle )
			angle = altitudeMaxAngle;
		// Establish an adjusted maximum pulse width, to enforce a 0°-'altitudeMaxAngle'° range.
		int pwmRange = maxPulseWidth - minPulseWidth;
		// The constrained maximum range should be the minimum value plus 1/4 of the range.
		int modifiedMaxPulseWidth = minPulseWidth + ( pwmRange / 4 );
		// Map the angle to a pulse width.  Note that swapping 0 and altitudeMaxAngle will effectively reverse the servo.
		int pulseWidth = map( angle, 0, altitudeMaxAngle, minPulseWidth, modifiedMaxPulseWidth );
		// Set the servo to the resulting value.
		altitudeServo.writeMicroseconds( pulseWidth );
		//	Serial.printf( "Altitude position: %d, pulse width: %d\n\n", altitudePosition, pulseWidth );
	}
	else
		Serial.println( "Unable to raise/lower arm due to limit switch!" );
} // End of setAltitude() function.


/**
 * @brief The altitude servo is a continuous-rotation servo.
 * @param speed the speed (in percentage of maximum) to set the servo to.
 * Positive altitude values raise the panel.
 * Nothing in this function is blocking.
 */
void setAltitudeSpeed( int speed )
{
	// Enforce constraints.
	if( speed > 100 )
		speed = 100;
	if( speed < -100 )
		speed = -100;
	// Map the speed to a pulse width.
	int pulseWidth = map( speed, -100, 100, minPulseWidth, maxPulseWidth );
	// Set the servo to the resulting value.
	altitudeServo.writeMicroseconds( pulseWidth );
	//	Serial.printf( "Altitude speed: %d, pulse width: %d\n\n", speed, pulseWidth );
} // End of setAltitudeSpeed() function.


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
 * @brief This function uses the analog readings to determine how the servos should be moved.
 * Nothing in this function is blocking.
 */
void moveArm()
{
	int upperSum = upperLeftValue + upperRightValue;
	int lowerSum = lowerLeftValue + lowerRightValue;
	int leftSum = upperLeftValue + lowerLeftValue;
	int rightSum = upperRightValue + lowerRightValue;

	//	int tempAlt = map( upperSum - lowerSum, -8190, 8190, 0, altitudeMaxAngle );
	//	int tempAz = map( leftSum - rightSum, -8190, 8190, -100, 100 );
	//	Serial.printf( "Altitude servo suggestion: %d\n", tempAlt );
	//	Serial.printf( "Azimuth servo suggestion : %d\n", tempAz );


	//	╭────────────╮
	//	│  Altitude  │
	//	╰────────────╯
	if( abs( upperSum - lowerSum ) > 1000 )
	{
		if( upperSum > lowerSum )
			altitudeSpeed = -30;
		else
			altitudeSpeed = 30;
	}
	else if( abs( upperSum - lowerSum ) > 700 )
	{
		if( upperSum > lowerSum )
			altitudeSpeed = -20;
		else
			altitudeSpeed = 20;
	}
	else if( abs( upperSum - lowerSum ) > 100 )
	{
		if( upperSum > lowerSum )
			altitudeSpeed = -10;
		else
			altitudeSpeed = 10;
	}
	else if( abs( upperSum - lowerSum ) > 50 )
	{
		if( upperSum > lowerSum )
			altitudeSpeed = -5;
		else
			altitudeSpeed = 5;
	}
	else
		altitudeSpeed = 0;
	// Don't allow upwards movement if the inclination limit switch is shorted to ground.
	if( altitudeSpeed > 0 && digitalRead( inclinationStopPin ) == 0 )
		altitudeSpeed = 0;
	// Don't allow downward movement if the declination limit switch is shorted to ground.
	if( altitudeSpeed < 0 && digitalRead( declinationStopPin ) == 0 )
		altitudeSpeed = 0;
	setAltitudeSpeed( altitudeSpeed );


	//	╭───────────╮
	//	│  Azimuth  │
	//	╰───────────╯
	if( abs( leftSum - rightSum ) > 1000 )
	{
		if( leftSum > rightSum )
			azimuthSpeed = 100;
		else
			azimuthSpeed = -100;
	}
	else if( abs( leftSum - rightSum ) > 800 )
	{
		if( leftSum > rightSum )
			azimuthSpeed = 50;
		else
			azimuthSpeed = -50;
	}
	else if( abs( leftSum - rightSum ) > 200 )
	{
		if( leftSum > rightSum )
			azimuthSpeed = 30;
		else
			azimuthSpeed = -30;
	}
	else if( abs( leftSum - rightSum ) > 100 )
	{
		if( leftSum > rightSum )
			azimuthSpeed = 15;
		else
			azimuthSpeed = -15;
	}
	else
		azimuthSpeed = 0;
	setAzimuthSpeed( azimuthSpeed );
} // End of moveArm() function.
