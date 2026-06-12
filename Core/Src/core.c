#include "core.h"
#include "main.h"
#include "motor.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_gpio.h"
#include "stm32g4xx_hal_uart.h"
#include "usbd_cdc_if.h"
#include <stdint.h>
#include <string.h>

#include "stusb4531.h"
#include "stusb4531_regs.h"
#include "usb.h"

#include "HC-05.h"

static stusb4531_handle_t g_pd;

int x = 0;
int y = 0;

void setup() {
    motor_init();
    HC05_init(); // Initialisation du HC-05 et lancement des IT UART
    motor_set_speed(MOTOR_LEFT, 0);
    motor_set_speed(MOTOR_RIGHT, 0);
}

void loop() {
    static uint32_t last_data_time = 0;
    HC05_process(); // Relaye les données entre l'USB et le HC-05 en continu

    /** Test d'envoi/réception */
    uint32_t usb_data_length  = usb_data_available();
    uint32_t hc05_data_length = HC05_data_available();
    if (usb_data_length || hc05_data_length) {
        last_data_time = HAL_GetTick(); // Met à jour le temps de la dernière réception de données

        char data[100]; // Adjust the size as needed
        if (usb_data_length) {
            usb_get_str(data, sizeof(data));
            printf("Reçu via USB: %s\n", data);
        } else if (hc05_data_length) {
            HC05_get_line(data, sizeof(data));
            printf("Reçu via HC-05: %s\n", data);
        }

        // Mise à jour de la vitesse des moteurs
        if (data[0] == 'X' && strlen(data) > 2) {
            x = atoi(data + 1);
            motor_set_speed_from_coordinates(x, y);
            printf("Coordonnée X: %d\n", x);
        } else if (data[0] == 'Y' && strlen(data) > 2) {
            y = atoi(data + 1);
            motor_set_speed_from_coordinates(x, y);
            printf("Coordonnée Y: %d\n", y);
        }
    }

    // Timeout après non réception de données pendant 500ms
    if (HAL_GetTick() - last_data_time > 500) {
        motor_set_speed_from_coordinates(0, 0); // Arrêt des moteurs
        // printf("Aucune donnée reçue depuis 500ms, arrêt des moteurs\n");
    }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
    if (GPIO_Pin == PIN_ALERT_Pin)
        stusb4531_irq_handler(&g_pd);
    printf("EXTI interrupt on pin %d\n", GPIO_Pin);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if (huart->Instance == USART1) {
        HC05_UART_RxCpltCallback();
    }
}
