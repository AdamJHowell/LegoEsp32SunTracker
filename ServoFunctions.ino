#include "LegoEsp32SunTracker.h"


void moveServo( Servo servoToMove, int pwm )
{
	servoToMove.writeMicroseconds( pwm );
}
