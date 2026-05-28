/**
 * @file    example_usage.c
 * @brief   Exemple d'utilisation de la bibliothèque STUSB4531
 *
 * Ce fichier illustre les cas d'usage typiques :
 *   1. Initialisation
 *   2. Configuration des PDOs Sink
 *   3. Gestion des interruptions ALERT
 *   4. Lecture d'état pour debug
 *   5. Lecture / écriture / sauvegarde de la NVM
 *
 * Adapter platform.c avant de compiler.
 */

#include "stusb4531.h"
#include "stusb4531_regs.h"
#include <stdio.h>

/* =========================================================================
 * Handle global (allouer statiquement)
 * ========================================================================= */
static stusb4531_handle_t g_pd;

/* =========================================================================
 * Callbacks utilisateur
 * ========================================================================= */

static void on_connection_change(stusb4531_connection_t conn) {
    const char *s;
    switch (conn) {
    case STUSB4531_CONN_NONE:
        s = "Déconnecté";
        break;
    case STUSB4531_CONN_DEFAULT:
        s = "USB Default (5V/0.9A)";
        break;
    case STUSB4531_CONN_1_5A:
        s = "Type-C 1.5A";
        break;
    case STUSB4531_CONN_3_0A:
        s = "Type-C 3.0A";
        break;
    case STUSB4531_CONN_PD_CONTRACT:
        s = "Contrat PD actif";
        break;
    default:
        s = "Inconnu";
        break;
    }
    printf("[APP] Connexion: %s\n", s);
}

static void on_pd_contract(bool active, uint8_t pdo_index, uint32_t rdo_raw) {
    if (active) {
        printf("[APP] Contrat PD établi: PDO#%d, RDO=0x%08lX\n", pdo_index, (unsigned long)rdo_raw);
    } else {
        printf("[APP] Contrat PD perdu\n");
    }
}

static void on_hw_fault() { printf("[APP] ERREUR MATERIELLE\n"); }

static void on_msg_received(uint16_t header, const uint8_t *data, size_t len) {
    printf("[APP] Message PD reçu: hdr=0x%04X len=%u\n", header, (unsigned)len);
    (void)data;
}

/* =========================================================================
 * 1. Initialisation
 * ========================================================================= */
static int example_init(void) {
    /* Enregistrement des callbacks */
    stusb4531_callbacks_t cbs = {
        .on_connection_change = on_connection_change,
        .on_pd_contract       = on_pd_contract,
        .on_hw_fault          = on_hw_fault,
        .on_msg_received      = on_msg_received,
    };

    stusb4531_status_t st = stusb4531_init(&g_pd, STUSB4531_I2C_ADDR_DEFAULT);
    if (st != STUSB4531_OK) {
        printf("[APP] Init échouée: %d\n", st);
        return -1;
    }

    stusb4531_register_callbacks(&g_pd, &cbs);
    printf("[APP] STUSB4531 initialisé\n");
    return 0;
}

/* =========================================================================
 * 2. Configuration PDO – exemples pour divers profils
 * ========================================================================= */
static void example_set_pdos(void) {
    stusb4531_pdo_t pdos[3];

    /* PDO 1 : Fixed 5V / 3A (obligatoire en Sink) */
    pdos[0].type                            = PDO_TYPE_FIXED;
    pdos[0].data.fixed.voltage_mv           = 5000U;
    pdos[0].data.fixed.current_ma           = 3000U;
    pdos[0].data.fixed.dual_role_power      = false;
    pdos[0].data.fixed.higher_capability    = false;
    pdos[0].data.fixed.unconstrained_power  = false;
    pdos[0].data.fixed.usb_communications   = true;
    pdos[0].data.fixed.dual_role_data       = false;
    pdos[0].data.fixed.fast_swap_current_ma = 0U;

    /* PDO 2 : Fixed 9V / 2A */
    pdos[1].type                            = PDO_TYPE_FIXED;
    pdos[1].data.fixed.voltage_mv           = 9000U;
    pdos[1].data.fixed.current_ma           = 2000U;
    pdos[1].data.fixed.dual_role_power      = false;
    pdos[1].data.fixed.higher_capability    = false;
    pdos[1].data.fixed.unconstrained_power  = false;
    pdos[1].data.fixed.usb_communications   = false;
    pdos[1].data.fixed.dual_role_data       = false;
    pdos[1].data.fixed.fast_swap_current_ma = 0U;

    /* PDO 3 : Variable 3V–11V / 1.5A */
    pdos[2].type                         = PDO_TYPE_VARIABLE;
    pdos[2].data.variable.min_voltage_mv = 3000U;
    pdos[2].data.variable.max_voltage_mv = 11000U;
    pdos[2].data.variable.current_ma     = 1500U;

    stusb4531_status_t st = stusb4531_set_sink_pdos(&g_pd, pdos, 3U);
    if (st != STUSB4531_OK) {
        printf("[APP] set_sink_pdos échoué: %d\n", st);
    } else {
        printf("[APP] PDOs configurés\n");
    }
}

