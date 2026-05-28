/**
 * @file    platform.c
 * @brief   Implémentation de la couche HAL – STUBS À COMPLÉTER
 *
 * Remplacez chaque stub par l'appel correspondant de votre BSP / SDK.
 *
 * Exemples commentés pour :
 *   - STM32 HAL (STM32Cube)
 *   - ESP-IDF (ESP32)
 *   - Arduino / AVR
 *   - MCU générique avec I2C bit-bang
 */

#include "platform.h"
#include "main.h"
#include "stm32g4xx_hal_smbus.h"
#include "stusb4531.h"
#include <stdarg.h>
#include <stdio.h>

/* =========================================================================
 * Configuration utilisateur – adapter selon le MCU
 * ========================================================================= */

/* Pour STM32 : inclure "stm32xxxx_hal.h" et déclarer le handle I2C externe */
/* extern I2C_HandleTypeDef hi2c1; */

/* Pour ESP-IDF : inclure "driver/i2c.h" et définir le port */
/* #define I2C_MASTER_PORT  I2C_NUM_0 */

/* =========================================================================
 * I2C
 * ========================================================================= */

platform_status_t platform_i2c_write(uint8_t dev_addr, uint8_t reg, const uint8_t *data, size_t len) {

    HAL_StatusTypeDef ret = HAL_I2C_Mem_Write(&hi2c1, (uint16_t)(dev_addr << 1), reg, I2C_MEMADD_SIZE_8BIT,
                                              (uint8_t *)data, (uint16_t)len, STUSB4531_TIMEOUT_DEFAULT_MS);

    return (ret == HAL_OK) ? PLATFORM_OK : PLATFORM_ERR_IO;
}

platform_status_t platform_i2c_read(uint8_t dev_addr, uint8_t reg, uint8_t *data, size_t len) {

    HAL_StatusTypeDef ret = HAL_I2C_Mem_Read(&hi2c1, (uint16_t)(dev_addr << 1), reg, I2C_MEMADD_SIZE_8BIT, data,
                                             (uint16_t)len, STUSB4531_TIMEOUT_DEFAULT_MS);

    return (ret == HAL_OK) ? PLATFORM_OK : PLATFORM_ERR_IO;
}

int platform_gpio_read_alert(void) { return (int)HAL_GPIO_ReadPin(PIN_ALERT_GPIO_Port, PIN_ALERT_Pin); }

void platform_gpio_reset(void) {

    // Non utilisé
}

/* =========================================================================
 * Temporisation
 * ========================================================================= */

void platform_delay_ms(uint32_t ms) { HAL_Delay(ms); }

uint32_t platform_get_tick_ms(void) { return HAL_GetTick(); }

/* =========================================================================
 * Log
 * ========================================================================= */

void platform_log(platform_log_level_t level, const char *fmt, ...) {

    static const char *level_str[] = {"ERR", "WRN", "INF", "DBG"};

    if (level > PLATFORM_LOG_DEBUG) {
        level = PLATFORM_LOG_DEBUG;
    }

    printf("[STUSB4531][%s] ", level_str[level]);

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);

    printf("\r\n");
    platform_delay_ms(1);
}

/* =========================================================================
 * Section critique
 * ========================================================================= */

void platform_enter_critical(void) {
    __disable_irq(); // Inutile ?
}

void platform_exit_critical(void) {
    __enable_irq(); // Inutile ?
}
