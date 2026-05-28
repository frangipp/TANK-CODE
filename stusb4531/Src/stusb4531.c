/**
 * @file    stusb4531.c
 * @brief   Bibliothèque STUSB4531 – Implémentation
 *
 * Rôle ciblé    : Sink uniquement (USB PD)
 * PDO supportés : Fixed, Variable (extensible PPS, Battery)
 * HAL           : voir platform.h / platform.c
 */

#include "stusb4531.h"
#include "stusb4531_regs.h"
#include <string.h>

/* =========================================================================
 * Macros internes
 * ========================================================================= */

#define CHECK_INIT(h)                                                                                                  \
    do {                                                                                                               \
        if (!(h) || !(h)->initialized)                                                                                 \
            return STUSB4531_ERR_NOT_INIT;                                                                             \
    } while (0)
#define CHECK_PTR(p)                                                                                                   \
    do {                                                                                                               \
        if (!(p))                                                                                                      \
            return STUSB4531_ERR_PARAM;                                                                                \
    } while (0)

#define LOG_E(fmt, ...) platform_log(PLATFORM_LOG_ERROR, fmt, ##__VA_ARGS__)
#define LOG_W(fmt, ...) platform_log(PLATFORM_LOG_WARN, fmt, ##__VA_ARGS__)
#define LOG_I(fmt, ...) platform_log(PLATFORM_LOG_INFO, fmt, ##__VA_ARGS__)
#define LOG_D(fmt, ...) platform_log(PLATFORM_LOG_DEBUG, fmt, ##__VA_ARGS__)

/* Timeout NVM en ms */
#define NVM_OP_TIMEOUT_MS 500U
/* Délai inter-opérations NVM */
#define NVM_INTER_OP_DELAY 5U

/* =========================================================================
 * Helpers I2C privés
 * ========================================================================= */

static stusb4531_status_t reg_read(stusb4531_handle_t *h, uint8_t reg, uint8_t *val) {
    platform_status_t ret = platform_i2c_read(h->i2c_addr, reg, val, 1U);
    if (ret != PLATFORM_OK) {
        LOG_E("reg_read 0x%02X failed (%d)", reg, ret);
        return STUSB4531_ERR_IO;
    }
    return STUSB4531_OK;
}

static stusb4531_status_t reg_write(stusb4531_handle_t *h, uint8_t reg, uint8_t val) {
    platform_status_t ret = platform_i2c_write(h->i2c_addr, reg, &val, 1U);
    if (ret != PLATFORM_OK) {
        LOG_E("reg_write 0x%02X failed (%d)", reg, ret);
        return STUSB4531_ERR_IO;
    }
    return STUSB4531_OK;
}

stusb4531_status_t reg_read_buf(stusb4531_handle_t *h, uint8_t reg, uint8_t *buf, size_t len) {
    platform_status_t ret = platform_i2c_read(h->i2c_addr, reg, buf, len);
    if (ret != PLATFORM_OK) {
        LOG_E("reg_read_buf 0x%02X len=%u failed (%d)", reg, (unsigned)len, ret);
        return STUSB4531_ERR_IO;
    }
    return STUSB4531_OK;
}

stusb4531_status_t reg_write_buf(stusb4531_handle_t *h, uint8_t reg, const uint8_t *buf, size_t len) {
    platform_status_t ret = platform_i2c_write(h->i2c_addr, reg, buf, len);
    if (ret != PLATFORM_OK) {
        LOG_E("reg_write_buf 0x%02X len=%u failed (%d)", reg, (unsigned)len, ret);
        return STUSB4531_ERR_IO;
    }
    return STUSB4531_OK;
}

/* Lecture d'un mot 32 bits little-endian */
static stusb4531_status_t reg_read_u32(stusb4531_handle_t *h, uint8_t reg, uint32_t *val) {
    uint8_t buf[4];
    stusb4531_status_t st = reg_read_buf(h, reg, buf, 4U);
    if (st != STUSB4531_OK)
        return st;
    *val = (uint32_t)buf[0] | ((uint32_t)buf[1] << 8U) | ((uint32_t)buf[2] << 16U) | ((uint32_t)buf[3] << 24U);
    return STUSB4531_OK;
}

/* Écriture d'un mot 32 bits little-endian */
static stusb4531_status_t reg_write_u32(stusb4531_handle_t *h, uint8_t reg, uint32_t val) {
    uint8_t buf[4];
    buf[0] = (uint8_t)(val & 0xFFU);
    buf[1] = (uint8_t)((val >> 8U) & 0xFFU);
    buf[2] = (uint8_t)((val >> 16U) & 0xFFU);
    buf[3] = (uint8_t)((val >> 24U) & 0xFFU);
    return reg_write_buf(h, reg, buf, 4U);
}

