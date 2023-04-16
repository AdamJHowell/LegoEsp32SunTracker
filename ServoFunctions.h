//
// Created by Adam on 2022-11-15.
//

#ifndef LEGOESP32SUNTRACKER_SERVOFUNCTIONS_H
#define LEGOESP32SUNTRACKER_SERVOFUNCTIONS_H

#include <ESP32Servo.h> // The servo library to use.

// ServoFunctions.ino
void setAltitude( int angle );
void setAltitudeSpeed( int speed );
void setAzimuthSpeed( int speed );
void moveArm();

Servo altitudeServo; // A Servo class object for controlling the altitude position.
Servo azimuthServo;	// A Servo class object for controlling the azimuth direction and speed.

/**
 * Global servo variables.
 * Depending on your servo make, the pulse width minimum and maximum may vary.
 * Adjust these to be as close to the mechanical limits of each servo, without hitting those actual limits.
 * For continuous rotation servos, use the declared minimum and maximum values from the specification sheet.
 * If unknown, 500 μsec is the most common minimum, and 2500 μsec is the most common maximum.
 */
const int declinationStopPin = 17; // The GPIO connected to the declination limit switch.
const int inclinationStopPin = 16; // The GPIO connected to the inclination limit switch.
const int azimuthServoPin = 23;	  // The GPIO which the azimuth servo connects to.
const int altitudeServoPin = 22;	  // The GPIO which the altitude servo connects to.
const int altitudeMaxAngle = 360;  // The "do not exceed" angle for the altitude servo.  This is used as a constraint to keep the tracker from trying to pitch beyond vertical.
const int minPulseWidth = 500;	  // The minimum pulse width to use with servos.
const int maxPulseWidth = 2500;	  // The maximum pulse width to use with servos.
int altitudeSpeed = 0;				  // Holds the current speed of the altitude servo.  The default of 1500 is motionless.
int azimuthSpeed = 0;				  // Holds the current speed of the azimuth servo.  The default of 1500 is motionless.


#endif //LEGOESP32SUNTRACKER_SERVOFUNCTIONS_H
