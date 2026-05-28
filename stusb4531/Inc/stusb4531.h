/**
 * @file    stusb4531.h
 * @brief   Bibliothèque STUSB4531 – API publique, structures et énumérations
 *
 * Rôle ciblé    : Sink uniquement (USB PD)
 * PDO supportés : Fixed, Variable (architecture extensible pour PPS, Battery…)
 * HAL           : voir platform.h / platform.c
 */

#ifndef STUSB4531_H
#define STUSB4531_H

#ifdef __cplusplus
extern "C" {
#endif

#include "platform.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/* =========================================================================
 * Version de la bibliothèque
 * ========================================================================= */
#define STUSB4531_LIB_VERSION_MAJOR 1
#define STUSB4531_LIB_VERSION_MINOR 0
#define STUSB4531_LIB_VERSION_PATCH 0

/* =========================================================================
 * Constantes générales
 * ========================================================================= */
#define STUSB4531_MAX_PDO_COUNT      3U   /**< Nombre max de PDOs Sink */
#define STUSB4531_NVM_SECTOR_COUNT   4U   /**< Secteurs NVM */
#define STUSB4531_NVM_SECTOR_SIZE    8U   /**< Octets par secteur */
#define STUSB4531_TIMEOUT_DEFAULT_MS 100U /**< Timeout I2C par défaut (ms) */

/* =========================================================================
 * Codes de retour de la bibliothèque
 * ========================================================================= */
typedef enum {
    STUSB4531_OK             = 0,  /**< Succès */
    STUSB4531_ERR_IO         = -1, /**< Erreur de communication I2C */
    STUSB4531_ERR_TIMEOUT    = -2, /**< Timeout dépassé */
    STUSB4531_ERR_PARAM      = -3, /**< Paramètre invalide */
    STUSB4531_ERR_DEVICE     = -4, /**< Device ID inattendu */
    STUSB4531_ERR_NVM        = -5, /**< Erreur séquence NVM */
    STUSB4531_ERR_NVM_VERIFY = -6, /**< Échec de vérification NVM */
    STUSB4531_ERR_PDO        = -7, /**< PDO invalide ou incohérent */
    STUSB4531_ERR_NOT_INIT   = -8, /**< Objet non initialisé */
} stusb4531_status_t;

/* =========================================================================
 * Type de PDO
 * ========================================================================= */
typedef enum {
    PDO_TYPE_FIXED    = 0x00U, /**< Tension fixe */
    PDO_TYPE_BATTERY  = 0x01U, /**< Batterie */
    PDO_TYPE_VARIABLE = 0x02U, /**< Tension variable */
    PDO_TYPE_APDO     = 0x03U, /**< Augmented (PPS, AVS…) */
} pdo_type_t;

/* Sous-type APDO (champ b[29:28] quand type = APDO) */
typedef enum {
    APDO_SUBTYPE_PPS = 0x00U, /**< Programmable Power Supply */
    /* Réserver ici d'autres sous-types futurs */
} apdo_subtype_t;

/* =========================================================================
 * Structures PDO – orientées Sink
 * ========================================================================= */

/** PDO Fixed Sink (USB PD spec §6.4.1) */
typedef struct {
    uint16_t voltage_mv;           /**< Tension en mV (ex. 5000, 9000, 20000) */
    uint16_t current_ma;           /**< Courant opérationnel en mA */
    uint16_t fast_swap_current_ma; /**< Courant Fast Role Swap (0 si non requis) */
    bool dual_role_power;          /**< Dual-Role Power capable */
    bool higher_capability;        /**< Higher Capability bit */
    bool unconstrained_power;      /**< Unconstrained Power source */
    bool usb_communications;       /**< USB communications capable */
    bool dual_role_data;           /**< Dual-Role Data capable */
} pdo_fixed_sink_t;

/** PDO Variable Sink (USB PD spec §6.4.3) */
typedef struct {
    uint16_t max_voltage_mv; /**< Tension maximale en mV */
    uint16_t min_voltage_mv; /**< Tension minimale en mV */
    uint16_t current_ma;     /**< Courant opérationnel en mA */
} pdo_variable_sink_t;

/** PDO Battery Sink (USB PD spec §6.4.2) */
typedef struct {
    uint16_t max_voltage_mv; /**< Tension maximale en mV */
    uint16_t min_voltage_mv; /**< Tension minimale en mV */
    uint16_t power_mw;       /**< Puissance opérationnelle en mW */
} pdo_battery_sink_t;

/** PDO PPS Sink (USB PD spec §6.4.5 – APDO) */
typedef struct {
    uint16_t max_voltage_mv; /**< Tension maximale (granularité 100 mV) */
    uint16_t min_voltage_mv; /**< Tension minimale (granularité 100 mV) */
    uint16_t max_current_ma; /**< Courant maximal (granularité 50 mA) */
    bool pps_power_limited;  /**< Power Limited flag */
} pdo_pps_sink_t;

/**
 * @brief  Objet PDO générique (union typée)
 *
 * Utilisation :
 * @code
 *   stusb4531_pdo_t pdo;
 *   pdo.type = PDO_TYPE_FIXED;
 *   pdo.data.fixed.voltage_mv  = 9000;
 *   pdo.data.fixed.current_ma  = 2000;
 * @endcode
 */
typedef struct {
    pdo_type_t type; /**< Type du PDO */
    union {
        pdo_fixed_sink_t fixed;
        pdo_variable_sink_t variable;
        pdo_battery_sink_t battery;
        pdo_pps_sink_t pps;
        /* Ajouter de nouveaux types ici sans modifier l'API */
    } data;
} stusb4531_pdo_t;

/* =========================================================================
 * État de connexion USB PD
 * ========================================================================= */
typedef enum {
    STUSB4531_CONN_NONE = 0,    /**< Aucune connexion */
    STUSB4531_CONN_DEFAULT,     /**< 5 V / 900 mA (USB Default) */
    STUSB4531_CONN_1_5A,        /**< 5 V / 1,5 A (USB Type-C current) */
    STUSB4531_CONN_3_0A,        /**< 5 V / 3,0 A (USB Type-C current) */
    STUSB4531_CONN_PD_CONTRACT, /**< Contrat PD négocié */
} stusb4531_connection_t;

/* =========================================================================
 * État VBUS
 * ========================================================================= */
typedef struct {
    bool vsafe0v;       /**< VBUS < vSafe0V */
    bool valid;         /**< VBUS dans la plage valide pour le Sink */
    bool over_voltage;  /**< VBUS KO (> seuil haut de détection) */
    bool under_voltage; /**< VBUS KO (< seuil bas de détection) */
} stusb4531_vbus_status_t;

/* =========================================================================
 * État CC (Configuration Channel)
 * ========================================================================= */
typedef struct {
    stusb4531_connection_t cc1_state; /**< État CC1 */
    stusb4531_connection_t cc2_state; /**< État CC2 */
    bool connected;                   /**< Le STUSB4531 présente Rd */
    bool looking;                     /**< En cours de recherche de connexion */
    bool attach_status;               /**< true si attaché */
    bool cc_reverse;                  /**< Câble inversé (CC2 actif) */
} stusb4531_cc_status_t;

/* =========================================================================
 * État global du composant (snapshot de registres)
 * ========================================================================= */
typedef struct {
    /* Connexion */
    stusb4531_connection_t connection; /**< Type de connexion actif */
    stusb4531_vbus_status_t vbus;      /**< État VBUS */
    stusb4531_cc_status_t cc;          /**< État CC */

    /* Contrat PD */
    bool pd_contract_active;    /**< Contrat PD en cours */
    uint8_t selected_pdo_index; /**< Index PDO sélectionné (1-based) */
    uint32_t rdo_raw;           /**< RDO brut reçu de la Source */

    /* Alertes actives */
    uint8_t alert_status; /**< Valeur brute REG_ALERT_STATUS_1 */
    bool hw_fault;        /**< Faute matérielle détectée */
    bool msg_received;    /**< Message PD reçu en attente */

    /* État Policy Engine */
    uint8_t pe_fsm_state; /**< État FSM de la Policy Engine */

    /* Identification */
    uint8_t device_id; /**< ID du composant (attendu : 0x21) */
} stusb4531_state_t;

/* =========================================================================
 * Sauvegarde de configuration NVM
 * ========================================================================= */
typedef struct {
    uint8_t raw[STUSB4531_NVM_SECTOR_COUNT][STUSB4531_NVM_SECTOR_SIZE];
    bool valid; /**< Indique si la structure est remplie */
} stusb4531_nvm_image_t;

/* =========================================================================
 * Callbacks utilisateur
 * ========================================================================= */

/**
 * @brief  Appelé lorsque la connexion change (attach/detach, type de connexion)
 * @param  connection  Nouveau type de connexion
 */
typedef void (*stusb4531_cb_connection_t)(stusb4531_connection_t connection);

/**
 * @brief  Appelé lorsqu'un contrat PD est établi ou révoqué
 * @param  active        true si contrat établi, false si révoqué
 * @param  pdo_index     Index du PDO sélectionné (1-based), 0 si révoqué
 * @param  rdo_raw       RDO brut (0 si révoqué)
 */
typedef void (*stusb4531_cb_pd_contract_t)(bool active, uint8_t pdo_index, uint32_t rdo_raw);

/**
 * @brief  Appelé sur faute matérielle
 */
typedef void (*stusb4531_cb_hw_fault_t)();

/**
 * @brief  Appelé à chaque message PD reçu (pour traitement applicatif avancé)
 * @param  header    En-tête du message (2 octets)
 * @param  data      Données du message (peut être NULL si pas de data objects)
 * @param  data_len  Longueur des données en octets
 */
typedef void (*stusb4531_cb_msg_received_t)(uint16_t header, const uint8_t *data, size_t data_len);

/** Structure regroupant tous les callbacks */
typedef struct {
    stusb4531_cb_connection_t on_connection_change; /**< Peut être NULL */
    stusb4531_cb_pd_contract_t on_pd_contract;      /**< Peut être NULL */
    stusb4531_cb_hw_fault_t on_hw_fault;            /**< Peut être NULL */
    stusb4531_cb_msg_received_t on_msg_received;    /**< Peut être NULL */
} stusb4531_callbacks_t;

/* =========================================================================
 * Handle principal (objet de la bibliothèque)
 *
 * Allouer statiquement par l'utilisateur, initialiser via stusb4531_init().
 * Ne pas manipuler les champs internes directement.
 * ========================================================================= */
typedef struct {
    uint8_t i2c_addr;                /**< Adresse I2C 7 bits */
    stusb4531_callbacks_t callbacks; /**< Callbacks utilisateur */
    stusb4531_state_t state;         /**< État courant (mis à jour par poll/IRQ) */
    bool initialized;                /**< true après stusb4531_init() réussie */
    /* Champs internes – ne pas accéder directement */
    uint8_t _alert_mask; /**< Masque d'alertes actif */
    stusb4531_connection_t _prev_connection;
} stusb4531_handle_t;

/* =========================================================================
 * API – Initialisation
 * ========================================================================= */

/**
 * @brief  Initialise le handle et vérifie la présence du composant.
 *         Configure les masques d'alertes par défaut.
 * @param  h         Pointeur sur le handle (alloué par l'utilisateur)
 * @param  i2c_addr  Adresse I2C 7 bits (ex. STUSB4531_I2C_ADDR_DEFAULT)
 * @return STUSB4531_OK ou code d'erreur
 */
stusb4531_status_t stusb4531_init(stusb4531_handle_t *h, uint8_t i2c_addr);

/**
 * @brief  Reset logiciel du composant (via registre RESET_CTRL).
 * @param  h  Pointeur sur le handle
 * @return STUSB4531_OK ou code d'erreur
 */
stusb4531_status_t stusb4531_soft_reset(stusb4531_handle_t *h);

/**
 * @brief  Reset matériel du composant via la broche RESET (GPIO).
 *         Requiert que platform_gpio_reset() soit implémenté.
 * @param  h  Pointeur sur le handle
 */
void stusb4531_hard_reset(stusb4531_handle_t *h);

/**
 * @brief  Enregistre les callbacks utilisateur.
 * @param  h   Pointeur sur le handle
 * @param  cb  Structure de callbacks (les pointeurs NULL sont ignorés)
 */
void stusb4531_register_callbacks(stusb4531_handle_t *h, const stusb4531_callbacks_t *cb);

/* =========================================================================
 * API – Gestion des alertes / interruptions
 * ========================================================================= */

/**
 * @brief  Handler principal des alertes. À appeler :
 *           - depuis l'ISR de la broche ALERT (ou en polling)
 *           - depuis la tâche de supervision si RTOS
 *
 *         Lit les registres de statut, met à jour h->state,
 *         et déclenche les callbacks enregistrés.
 * @param  h  Pointeur sur le handle
 * @return STUSB4531_OK ou code d'erreur
 */
stusb4531_status_t stusb4531_irq_handler(stusb4531_handle_t *h);

/**
 * @brief  Configure le masque des alertes I2C (registre ALERT_STATUS_1_MASK).
 * @param  h     Pointeur sur le handle
 * @param  mask  Masque (bit à 1 = alerte masquée)
 * @return STUSB4531_OK ou code d'erreur
 */
stusb4531_status_t stusb4531_set_alert_mask(stusb4531_handle_t *h, uint8_t mask);

/* =========================================================================
 * API – Lecture d'état (debug)
 * ========================================================================= */

/**
 * @brief  Lit et met à jour l'intégralité de l'état dans h->state.
 * @param  h  Pointeur sur le handle
 * @return STUSB4531_OK ou code d'erreur
 */
stusb4531_status_t stusb4531_read_state(stusb4531_handle_t *h);

/**
 * @brief  Retourne un pointeur sur l'état courant (lecture seule).
 *         Ne relit pas les registres ; appeler stusb4531_read_state() avant.
 * @param  h  Pointeur sur le handle
 * @return Pointeur sur stusb4531_state_t (lecture seule)
 */
const stusb4531_state_t *stusb4531_get_state(const stusb4531_handle_t *h);

/**
 * @brief  Affiche un dump formaté de l'état via platform_log().
 *         Utile pour le debug UART/RTT.
 * @param  h  Pointeur sur le handle
 */
void stusb4531_debug_dump(const stusb4531_handle_t *h);

/**
 * @brief  Lit un registre brut (accès direct, pour debug avancé).
 * @param  h      Pointeur sur le handle
 * @param  reg    Adresse du registre
 * @param  value  [out] Valeur lue
 * @return STUSB4531_OK ou code d'erreur
 */
stusb4531_status_t stusb4531_read_reg(stusb4531_handle_t *h, uint8_t reg, uint8_t *value);

/**
 * @brief  Écrit un registre brut (accès direct, pour debug avancé).
 * @param  h      Pointeur sur le handle
 * @param  reg    Adresse du registre
 * @param  value  Valeur à écrire
 * @return STUSB4531_OK ou code d'erreur
 */
stusb4531_status_t stusb4531_write_reg(stusb4531_handle_t *h, uint8_t reg, uint8_t value);

/* =========================================================================
 * API – Gestion des PDO
 * ========================================================================= */

/**
 * @brief  Encode un PDO générique en mot 32 bits USB PD.
 * @param  pdo     PDO source (structure)
 * @param  raw     [out] Mot 32 bits encodé
 * @return STUSB4531_OK ou STUSB4531_ERR_PARAM si PDO invalide
 */
stusb4531_status_t stusb4531_pdo_encode(const stusb4531_pdo_t *pdo, uint32_t *raw);

/**
 * @brief  Décode un mot 32 bits USB PD en structure PDO.
 * @param  raw     Mot 32 bits brut
 * @param  pdo     [out] Structure PDO décodée
 * @return STUSB4531_OK ou STUSB4531_ERR_PARAM
 */
stusb4531_status_t stusb4531_pdo_decode(uint32_t raw, stusb4531_pdo_t *pdo);

/**
 * @brief  Valide la cohérence d'un PDO (plages de tension/courant, etc.).
 * @param  pdo  PDO à valider
 * @return STUSB4531_OK si valide, STUSB4531_ERR_PDO sinon
 */
stusb4531_status_t stusb4531_pdo_validate(const stusb4531_pdo_t *pdo);

/**
 * @brief  Écrit la liste de PDOs Sink dans les registres DPM du composant.
 *         Les PDOs sont validés avant l'écriture.
 * @param  h          Pointeur sur le handle
 * @param  pdos       Tableau de PDOs
 * @param  pdo_count  Nombre de PDOs (1 à STUSB4531_MAX_PDO_COUNT)
 * @return STUSB4531_OK ou code d'erreur
 */
stusb4531_status_t stusb4531_set_sink_pdos(stusb4531_handle_t *h, const stusb4531_pdo_t *pdos, uint8_t pdo_count);

/**
 * @brief  Lit les PDOs Sink actuellement chargés dans les registres DPM.
 * @param  h          Pointeur sur le handle
 * @param  pdos       [out] Tableau de PDOs
 * @param  pdo_count  [in/out] Capacité du tableau en entrée, nombre lus en sortie
 * @return STUSB4531_OK ou code d'erreur
 */
stusb4531_status_t stusb4531_get_sink_pdos(stusb4531_handle_t *h, stusb4531_pdo_t *pdos, uint8_t *pdo_count);

/* =========================================================================
 * API – NVM
 * ========================================================================= */

/**
 * @brief  Lit l'intégralité de la NVM dans une image mémoire.
 * @param  h      Pointeur sur le handle
 * @param  image  [out] Image NVM
 * @return STUSB4531_OK ou code d'erreur
 */
stusb4531_status_t stusb4531_nvm_read(stusb4531_handle_t *h, stusb4531_nvm_image_t *image);

/**
 * @brief  Programme l'intégralité de la NVM depuis une image mémoire.
 *         Effectue : unlock → erase → write → verify → lock.
 * @param  h      Pointeur sur le handle
 * @param  image  Image NVM à programmer
 * @return STUSB4531_OK ou code d'erreur (STUSB4531_ERR_NVM_VERIFY si vérif échoue)
 */
stusb4531_status_t stusb4531_nvm_write(stusb4531_handle_t *h, const stusb4531_nvm_image_t *image);

stusb4531_status_t reg_write_buf(stusb4531_handle_t *h, uint8_t reg, const uint8_t *buf, size_t len);
stusb4531_status_t reg_read_buf(stusb4531_handle_t *h, uint8_t reg, uint8_t *buf, size_t len);

/**
 * @brief  Affiche le contenu brut d'une image NVM via platform_log() (debug).
 * @param  image  Image NVM à afficher
 */
void stusb4531_nvm_dump(const stusb4531_nvm_image_t *image);

stusb4531_status_t nvm_wait_ready(stusb4531_handle_t *h);

#ifdef __cplusplus
}
#endif

#endif /* STUSB4531_H */