/* =========================================================================
 * Initialisation
 * ========================================================================= */

stusb4531_status_t stusb4531_init(stusb4531_handle_t *h, uint8_t i2c_addr) {
    CHECK_PTR(h);
    memset(h, 0, sizeof(*h));
    h->i2c_addr = i2c_addr;

    /* Vérification Device ID */
    uint8_t dev_id        = 0U;
    stusb4531_status_t st = reg_read(h, REG_DEVICE_HW, &dev_id);
    if (st != STUSB4531_OK)
        return st;

    if (dev_id != STUSB4531_DEVICE_HW_ID_VALUE) {
        LOG_E("Device ID mismatch: got 0x%02X, expected 0x%02X", dev_id, STUSB4531_DEVICE_HW_ID_VALUE);
        return STUSB4531_ERR_DEVICE;
    }
    h->state.device_id = dev_id;

    /* Masque d'alertes par défaut : tout activer (0x00 = rien masqué) */
    st = reg_write(h, REG_ALERT_STATUS_MASK, 0x00U);
    if (st != STUSB4531_OK)
        return st;
    h->_alert_mask = 0x00U;

    h->initialized = true;
    LOG_I("STUSB4531 init OK (addr=0x%02X, devID=0x%02X)", i2c_addr, dev_id);
    return STUSB4531_OK;
}

stusb4531_status_t stusb4531_soft_reset(stusb4531_handle_t *h) {
    CHECK_INIT(h);
    stusb4531_status_t st = reg_write(h, REG_RESET_CTRL, RESET_SW_RESET_EN);
    if (st != STUSB4531_OK)
        return st;
    platform_delay_ms(10U);
    LOG_I("Soft reset done");
    return STUSB4531_OK;
}

void stusb4531_hard_reset(stusb4531_handle_t *h) {
    (void)h;
    platform_gpio_reset();
    platform_delay_ms(25U);
    LOG_I("Hard reset done");
}

void stusb4531_register_callbacks(stusb4531_handle_t *h, const stusb4531_callbacks_t *cb) {
    if (!h || !cb)
        return;
    h->callbacks = *cb;
}

/* =========================================================================
 * Gestion des alertes / interruptions
 * ========================================================================= */

stusb4531_status_t stusb4531_set_alert_mask(stusb4531_handle_t *h, uint8_t mask) {
    CHECK_INIT(h);
    stusb4531_status_t st = reg_write(h, REG_ALERT_STATUS_MASK, mask);
    if (st == STUSB4531_OK) {
        h->_alert_mask = mask;
    }
    return st;
}

