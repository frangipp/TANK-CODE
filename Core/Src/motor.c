#include "motor.h"
#include "stm32g4xx_hal_tim.h"
#include <stdlib.h>

extern TIM_HandleTypeDef htim15;
extern TIM_HandleTypeDef htim16;

#define MOTOR_MAX_SPEED 100
#define MOTOR_MAX_XY    100

#define PWM_WRAP_VALUE 8191 // Correspond à une période de 100% pour un timer de 13 bits (0-8191)

void motor_init() {
    // Initialize timers for motor control
    HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1); // left motor
    HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1); // right motor
}

void motor_set_speed(MotorID motor, int8_t speed) {
    // Clamp speed from -100 to 100
    if (speed > MOTOR_MAX_SPEED)
        speed = MOTOR_MAX_SPEED;
    if (speed < -MOTOR_MAX_SPEED)
        speed = -MOTOR_MAX_SPEED;

    TIM_HandleTypeDef *htim = (motor == MOTOR_LEFT) ? &htim15 : &htim16;

    // Determine direction and PWM duty cycle
    uint32_t duty_cycle     = (uint32_t)(abs(speed) * PWM_WRAP_VALUE / 100); // Scale to timer period
    GPIO_PinState dir_state = (speed >= 0) ? GPIO_PIN_SET : GPIO_PIN_RESET;

    htim->Instance->CCR1 = duty_cycle; // Set PWM duty cycle

    if (motor == MOTOR_LEFT) {
        // Set inverse direction for left motor to ensure both motors move forward with positive speed
        dir_state = (dir_state == GPIO_PIN_SET) ? GPIO_PIN_RESET : GPIO_PIN_SET;
        HAL_GPIO_WritePin(PIN_DIR_LEFT_GPIO_Port, PIN_DIR_LEFT_Pin, dir_state);
    } else if (motor == MOTOR_RIGHT) {
        HAL_GPIO_WritePin(PIN_DIR_RIGHT_GPIO_Port, PIN_DIR_RIGHT_Pin, dir_state);
    }
}

void motor_set_speed_from_coordinates(int x, int y) {
    // Clamp x and y from -100 to 100
    if (x > MOTOR_MAX_XY)
        x = MOTOR_MAX_XY;
    if (x < -MOTOR_MAX_XY)
        x = -MOTOR_MAX_XY;
    if (y > MOTOR_MAX_XY)
        y = MOTOR_MAX_XY;
    if (y < -MOTOR_MAX_XY)
        y = -MOTOR_MAX_XY;

    // Simple mixing algorithm for differential drive
    int left_speed  = y + x; // Forward component + turn component
    int right_speed = y - x; // Forward component - turn component

    left_speed  = (left_speed > MOTOR_MAX_SPEED) ? MOTOR_MAX_SPEED : left_speed;
    left_speed  = (left_speed < -MOTOR_MAX_SPEED) ? -MOTOR_MAX_SPEED : left_speed;
    right_speed = (right_speed > MOTOR_MAX_SPEED) ? MOTOR_MAX_SPEED : right_speed;
    right_speed = (right_speed < -MOTOR_MAX_SPEED) ? -MOTOR_MAX_SPEED : right_speed;

    motor_set_speed(MOTOR_LEFT, left_speed);
    motor_set_speed(MOTOR_RIGHT, right_speed);
}