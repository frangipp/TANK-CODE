#include "motor.h"
#include "stm32g4xx_hal_tim.h"
#include <stdlib.h>

extern TIM_HandleTypeDef htim15;
extern TIM_HandleTypeDef htim16;

void motorInit() {
    // Initialize timers for motor control
    HAL_TIM_PWM_Start(&htim15, TIM_CHANNEL_1); // left motor
    HAL_TIM_PWM_Start(&htim16, TIM_CHANNEL_1); // right motor
}

void motorSetSpeed(MotorID motor, int8_t speed) {
    // Clamp speed to -100 to 100
    if (speed > 100) speed = 100;
    if (speed < -100) speed = -100;

    TIM_HandleTypeDef *htim = (motor == MOTOR_LEFT) ? &htim15 : &htim16;

    // Determine direction and PWM duty cycle
    uint32_t dutyCycle = (uint32_t)(abs(speed) * 65535 / 100); // Scale to timer period
    GPIO_PinState dirState = (speed >= 0) ? GPIO_PIN_SET : GPIO_PIN_RESET;

    htim->Instance->CCR1 = dutyCycle; // Set PWM duty cycle

    if (motor == MOTOR_LEFT) {
        HAL_GPIO_WritePin(PIN_DIR_LEFT_GPIO_Port, PIN_DIR_LEFT_Pin, dirState);
    } else if (motor == MOTOR_RIGHT) {
        HAL_GPIO_WritePin(PIN_DIR_RIGHT_GPIO_Port, PIN_DIR_RIGHT_Pin, dirState);
    }
}