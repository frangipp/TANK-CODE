#ifndef HC_05_H
#define HC_05_H

#include "main.h"           
#include "stm32g4xx_hal.h"  
#include <stdint.h>         

#define BUFFER_SIZE 128     // Taille max d'une ligne reçue/envoyée

extern UART_HandleTypeDef huart1;  // UART1 connecté au HC-05

// Initialisation et boucle principale
void HC05_init(void);
void HC05_process(void);
void HC05_UART_RxCpltCallback(void);

// Mode AT (configuration du module)
void HC05_enter_AT_mode(void);
void HC05_exit_AT_mode(void);

#endif /* HC_05_H */