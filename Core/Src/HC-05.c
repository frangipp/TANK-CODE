#include "HC-05.h"
#include "main.h"
#include "stm32g4xx_hal.h"
#include "usb.h"
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// Buffers privés (non accessibles depuis l'extérieur)
static uint8_t hc05_rx_byte;
static uint8_t hc05_rx_buffer[BUFFER_SIZE];
static volatile uint16_t hc05_rx_index  = 0;
static volatile uint8_t hc05_line_ready = 0;

/**
 * @brief Initialise le HC-05 en mode données et lance la réception par IT
 */
void HC05_init(void) {
    HAL_GPIO_WritePin(PIN_HC05_EN_GPIO_Port, PIN_HC05_EN_Pin, GPIO_PIN_SET);
    HAL_UART_Receive_IT(&huart1, &hc05_rx_byte, 1); // Attente du 1er octet
}

/**
 * @brief Callback appelé automatiquement à chaque octet reçu depuis le HC-05
 *        Reconstruit la ligne caractère par caractère jusqu'au \n ou \r
 */
void HC05_UART_RxCpltCallback(void) {
    uint8_t c = hc05_rx_byte;

    if (c == '\n' || c == '\r') // Fin de ligne détectée
    {
        if (hc05_rx_index > 0) // Ignore les lignes vides
        {
            hc05_rx_buffer[hc05_rx_index] = '\0';
            hc05_line_ready               = 1;
            hc05_rx_index                 = 0;
        }
    } else {
        if (hc05_rx_index < BUFFER_SIZE - 1) // Protection débordement
            hc05_rx_buffer[hc05_rx_index++] = c;
    }

    HAL_UART_Receive_IT(&huart1, &hc05_rx_byte, 1); // Relance la réception
}

/**
 * @brief Relaie les données dans les deux sens :
 *        PC (USB) -> HC-05 -> Téléphone
 *        Téléphone -> HC-05 -> PC (USB)
 */
void HC05_process(void) {
    // PC (USB) ->Téléphone
    uint32_t usb_len = usb_data_available();
    if (usb_len) {
        uint8_t buffer[BUFFER_SIZE];
        usb_get_str((char *)buffer, sizeof(buffer));

        int len = strlen((char *)buffer);
        if (len > 0) {
            buffer[len++] = '\r'; // Ajout du \r\n requis par le HC-05
            buffer[len++] = '\n';
            HAL_UART_Transmit(&huart1, buffer, len, 500);
        }
    }

    // Téléphone -> PC (USB)
    // if (hc05_line_ready)
    // {
    //     printf("Tel: %s\n", (char*)hc05_rx_buffer); // Affiche sur terminal USB
    //     hc05_line_ready = 0;                        // Réinitialise le flag
    // }
}

uint32_t HC05_data_available() { return hc05_line_ready ? 1 : 0; }

void HC05_get_line(char *buffer, uint16_t max_len) {
    if (hc05_line_ready) {
        strncpy(buffer, (char *)hc05_rx_buffer, max_len - 1);
        buffer[max_len - 1] = '\0'; // Assure la terminaison nulle
        hc05_line_ready     = 0;    // Réinitialise le flag
    } else {
        buffer[0] = '\0'; // Aucune ligne prête, retourne une chaîne vide
    }
}

/**
 * @brief Passe le HC-05 en mode AT pour la configuration
 *        (changer nom, PIN...)
 *        La LED clignote toutes les 2 secondes en mode AT
 */
void HC05_enter_AT_mode(void) {
    HAL_UART_AbortReceive(&huart1);                                          // Stoppe les IT
    HAL_GPIO_WritePin(PIN_HC05_EN_GPIO_Port, PIN_HC05_EN_Pin, GPIO_PIN_SET); // ENABLE = HIGH
    HAL_Delay(100);
    huart1.Init.BaudRate = 38400; // Baudrate du mode AT
    HAL_UART_Init(&huart1);
    HAL_UART_Receive_IT(&huart1, &hc05_rx_byte, 1); // Relance la réception
}

/**
 * @brief Repasse le HC-05 en mode données (mode normal Bluetooth)
 *        La LED clignote rapidement quand non connecté
 */
void HC05_exit_AT_mode(void) {
    HAL_UART_AbortReceive(&huart1);                                            // Stoppe les IT
    HAL_GPIO_WritePin(PIN_HC05_EN_GPIO_Port, PIN_HC05_EN_Pin, GPIO_PIN_RESET); // ENABLE = LOW
    HAL_Delay(100);
    huart1.Init.BaudRate = 115200; // Baudrate du mode données
    HAL_UART_Init(&huart1);
    HAL_UART_Receive_IT(&huart1, &hc05_rx_byte, 1); // Relance la réception
}