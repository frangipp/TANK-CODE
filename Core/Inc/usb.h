#include "main.h"
#include <stdint.h>

/** Gestion USB */

/** @return le nombre d'octets disponibles dans le buffer USB */
uint32_t usb_data_available();

/** Copie les données reçues via USB dans un buffer
 * @param data pointeur vers le buffer à remplir
 * @param len taille du buffer */
void usb_get(uint8_t* data, uint32_t len);

/** Copie les données reçues via USB dans une chaîne de caractères
 * @param str pointeur vers la chaîne de caractères à remplir
 * @param max_len taille maximale de la chaîne (y compris le caractère de fin chaine) */
void usb_get_str(char* str, uint32_t max_len);

/** Envoi de données via USB
 * @param data pointeur vers les données
 * @param len taille du buffer */
void usb_send(uint8_t* data, uint32_t len);