stusb4531_status_t stusb4531_irq_handler(stusb4531_handle_t *h) {
    CHECK_INIT(h);
    stusb4531_status_t st;

    /* --- 1. Lire le registre d'alerte --- */
    uint8_t alert = 0U;
    st            = reg_read(h, REG_ALERT_STATUS, &alert);
    if (st != STUSB4531_OK)
        return st;

    if (alert == 0U) {
        return STUSB4531_OK; /* Aucune alerte active */
    }

    h->state.alert_status = alert;
    LOG_D("ALERT_STATUS_1 = 0x%02X", alert);

    /* --- 2. Changement d'état de port --- */
    if (alert & ALERT_PORT_STATUS_AL) {
        uint8_t ps0 = 0U, ps1 = 0U;
        st = reg_read(h, REG_CC_STATUS, &ps0);
        st |= reg_read(h, REG_CC_STATUS, &ps1);
        if (st != STUSB4531_OK)
            return st;

        bool attached          = (ps1 & CC_STATUS_ATTACH) != 0U;
        h->state.cc.cc_reverse = (ps1 & CC_STATUS_CC_REVERSE) != 0U;

        stusb4531_connection_t new_conn = STUSB4531_CONN_NONE;
        if (attached) {
            new_conn = STUSB4531_CONN_DEFAULT; /* Sera affiné via CC_STATUS */
        }

        if (new_conn != h->_prev_connection) {
            LOG_I("Connection changed: %d -> %d", h->_prev_connection, new_conn);
            h->state.connection = new_conn;
            h->_prev_connection = new_conn;
            if (h->callbacks.on_connection_change) {
                h->callbacks.on_connection_change(new_conn);
            }
        }
    }

    /* --- 3. Monitoring VBUS / CC --- */
    if (alert & ALERT_MONITORING_STATUS_AL) {
        uint8_t mon0 = 0U, mon1 = 0U;
        st = reg_read(h, REG_MONITORING_STATUS, &mon0);
        st |= reg_read(h, REG_MONITORING_STATUS, &mon1);
        if (st != STUSB4531_OK)
            return st;

        h->state.vbus.vsafe0v       = (mon1 & MONITORING_VBUS_VSAFE0V) != 0U;
        h->state.vbus.valid         = (mon1 & MONITORING_VBUS_VALID) != 0U;
        h->state.vbus.over_voltage  = (mon1 & MONITORING_VBUS_OVERVOLTAGE) != 0U;
        h->state.vbus.under_voltage = (mon1 & MONITORING_VBUS_UNDERVOLTAGE) != 0U;

        LOG_D("VBUS: vsafe0v=%d valid=%d over_voltage=%d under_voltage=%d", h->state.vbus.vsafe0v, h->state.vbus.valid,
              h->state.vbus.over_voltage, h->state.vbus.under_voltage);
    }

    /* --- 4. État CC --- */
    {
        uint8_t cc = 0U;
        st         = reg_read(h, REG_CC_STATUS, &cc);
        if (st != STUSB4531_OK)
            return st;

        h->state.cc.cc1_state = (cc & CC_STATUS_CC1_STATE_MASK);
        h->state.cc.cc2_state = (cc & CC_STATUS_CC2_STATE_MASK) >> 2U;
        h->state.cc.connected = (cc & CC_STATUS_CONNECT_RESULT) != 0U;
        h->state.cc.looking   = (cc & CC_STATUS_LOOKING4CON) != 0U;

        /* Affiner le type de connexion depuis CC */
        if (h->state.cc.connected && h->state.connection != STUSB4531_CONN_PD_CONTRACT) {
            uint8_t cc_val = h->state.cc.cc_reverse ? h->state.cc.cc2_state : h->state.cc.cc1_state;
            switch (cc_val) {
            case CC_STATUS_SNK_PWR1_5A:
                h->state.connection = STUSB4531_CONN_1_5A;
                break;
            case CC_STATUS_SNK_PWR3_0A:
                h->state.connection = STUSB4531_CONN_3_0A;
                break;
            default:
                h->state.connection = STUSB4531_CONN_DEFAULT;
                break;
            }
        }
    }

    /* --- 5. Message PD reçu --- */
    if (alert & ALERT_PRL_STATUS_AL) {
        uint8_t prl = 0U;
        st          = reg_read(h, ALERT_PRL_STATUS_AL, &prl);
        if (st != STUSB4531_OK)
            return st;

        if (prl & PRL_STATUS_MSG_RECEIVED) {
            h->state.msg_received = true;

            /* Lire l'en-tête et les données */
            uint8_t byte_cnt = 0U;
            st               = reg_read(h, REG_RX_BYTE_CNT, &byte_cnt);
            if (st != STUSB4531_OK)
                return st;

            uint8_t hdr_buf[2] = {0U};
            st                 = reg_read_buf(h, REG_RX_HEADER, hdr_buf, 2U);
            if (st != STUSB4531_OK)
                return st;
            uint16_t header = (uint16_t)hdr_buf[0] | ((uint16_t)hdr_buf[1] << 8U);

            uint8_t msg_type = (uint8_t)(header & 0x1FU);
            LOG_D("PD msg received: hdr=0x%04X type=0x%02X", header, msg_type);

            /* Détecter un contrat PD (PS_RDY = type 0x06 en data message) */
            if (byte_cnt > 2U) {
                uint8_t data_buf[28] = {0U};
                uint8_t data_len     = (uint8_t)(byte_cnt - 2U);
                if (data_len > sizeof(data_buf))
                    data_len = sizeof(data_buf);
                st = reg_read_buf(h, REG_RX_DATA_OBJ_224BITS, data_buf, data_len);
                if (st != STUSB4531_OK)
                    return st;

                if (h->callbacks.on_msg_received) {
                    h->callbacks.on_msg_received(header, data_buf, data_len);
                }
            } else {
                if (h->callbacks.on_msg_received) {
                    h->callbacks.on_msg_received(header, NULL, 0U);
                }
            }
        }
    }

    /* --- 6. Contrat PD (RDO status) --- */
    {
        uint8_t pe = 0U;
        st         = reg_read(h, REG_PE_FSM, &pe);
        if (st != STUSB4531_OK)
            return st;
        h->state.pe_fsm_state = pe;

        /* PE state 0x08 = SNK_Ready => contrat actif */
        bool contract_now = (pe == 0x08U);
        if (contract_now != h->state.pd_contract_active) {
            if (contract_now) {
                uint32_t rdo = 0U;
                st           = reg_read_u32(h, REG_DPM_RDO, &rdo);
                if (st != STUSB4531_OK)
                    return st;
                h->state.rdo_raw            = rdo;
                h->state.selected_pdo_index = (uint8_t)((rdo >> 28U) & 0x07U);
                h->state.connection         = STUSB4531_CONN_PD_CONTRACT;
                LOG_I("PD contract active: PDO#%d RDO=0x%08lX", h->state.selected_pdo_index, (unsigned long)rdo);
                if (h->callbacks.on_pd_contract) {
                    h->callbacks.on_pd_contract(true, h->state.selected_pdo_index, rdo);
                }
            } else {
                LOG_I("PD contract lost");
                h->state.rdo_raw            = 0U;
                h->state.selected_pdo_index = 0U;
                if (h->callbacks.on_pd_contract) {
                    h->callbacks.on_pd_contract(false, 0U, 0U);
                }
            }
            h->state.pd_contract_active = contract_now;
        }
    }

    /* --- 7. Faute matérielle --- */
    if (alert & ALERT_HW_FAULT_STATUS_AL) {
        h->state.hw_fault = true;
        LOG_E("HW Fault");

        if (h->callbacks.on_hw_fault) {
            h->callbacks.on_hw_fault();
        }
    }

    return STUSB4531_OK;
}

