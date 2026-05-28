#include "core.h"
#include "motor.h"          
#include "stm32g4xx_hal.h"  
#include "usb.h"            
#include "HC-05.h"          

void setup() {
    motorInit();   // Initialisation des moteurs
    HC05_init();   // Initialisation du HC-05 et lancement des IT UART
}

void loop() {
    HC05_process(); // Relaye les données entre l'USB et le HC-05 en continu
}

/**
 * @brief Callback HAL appelé à chaque octet reçu sur n'importe quel UART
 *        Délègue le traitement à HC05 si c'est l'UART1
 */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART1)
    {
        HC05_UART_RxCpltCallback();
    }
}


