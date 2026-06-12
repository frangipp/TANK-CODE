
#include "main.h"

typedef enum MotorID {
    MOTOR_LEFT,
    MOTOR_RIGHT,
} MotorID;

/** Initialize timers */
void motor_init();

/** Set the speed of a motor
 * @param motor MotorID, left or right
 * @param speed The speed to set (between -100 and 100) */
void motor_set_speed(MotorID motor, int8_t speed);

/** Défini la vitesse des deux moteurs selon des coordonnées
 * @param x The x-coordinate (between -100 and 100)
 * @param y The y-coordinate (between -100 and 100) */
void motor_set_speed_from_coordinates(int x, int y);