/* =========================================================================
 * Lecture d'état (debug)
 * ========================================================================= */

stusb4531_status_t stusb4531_read_state(stusb4531_handle_t *h) {
    CHECK_INIT(h);
    stusb4531_status_t st;

    /* CC status */
    uint8_t cc = 0U;
    st         = reg_read(h, REG_CC_STATUS, &cc);
    if (st != STUSB4531_OK)
        return st;
    h->state.cc.cc1_state     = (cc & CC_STATUS_CC1_STATE_MASK);
    h->state.cc.cc2_state     = (cc & CC_STATUS_CC2_STATE_MASK) >> 2U;
    h->state.cc.connected     = (cc & CC_STATUS_CONNECT_RESULT) != 0U;
    h->state.cc.looking       = (cc & CC_STATUS_LOOKING4CON) != 0U;
    h->state.cc.attach_status = (cc & CC_STATUS_ATTACH) != 0U;
    h->state.cc.cc_reverse    = (cc & CC_STATUS_CC_REVERSE) != 0U;

    /* VBUS monitoring */
    uint8_t mon1 = 0U;
    st           = reg_read(h, REG_MONITORING_STATUS, &mon1);
    if (st != STUSB4531_OK)
        return st;
    h->state.vbus.vsafe0v       = (mon1 & MONITORING_VBUS_VSAFE0V) != 0U;
    h->state.vbus.valid         = (mon1 & MONITORING_VBUS_VALID) != 0U;
    h->state.vbus.over_voltage  = (mon1 & MONITORING_VBUS_OVERVOLTAGE) != 0U;
    h->state.vbus.under_voltage = (mon1 & MONITORING_VBUS_UNDERVOLTAGE) != 0U;

    /* Alerte */
    st = reg_read(h, REG_ALERT_STATUS, &h->state.alert_status);
    if (st != STUSB4531_OK)
        return st;

    /* PE FSM */
    st = reg_read(h, REG_PE_FSM, &h->state.pe_fsm_state);
    if (st != STUSB4531_OK)
        return st;

    /* RDO */
    st = reg_read_u32(h, REG_DPM_RDO, &h->state.rdo_raw);
    if (st != STUSB4531_OK)
        return st;

    /* Connexion type */
    if (h->state.cc.attach_status == 0U) {
        h->state.connection         = STUSB4531_CONN_NONE;
        h->state.pd_contract_active = false;
    } else if (h->state.pe_fsm_state == 0x08U) {
        h->state.connection         = STUSB4531_CONN_PD_CONTRACT;
        h->state.pd_contract_active = true;
        h->state.selected_pdo_index = (uint8_t)((h->state.rdo_raw >> 28U) & 0x07U);
    } else {
        uint8_t cc_val = h->state.cc.cc_reverse ? h->state.cc.cc2_state : h->state.cc.cc1_state;
        switch (cc_val) {
        case CC_STATUS_SNK_PWR1_5A:
            h->state.connection = STUSB4531_CONN_1_5A;
            break;
        case CC_STATUS_SNK_PWR3_0A:
            h->state.connection = STUSB4531_CONN_3_0A;
            break;
        default:
            h->state.connection = STUSB4531_CONN_DEFAULT;
            break;
        }
        h->state.pd_contract_active = false;
    }

    return STUSB4531_OK;
}