/* =========================================================================
 * 3. Gestion de l'interruption ALERT
 *    À appeler depuis l'ISR de la broche ALERT ou en polling
 * ========================================================================= */
void ALERT_IRQHandler(void) {
    /* Appel depuis l'ISR ou une tâche dédiée */
    stusb4531_irq_handler(&g_pd);
}

/* Variante polling (sans ISR) */
static void example_poll(void) {
    if (platform_gpio_read_alert() == 0) { /* ALERT actif bas */
        stusb4531_irq_handler(&g_pd);
    }
}

/* =========================================================================
 * 4. Lecture d'état pour debug
 * ========================================================================= */
static void example_debug(void) {
    /* Relire tous les registres d'état */
    stusb4531_read_state(&g_pd);

    /* Dump formaté via platform_log */
    stusb4531_debug_dump(&g_pd);

    /* Accès direct à la structure si besoin */
    const stusb4531_state_t *s = stusb4531_get_state(&g_pd);
    if (s) {
        printf("[APP] PE FSM state = 0x%02X\n", s->pe_fsm_state);
        printf("[APP] VBUS ready   = %d\n", s->vbus.valid);
    }

    /* Lecture d'un registre brut */
    uint8_t val = 0U;
    stusb4531_read_reg(&g_pd, REG_CC_STATUS, &val);
    printf("[APP] CC_STATUS brut = 0x%02X\n", val);
}

/* =========================================================================
 * 5. NVM : lecture, modification PDO, écriture, vérification
 * ========================================================================= */
static void example_nvm(void) {
    stusb4531_nvm_image_t image;

    /* --- 5a. Lecture de la NVM courante --- */
    stusb4531_status_t st = stusb4531_nvm_read(&g_pd, &image);
    if (st != STUSB4531_OK) {
        printf("[APP] NVM read échoué: %d\n", st);
        return;
    }
    stusb4531_nvm_dump(&image); /* Affichage debug */

    /* --- 5b. Configurer des PDOs, puis les sauver dans l'image NVM --- */
    example_set_pdos(); /* Configure les PDOs dans les registres DPM */

    // st = stusb4531_nvm_save_pdo_config(&g_pd, &image);
    // if (st != STUSB4531_OK) {
    //     printf("[APP] NVM save PDO échoué: %d\n", st);
    //     return;
    // }

    /* --- 5c. Programmer la NVM (erase + write + verify) --- */
    st = stusb4531_nvm_write(&g_pd, &image);
    if (st != STUSB4531_OK) {
        printf("[APP] NVM write échoué: %d\n", st);
        return;
    }
    printf("[APP] NVM programmée avec succès\n");

    /* --- 5d. Restaurer la config PDO depuis une image NVM existante --- */
    // st = stusb4531_nvm_restore_pdo_config(&g_pd, &image);
    // if (st != STUSB4531_OK) {
    //     printf("[APP] NVM restore échoué: %d\n", st);
    // } else {
    //     printf("[APP] Config PDO restaurée depuis la NVM\n");
    // }
}

/* =========================================================================
 * main – séquence d'exemple complète
 * ========================================================================= */
int main(void) {
    /* 1. Init */
    if (example_init() != 0)
        return 1;

    /* 2. Config PDOs */
    example_set_pdos();

    /* 3. Boucle principale avec polling ALERT */
    for (int i = 0; i < 10; i++) {
        example_poll();
        platform_delay_ms(100U);
    }

    /* 4. Debug dump */
    example_debug();

    /* 5. NVM (décommenter si NVM nécessaire) */
    /* example_nvm(); */

    return 0;
}
