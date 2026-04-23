
#include "main.h"

typedef enum MotorID {
    MOTOR_LEFT,
    MOTOR_RIGHT,
} MotorID;

/** Initialize timers */
void motorInit();

/** Set the speed of a motor
 * @param motor MotorID, left or right
 * @param speed The speed to set (between -100 and 100) */
void motorSetSpeed(MotorID motor, int8_t speed);