const stusb4531_state_t *stusb4531_get_state(const stusb4531_handle_t *h) {
    if (!h || !h->initialized)
        return NULL;
    return &h->state;
}

void stusb4531_debug_dump(const stusb4531_handle_t *h) {
    if (!h || !h->initialized) {
        platform_log(PLATFORM_LOG_ERROR, "stusb4531_debug_dump: not initialized");
        return;
    }
    const stusb4531_state_t *s = &h->state;

    static const char *conn_str[] = {"NONE", "DEFAULT", "1.5A", "3.0A", "PD_CONTRACT"};
    const char *conn              = (s->connection <= STUSB4531_CONN_PD_CONTRACT) ? conn_str[s->connection] : "?";

    platform_log(PLATFORM_LOG_INFO, "--- STUSB4531 State Dump (addr=0x%02X) ---", h->i2c_addr);
    platform_log(PLATFORM_LOG_INFO, "  DeviceID      : 0x%02X", s->device_id);
    platform_log(PLATFORM_LOG_INFO, "  Connection    : %s", conn);
    platform_log(PLATFORM_LOG_INFO, "  PD Contract   : %s (PDO#%d, RDO=0x%08lX)", s->pd_contract_active ? "YES" : "NO",
                 s->selected_pdo_index, (unsigned long)s->rdo_raw);
    platform_log(PLATFORM_LOG_INFO, "  CC1 state     : %s", conn_str[s->cc.cc1_state]);
    platform_log(PLATFORM_LOG_INFO, "  CC2 state     : %s", conn_str[s->cc.cc2_state]);
    platform_log(PLATFORM_LOG_INFO, "  CC reverse    : %s", s->cc.cc_reverse ? "YES" : "NO");
    platform_log(PLATFORM_LOG_INFO, "  CC connected  : %s", s->cc.connected ? "YES" : "NO");
    platform_log(PLATFORM_LOG_INFO, "  CC looking    : %s", s->cc.looking ? "YES" : "NO");
    platform_log(PLATFORM_LOG_INFO, "  VBUS vsafe0v  : %s", s->vbus.vsafe0v ? "YES" : "NO");
    platform_log(PLATFORM_LOG_INFO, "  VBUS valid    : %s", s->vbus.valid ? "YES" : "NO");
    platform_log(PLATFORM_LOG_INFO, "  VBUS OV       : %s", s->vbus.over_voltage ? "YES" : "NO");
    platform_log(PLATFORM_LOG_INFO, "  VBUS UV       : %s", s->vbus.under_voltage ? "YES" : "NO");
    platform_log(PLATFORM_LOG_INFO, "  Alert status  : 0x%02X", s->alert_status);
    platform_log(PLATFORM_LOG_INFO, "  HW fault      : %s", s->hw_fault ? "YES" : "NO");
    platform_log(PLATFORM_LOG_INFO, "  Msg received  : %s", s->msg_received ? "YES" : "NO");
    platform_log(PLATFORM_LOG_INFO, "  PE FSM state  : 0x%02X", s->pe_fsm_state);
    platform_log(PLATFORM_LOG_INFO, "-------------------------------------------");
}

stusb4531_status_t stusb4531_read_reg(stusb4531_handle_t *h, uint8_t reg, uint8_t *value) {
    CHECK_INIT(h);
    CHECK_PTR(value);
    return reg_read(h, reg, value);
}

stusb4531_status_t stusb4531_write_reg(stusb4531_handle_t *h, uint8_t reg, uint8_t value) {
    CHECK_INIT(h);
    return reg_write(h, reg, value);
}

/* Attendre que le bit REQ retombe (opération NVM terminée) */
stusb4531_status_t nvm_wait_ready(stusb4531_handle_t *h) {
    uint32_t t0 = platform_get_tick_ms();
    uint8_t reg_nvm_status;
    do {
        stusb4531_status_t st = reg_read(h, REG_NVM_STATUS, &reg_nvm_status);
        if (st != STUSB4531_OK)
            return st;
        if ((platform_get_tick_ms() - t0) > NVM_OP_TIMEOUT_MS) {
            LOG_E("NVM wait_ready timeout. All bits not set:%02X", reg_nvm_status);
            return STUSB4531_ERR_TIMEOUT;
        }
        platform_delay_ms(1);
    } while ((reg_nvm_status) != 0b11001111); // & (0b11 << 6)) != 0b11000000);
    return STUSB4531_OK;
}

