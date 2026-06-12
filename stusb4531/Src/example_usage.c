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

void setup() {
    motor_init();
    HC05_init(); // Initialisation du HC-05 et lancement des IT UART

    HAL_Delay(500); // Attendre que le terminal USB soit prêt
    printf("Initialisation du STUSB4531...\n");
    if (stusb4531_init(&g_pd, 0x28) != STUSB4531_OK) {
        printf("Erreur d'initialisation du STUSB4531\n");
    } else {
        printf("STUSB4531 initialisé avec succès\n");
    }
}

void loop() {
    HC05_process(); // Relaye les données entre l'USB et le HC-05 en continu

    /** Test d'envoi/réception */
    uint32_t usb_data_length  = usb_data_available();
    uint32_t hc05_data_length = HC05_data_available();
    if (usb_data_length || hc05_data_length) {
        char data[100]; // Adjust the size as needed
        if (usb_data_length) {
            usb_get_str(data, sizeof(data));
            printf("Reçu via USB: %s\n", data);
        } else if (hc05_data_length) {
            HC05_get_line(data, sizeof(data));
            printf("Reçu via HC-05: %s\n", data);
        }

        if (data[0] == '0') {
            printf("Initialisation du STUSB4531...\n");
            if (stusb4531_init(&g_pd, 0x28) != STUSB4531_OK) {
                printf("Erreur d'initialisation du STUSB4531\n");
            } else {
                printf("STUSB4531 initialisé avec succès\n");
            }
        }
        if (data[0] == '1') {
            /* Relire tous les registres d'état */
            stusb4531_read_state(&g_pd);

            /* Dump formaté via platform_log */
            stusb4531_debug_dump(&g_pd);
        }
        if (data[0] == '2') {
            stusb4531_pdo_t pdos[STUSB4531_MAX_PDO_COUNT];
            uint8_t count = 0;

            stusb4531_nvm_image_t *image = malloc(sizeof(stusb4531_nvm_image_t));
            stusb4531_nvm_read(&g_pd, image);
            stusb4531_nvm_dump(image);

            // stusb4531_nvm_restore_pdo_config(&g_pd, image);<
            // stusb4531_get_sink_pdos(&g_pd, pdos, &count);

            const char *pdo_type_str[] = {"Fixed", "Variable", "Battery", "APDO"};

            printf("PDOs Sink configurés (%d):\n", count);
            for (uint8_t i = 0; i < count; i++) {
                printf("  PDO #%d: type=%s\n", i + 1, pdo_type_str[pdos[i].type]);
            }
        }
        /** Définition des PDO */
        if (data[0] == '3') {

            // stusb4531_write_reg(&g_pd, REG_ALERT_STATUS_MASK, 0xff); // Tout activer

            // Défini le nb de PDO
            uint8_t value = 3;
            stusb4531_write_reg(&g_pd, REG_NUM_PDO, value);

            // PDO2 9V ; PDO3 15V
            value = (1 << 2) | (2 << 4);
            stusb4531_write_reg(&g_pd, REG_SNK_PDO_PARAMS, value);

            // bit [15..8] par pas de 200mV
            // bit [7..6] par pas de 50mV (non utilisée)
            uint16_t value16 = 0;                                                   //((uint16_t)(12 / 0.2));
            reg_write_buf(&g_pd, REG_SNK_PDO_CAPABILITIES, (uint8_t *)&value16, 2); // PDO3 12V@1.5A

            // Défini la puissance max à 10W, utile ?
            value = 20 / 0.5;
            stusb4531_write_reg(&g_pd, REG_DEVICE_PDP, value);

            // Défini l'utilisation d'un PDO variable (PDO3)
            value = (1 << 6); // Variable PDO
            stusb4531_write_reg(&g_pd, REG_ALGO, value);

            // Set mismatch_pdn
            value = 0xC0 | (1 << 3); // Mismatch PDO
            stusb4531_write_reg(&g_pd, REG_APPLI_CTRL, value);

            value = 0; //(2 << 4) | (2 << 2) | (2 << 0);
            stusb4531_write_reg(&g_pd, REG_REQUEST_SRC_PDP, value);

            // == APDO ==

            // MAx voltage
            stusb4531_write_reg(&g_pd, REG_SNK_APDO_FILL_1, (15 / 0.1)); // 12V max
            // Min voltage
            stusb4531_write_reg(&g_pd, REG_SNK_APDO_FILL_2, (9 / 0.1)); // 5V min
            // Max current
            stusb4531_write_reg(&g_pd, REG_SNK_APDO_FILL_3, (1 / 0.05)); // 2A max

            // == Ecriture NVM ==
            stusb4531_write_reg(&g_pd, REG_NVM_CUST_CTRL, (1 << 7) | 0x3);
            uint32_t flag = HAL_GetTick();
            nvm_wait_ready(&g_pd);
            printf("NVM write completed in %lu ms\n", (unsigned long)(HAL_GetTick() - flag));

            stusb4531_write_reg(&g_pd, REG_NVM_CUST_CTRL, 0); // lock
            printf("Waiting for lock...\n");
            nvm_wait_ready(&g_pd);
        }
        /** Vérification des PDO */
        if (data[0] == '4') {
            // Mask
            uint8_t value = 0;
            stusb4531_read_reg(&g_pd, REG_ALERT_STATUS_MASK, &value);
            printf("REG_ALERT_STATUS_MASK = 0x%02X\n", value);

            stusb4531_read_reg(&g_pd, REG_NUM_PDO, &value);
            printf("REG_NUM_PDO = 0x%02X\n", value);

            stusb4531_read_reg(&g_pd, REG_SNK_PDO_PARAMS, (uint8_t *)&value);
            printf("REG_SNK_PDO_PARAMS = 0x%02x\n", value);

            uint16_t val16 = 0;
            reg_read_buf(&g_pd, REG_SNK_PDO_CAPABILITIES, (uint8_t *)&val16, 2);
            printf("REG_SNK_PDO_CAPABILITIES = 0x%04X\n", val16);

            // Device PDP
            stusb4531_read_reg(&g_pd, REG_DEVICE_PDP, &value);
            printf("REG_DEVICE_PDP = 0x%02X\n", value);

            // Algo
            stusb4531_read_reg(&g_pd, REG_ALGO, &value);
            printf("REG_ALGO = 0x%02X\n", value);

            // Check mismatch
            stusb4531_read_reg(&g_pd, REG_APPLI_CTRL, &value);
            printf("REG_APPLI_CTRL = 0x%02X\n", value);

            stusb4531_read_reg(&g_pd, REG_REQUEST_SRC_PDP, &value);
            printf("REG_REQUEST_SRC_PDP = 0x%02X\n", value);
        }
        /** Ecrit les registre I2C depuis NVM */
        if (data[0] == '5') {
            // Charge la NVM
            stusb4531_write_reg(&g_pd, REG_NVM_CUST_CTRL, 1 << 7); // Unlock
            printf("Waiting for unlock...\n");
            nvm_wait_ready(&g_pd);

            printf("Restoring PDO config from NVM...\n");
            stusb4531_write_reg(&g_pd, REG_NVM_CUST_CTRL, (1 << 7) | 0x4);
            nvm_wait_ready(&g_pd);

            stusb4531_write_reg(&g_pd, REG_NVM_CUST_CTRL, 0); // lock
            printf("Waiting for lock...\n");
            nvm_wait_ready(&g_pd);
        }
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
