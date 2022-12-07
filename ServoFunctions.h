//
// Created by Adam on 2022-11-15.
//

#ifndef LEGOESP32SUNTRACKER_SERVOFUNCTIONS_H
#define LEGOESP32SUNTRACKER_SERVOFUNCTIONS_H

#include <ESP32Servo.h>		// The servo library to use.

// ServoFunctions.ino
void setAltitude( int angle );
void setAzimuthSpeed( int speed );
void altitudeDemo();
void azimuthDemo();
void moveArm();

Servo altitudeServo;						// A Servo class object for controlling the altitude position.
Servo azimuthServo;						// A Servo class object for controlling the azimuth direction and speed.

/**
 * Global servo variables.
 * Depending on your servo make, the pulse width minimum and maximum may vary.
 * Adjust these to be as close to the mechanical limits of each servo, without hitting those actual limits.
 * For continuous rotation servos, use the declared minimum and maximum values from the specification sheet.
 * If unknown, 500 μsec is the most common minimum, and 2500 μsec is the most common maximum.
 */
int altitudeServoPin = 22;		// The GPIO which the altitude servo connects to.
int altitudePosition = 0;		// Holds the current position of the azimuth servo.  The default of 0 is straight forward.
int altitudeMaxAngle = 360;	// The "do not exceed" angle for the altitude servo.  This is used as a constraint to keep the tracker from trying to pitch beyond vertical.
int azimuthServoPin = 23;		// The GPIO which the azimuth servo connects to.
int azimuthSpeed = 0;			// Holds the current position of the azimuth servo.  The default of 1500 is motionless.
int minPulseWidth = 500;		// The minimum pulse width to use with servos.
int maxPulseWidth = 2500;		// The maximum pulse width to use with servos.


#endif //LEGOESP32SUNTRACKER_SERVOFUNCTIONS_H