/* Envoyer une opcode NVM pour un secteur donné */
static stusb4531_status_t nvm_execute(stusb4531_handle_t *h, uint8_t opcode, uint8_t sector) {
    uint8_t reg_nvm_cust_ctrl = (1 << NVM_MODE) | (sector << NVM_CUST_SELECT) | opcode;

    stusb4531_status_t st = reg_write(h, REG_NVM_CUST_CTRL, reg_nvm_cust_ctrl);
    if (st != STUSB4531_OK)
        return st;

    return nvm_wait_ready(h);
}

/* ---- API publique NVM ---- */

stusb4531_status_t stusb4531_nvm_read(stusb4531_handle_t *h, stusb4531_nvm_image_t *image) {
    CHECK_INIT(h);
    CHECK_PTR(image);

    memset(image, 0, sizeof(*image));

    stusb4531_status_t st = STUSB4531_OK;
    if (st != STUSB4531_OK)
        return st;

    for (uint8_t s = 0U; s < STUSB4531_NVM_SECTOR_COUNT; s++) {
        st = nvm_execute(h, NVM_OPCODE_READ_CUST, s);
        if (st != STUSB4531_OK)
            goto out;

        st = reg_read_buf(h, REG_DPM_SRC_PDO1, image->raw[s], STUSB4531_NVM_SECTOR_SIZE);
        if (st != STUSB4531_OK)
            goto out;

        LOG_D("NVM read sector %d OK", s);
        platform_delay_ms(NVM_INTER_OP_DELAY);
    }
    image->valid = true;

out:
    return st;
}

stusb4531_status_t stusb4531_nvm_write(stusb4531_handle_t *h, const stusb4531_nvm_image_t *image) {
    CHECK_INIT(h);
    CHECK_PTR(image);

    if (!image->valid) {
        LOG_E("nvm_write: image not valid");
        return STUSB4531_ERR_PARAM;
    }

    stusb4531_status_t st = STUSB4531_OK;
    if (st != STUSB4531_OK)
        return st;

    /* --- Écriture secteur par secteur --- */
    for (uint8_t s = 0U; s < STUSB4531_NVM_SECTOR_COUNT; s++) {

        /* 1. Charger les données dans le Program Load register */
        st = reg_write_buf(h, REG_DPM_SRC_PDO1, image->raw[s], STUSB4531_NVM_SECTOR_SIZE);
        if (st != STUSB4531_OK)
            goto out;

        st = nvm_execute(h, NVM_OPCODE_WRITE_CUST, s);
        if (st != STUSB4531_OK)
            goto out;

        LOG_D("NVM write sector %d OK", s);
        platform_delay_ms(NVM_INTER_OP_DELAY);
    }

    /* --- Vérification (relecture) --- */
    stusb4531_nvm_image_t verify;
    st = stusb4531_nvm_read(h, &verify);
    if (st != STUSB4531_OK)
        goto out;

    for (uint8_t s = 0U; s < STUSB4531_NVM_SECTOR_COUNT; s++) {
        if (memcmp(image->raw[s], verify.raw[s], STUSB4531_NVM_SECTOR_SIZE) != 0) {
            LOG_E("NVM verify failed at sector %d", s);
            st = STUSB4531_ERR_NVM_VERIFY;
            goto out;
        }
    }
    LOG_I("NVM write + verify OK");

out:
    return st;
}

void stusb4531_nvm_dump(const stusb4531_nvm_image_t *image) {
    if (!image)
        return;

    platform_log(PLATFORM_LOG_INFO, "--- NVM Image dump (valid=%d) ---", image->valid);
    for (uint8_t s = 0U; s < STUSB4531_NVM_SECTOR_COUNT; s++) {
        platform_log(PLATFORM_LOG_INFO, "  Sector %d: %02X %02X %02X %02X %02X %02X %02X %02X", s, image->raw[s][0],
                     image->raw[s][1], image->raw[s][2], image->raw[s][3], image->raw[s][4], image->raw[s][5],
                     image->raw[s][6], image->raw[s][7]);
    }
    platform_log(PLATFORM_LOG_INFO, "---------------------------------");
}