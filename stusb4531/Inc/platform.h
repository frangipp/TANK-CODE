/**
 * @file    platform.h
 * @brief   Couche d'abstraction matérielle (HAL) pour la bibliothèque STUSB4531
 *
 * L'utilisateur DOIT implémenter toutes les fonctions marquées « À IMPLÉMENTER »
 * dans platform.c (ou tout fichier équivalent dans son projet).
 *
 * Les fonctions optionnelles peuvent rester en stubs vides si la fonctionnalité
 * n'est pas requise.
 */

#ifndef PLATFORM_H
#define PLATFORM_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/* =========================================================================
 * Codes de retour de la plateforme
 * ========================================================================= */
typedef enum {
    PLATFORM_OK          = 0,  /**< Opération réussie */
    PLATFORM_ERR_IO      = -1, /**< Erreur de communication I2C */
    PLATFORM_ERR_TIMEOUT = -2, /**< Timeout dépassé */
    PLATFORM_ERR_PARAM   = -3, /**< Paramètre invalide */
    PLATFORM_ERR_BUSY    = -4, /**< Bus ou périphérique occupé */
} platform_status_t;

/* =========================================================================
 * Niveau de log
 * ========================================================================= */
typedef enum {
    PLATFORM_LOG_ERROR = 0,
    PLATFORM_LOG_WARN,
    PLATFORM_LOG_INFO,
    PLATFORM_LOG_DEBUG,
} platform_log_level_t;

/* =========================================================================
 * I2C – À IMPLÉMENTER
 * ========================================================================= */

/**
 * @brief  Écriture I2C : envoie `len` octets depuis `data` au registre `reg`
 *         de l'esclave à l'adresse 7 bits `dev_addr`.
 * @param  dev_addr  Adresse I2C 7 bits du STUSB4531
 * @param  reg       Adresse du premier registre cible
 * @param  data      Pointeur sur les données à écrire
 * @param  len       Nombre d'octets à écrire
 * @return PLATFORM_OK ou code d'erreur
 */
platform_status_t platform_i2c_write(uint8_t dev_addr, uint8_t reg, const uint8_t *data, size_t len);

/**
 * @brief  Lecture I2C : lit `len` octets depuis le registre `reg`
 *         de l'esclave à l'adresse 7 bits `dev_addr` dans `data`.
 * @param  dev_addr  Adresse I2C 7 bits du STUSB4531
 * @param  reg       Adresse du premier registre à lire
 * @param  data      Buffer de réception
 * @param  len       Nombre d'octets à lire
 * @return PLATFORM_OK ou code d'erreur
 */
platform_status_t platform_i2c_read(uint8_t dev_addr, uint8_t reg, uint8_t *data, size_t len);

/* =========================================================================
 * GPIO – À IMPLÉMENTER
 * ========================================================================= */

/**
 * @brief  Lit l'état logique de la broche ALERT (actif bas).
 * @return 0 si ALERT actif (bas), 1 sinon
 */
int platform_gpio_read_alert(void);

/**
 * @brief  Applique un reset matériel via la broche RESET (actif bas).
 *         Doit maintenir RESET bas pendant au moins 10 µs.
 */
void platform_gpio_reset(void);

/* =========================================================================
 * Temporisation – À IMPLÉMENTER
 * ========================================================================= */

/**
 * @brief  Attente bloquante en millisecondes.
 * @param  ms  Durée en millisecondes
 */
void platform_delay_ms(uint32_t ms);

/**
 * @brief  Retourne un timestamp en millisecondes (pour les timeouts).
 *         Peut rester à 0 si les timeouts ne sont pas utilisés.
 * @return Timestamp courant en ms (type libre, utilisé pour les deltas)
 */
uint32_t platform_get_tick_ms(void);

/* =========================================================================
 * Log – OPTIONNEL (stub vide acceptable)
 * ========================================================================= */

/**
 * @brief  Émet un message de log.
 *         Peut être redirigé vers UART, RTT, semihosting, etc.
 *         Implémenter en stub vide si le log n'est pas nécessaire.
 *
 * @param  level   Niveau de log
 * @param  fmt     Format printf-like
 * @param  ...     Arguments variadiques
 */
void platform_log(platform_log_level_t level, const char *fmt, ...);

/* =========================================================================
 * Section critique – OPTIONNEL (stub vide acceptable si non-RTOS)
 * ========================================================================= */

/**
 * @brief  Entre en section critique (désactive les interruptions ou prend un mutex).
 *         Laisser en stub vide si l'application est mono-tâche.
 */
void platform_enter_critical(void);

/**
 * @brief  Quitte la section critique.
 */
void platform_exit_critical(void);

/* =========================================================================
 * Callbacks gérés par la bibliothèque – À CÂBLER par l'utilisateur
 *
 * Ces fonctions sont DÉFINIES dans stusb4531.c et APPELÉES depuis la
 * couche applicative (ISR ou polling) via stusb4531_irq_handler().
 *
 * L'utilisateur n'a PAS à les implémenter ; il enregistre ses propres
 * callbacks via stusb4531_register_callbacks().
 * ========================================================================= */

#ifdef __cplusplus
}
#endif

#endif /* PLATFORM_H */
