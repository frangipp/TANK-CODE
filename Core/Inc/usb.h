#include "main.h"
#include <stdint.h>

/** Gestion USB */

/** @return le nombre d'octets disponibles dans le buffer USB */
uint32_t usb_data_available();

/** @return le pointeur vers les dernières données reçues via USB */
void usb_get(uint8_t* data, uint32_t len);

/** Envoi de données via USB
 * @param data pointeur vers les données
 * @param len taille du buffer */
void usb_send(uint8_t* data, uint32_t len);

