/**
 * @file    stusb4531_regs.h
 * @brief   STUSB4531 – Adresses de registres et masques de bits
 *
 * Référence : STUSB4531 datasheet, Rev. 5
 * Ce fichier est purement descriptif, aucune logique applicative ici.
 */

#ifndef STUSB4531_REGS_H
#define STUSB4531_REGS_H

#ifdef __cplusplus
extern "C" {
#endif

#define STUSB4531_DEVICE_HW_ID_VALUE 0x12U /** Reset Hardware ID value */

/* =========================================================================
 * Adresse I2C par défaut (7 bits, ADDR pin = 0)
 * ========================================================================= */

#define STUSB4531_I2C_ADDR_DEFAULT 0x28U /**< ADDR = 0b00 */
#define STUSB4531_I2C_ADDR_01      0x29U /**< ADDR = 0b01 */
#define STUSB4531_I2C_ADDR_10      0x2AU /**< ADDR = 0b10 */
#define STUSB4531_I2C_ADDR_11      0x2BU /**< ADDR = 0b11 */

/* =========================================================================
 * Registres d'identification
 * ========================================================================= */

#define REG_VID_LOW     0x0000U /**< VID_LOW (1 octet) */
#define REG_VID_HIGH    0x0001U /**< VID_HIGH (1 octet) */
#define REG_PID_LOW     0x0002U /**< PID_LOW (1 octet) */
#define REG_PID_HIGH    0x0003U /**< PID_HIGH (1 octet) */
#define REG_XID_0       0x0004U /**< XID_0 (1 octet) */
#define REG_XID_1       0x0005U /**< XID_1 (1 octet) */
#define REG_XID_2       0x0006U /**< XID_2 (1 octet) */
#define REG_XID_3       0x0007U /**< XID_3 (1 octet) */
#define REG_FW_REVISION 0x0008U /**< FW_REVISION (1 octet) */
#define REG_HW_REVISION 0x0009U /**< HW_REVISION (1 octet) */
#define REG_PD_REVISION 0x000AU /**< PD_REVISION (1 octet) */
#define REG_PD_VERSION  0x000BU /**< PD_VERSION (1 octet) */
#define REG_DEVICE_HW   0x000CU /**< DEVICE_HW (1 octet) */

/* =========================================================================
 * Registres d'état et d'alerte
 * ========================================================================= */

#define REG_ALERT_STATUS           0x0010U /**< ALERT_STATUS (1 octet) */
#define REG_ALERT_STATUS_MASK      0x0011U /**< ALERT_STATUS_MASK (1 octet) */
#define REG_PD_STATUS_EVENT        0x0012U /**< PD_STATUS_EVENT (1 octet) */
#define REG_PD_STATUS_EVENT_MASK   0x0013U /**< PD_STATUS_EVENT_MASK (1 octet) */
#define REG_MONITORING_TRANS       0x0014U /**< MONITORING_TRANS (1 octet) */
#define REG_MONITORING_MASK        0x0015U /**< MONITORING_MASK (1 octet) */
#define REG_MONITORING_STATUS      0x0016U /**< MONITORING_STATUS (1 octet) */
#define REG_PRL_TRANS              0x0017U /**< PRL_TRANS (1 octet) */
#define REG_PRL_STATUS_MASK        0x0018U /**< PRL_STATUS_MASK (1 octet) */
#define REG_VBUS_STATUS            0x0019U /**< VBUS_STATUS (1 octet) */
#define REG_CC_STATUS              0x001AU /**< CC_STATUS (1 octet) */
#define REG_PD_STATUS              0x001BU /**< PD_STATUS (1 octet) */
#define REG_HW_FAULT_TRANS         0x001CU /**< HW_FAULT_TRANS (1 octet) */
#define REG_HW_FAULT_STATUS        0x001DU /**< HW_FAULT_STATUS (1 octet) */
#define REG_GPIO_STATUS            0x001EU /**< GPIO_STATUS (1 octet) */
#define REG_NVM_STATUS             0x001FU /**< NVM_STATUS (1 octet) */
#define REG_TYPEC_FSM              0x0020U /**< TYPEC_FSM (1 octet) */
#define REG_PE_FSM                 0x0021U /**< PE_FSM (1 octet) */
#define REG_VBUS_FSM               0x0022U /**< VBUS_FSM (1 octet) */
#define REG_MONITORING_DACH_STATUS 0x0023U /**< MONITORING_DACH_STATUS (2 octets) */
#define REG_MONITORING_DACL_STATUS 0x0025U /**< MONITORING_DACL_STATUS (2 octets) */
#define REG_CONNECTION_TRANS       0x0027U /**< CONNECTION_TRANS (1 octet) */
#define REG_CONNECTION_MASK        0x0028U /**< CONNECTION_MASK (1 octet) */

/* =========================================================================
 * Registres de commande et configuration
 * ========================================================================= */

#define REG_COMMAND                  0x0031U /**< COMMAND (1 octet) */
#define REG_RX_MSG_DETECT            0x0032U /**< RX_MSG_DETECT (1 octet) */
#define REG_RESET_CTRL               0x0033U /**< RESET_CTRL (1 octet) */
#define REG_VBUS_CTRL                0x0034U /**< VBUS_CTRL (1 octet) */
#define REG_MONITORING_CTRL          0x0035U /**< MONITORING_CTRL (1 octet) */
#define REG_MONITORING_DACH_CTRL     0x0036U /**< MONITORING_DACH_CTRL (2 octets) */
#define REG_MONITORING_DACL_CTRL     0x0038U /**< MONITORING_DACL_CTRL (2 octets) */
#define REG_MONITORING_SHIFT_CTRL    0x003AU /**< MONITORING_SHIFT_CTRL (1 octet) */
#define REG_VBUS_DISCHARGE_CTRL      0x003BU /**< VBUS_DISCHARGE_CTRL (1 octet) */
#define REG_VBUS_DISCHARGE_TIME_CTRL 0x003CU /**< VBUS_DISCHARGE_TIME_CTRL (1 octet) */
#define REG_DEVICE_CTRL              0x003DU /**< DEVICE_CTRL (1 octet) */
#define REG_APPLI_CTRL               0x003EU /**< APPLI_CTRL (1 octet) */
#define REG_DPM_CTRL                 0x003FU /**< DPM_CTRL (1 octet) */
#define REG_GPIO_CTRL                0x0040U /**< GPIO_CTRL (1 octet) */
#define REG_NVM_CUST_CTRL            0x0041U /**< NVM_CUST_CTRL (1 octet) */

/* =========================================================================
 * Registres de configuration PD / NVM
 * ========================================================================= */

#define REG_GPIO_SETTING         0x0050U /**< GPIO_SETTING (1 octet) */
#define REG_DEVICE_PDP           0x0051U /**< DEVICE_PDP (1 octet) */
#define REG_NUM_PDO              0x0052U /**< NUM_PDO (1 octet) */
#define REG_SNK_PDO_PARAMS       0x0053U /**< SNK_PDO_PARAMS (1 octet) */
#define REG_SNK_PDO_CAPABILITIES 0x0054U /**< SNK_PDO_CAPABILITIES (2 octets) */
#define REG_ALGO                 0x0056U /**< ALGO (1 octet) */
#define REG_SNK_APDO_FILL_1      0x0057U /**< SNK_APDO_FILL_1 (1 octet) */
#define REG_SNK_APDO_FILL_2      0x0058U /**< SNK_APDO_FILL_2 (1 octet) */
#define REG_SNK_APDO_FILL_3      0x0059U /**< SNK_APDO_FILL_3 (1 octet) */
#define REG_DEVICE_SETTING       0x005AU /**< DEVICE_SETTING (1 octet) */
#define REG_REQUEST_SRC_PDP      0x005CU /**< REQUEST_SRC_PDP (1 octet) */

/* =========================================================================
 * Registres RX / TX
 * ========================================================================= */

#define REG_RX_BYTE_CNT         0x0060U /**< RX_BYTE_CNT (1 octet) */
#define REG_RX_FRAME_TYPE       0x0061U /**< RX_FRAME_TYPE (1 octet) */
#define REG_RX_HEADER           0x0062U /**< RX_HEADER (2 octets) */
#define REG_RX_DATA_OBJ_224BITS 0x0064U /**< RX_DATA_OBJ_224BITS (32 octets) */
#define REG_PRL_TX_CTRL         0x0080U /**< PRL_TX_CTRL (1 octet) */
#define REG_TX_BYTE_CNT         0x0081U /**< TX_BYTE_CNT (1 octet) */
#define REG_TX_HEADER           0x0082U /**< TX_HEADER (2 octets) */
#define REG_TX_DATA_OBJ_224BITS 0x0084U /**< TX_DATA_OBJ_224BITS (32 octets) */

/* =========================================================================
 * Registres DPM
 * ========================================================================= */

#define REG_DPM_SRC_PDO1           0x00A0U /**< DPM_SRC_PDO1 (4 octets) */
#define REG_DPM_SRC_PDO2           0x00A4U /**< DPM_SRC_PDO2 (4 octets) */
#define REG_DPM_SRC_PDO3           0x00A8U /**< DPM_SRC_PDO3 (4 octets) */
#define REG_DPM_SRC_PDO4           0x00ACU /**< DPM_SRC_PDO4 (4 octets) */
#define REG_DPM_SRC_PDO5           0x00B0U /**< DPM_SRC_PDO5 (4 octets) */
#define REG_DPM_SRC_PDO6           0x00B4U /**< DPM_SRC_PDO6 (4 octets) */
#define REG_DPM_SRC_PDO7           0x00B8U /**< DPM_SRC_PDO7 (4 octets) */
#define REG_DPM_RDO                0x00BCU /**< DPM_RDO (4 octets) */
#define REG_DPM_ALGO_RESULT        0x00C0U /**< DPM_ALGO_RESULT (4 octets) */
#define REG_DPM_SRC_PDO_NEGOCIATED 0x00C4U /**< DPM_SRC_PDO_NEGOCIATED (4 octets) */

#define STUSB4531_NVM_SECTOR_SIZE 8U /**< Taille d'un secteur en octets */

/* =========================================================================
 * Masques – REG_ALERT_STATUS (0x0B)
 * ========================================================================= */
#define ALERT_MONITORING_STATUS_AL (1U << 0)
#define ALERT_PRL_STATUS_AL        (1U << 1)
#define ALERT_PD_STATUS_AL         (1U << 2) /* inutilisé sur 4531 */
#define ALERT_HW_FAULT_STATUS_AL   (1U << 3)
#define ALERT_PORT_STATUS_AL       (1U << 4)

/* =========================================================================
 * Masques – REG_PORT_STATUS_1 (0x0E)
 * ========================================================================= */

#define PORT_STATUS_DATA_MODE_MASK  (0x7U << 1)
#define PORT_STATUS_POWER_MODE_MASK (0x3U << 4)
#define PORT_STATUS_CONNECT_RESULT  (1U << 6)

/* =========================================================================
 * Masques – REG_CC_STATUS (0x11)
 * ========================================================================= */
#define CC_STATUS_CC1_STATE_MASK (3U << 0)
#define CC_STATUS_CC2_STATE_MASK (3U << 2)
#define CC_STATUS_CONNECT_RESULT (1U << 4)
#define CC_STATUS_LOOKING4CON    (1U << 5)
#define CC_STATUS_ATTACH         (1U << 6)
#define CC_STATUS_CC_REVERSE     (1U << 7)
#define CC_STATUS_SNK_PWRDEFAULT (0U)
#define CC_STATUS_SNK_PWR1_5A    (1U)
#define CC_STATUS_SNK_PWR3_0A    (2U)

/* =========================================================================
 * Masques – REG_TYPEC_MONITORING_STATUS_1 (0x10)
 * ========================================================================= */
#define MONITORING_VBUS_VSAFE0V      (1U << 0)
#define MONITORING_VBUS_VALID        (1U << 1)
#define MONITORING_VBUS_OVERVOLTAGE  (1U << 3)
#define MONITORING_VBUS_UNDERVOLTAGE (1U << 4)

/* =========================================================================
 * Masques – REG_PRL_STATUS (0x16)
 * ========================================================================= */
#define PRL_HR_RECEIVED         (1U << 0)
#define PRL_HR_DONE             (1U << 1)
#define PRL_STATUS_MSG_RECEIVED (1U << 2)
#define PRL_STATUS_MSG_SENT     (1U << 3)
#define PRL_TX_DISCARD          (1U << 6)
#define PRL_TX_ERROR            (1U << 7)

/* =========================================================================
 * Masques – REG_PD_COMMAND_CTRL (0x1A)
 * ========================================================================= */
#define PD_CMD_SEND_MSG         0x26U /**< Envoyer un message USB PD */
#define PD_CMD_REQUEST_NEW_CAPS 0x3EU /**< Demander de nouvelles capacités */
#define PD_CMD_GET_SNK_CAP      0x3FU /**< Demander les capacités sink */

/* =========================================================================
 * Masques – REG_RESET_CTRL (0x23)
 * ========================================================================= */
#define RESET_SW_RESET_EN (1U << 0)

/* =========================================================================
 * Masques – REG_NUM_PDO (0x70)
 * ========================================================================= */
#define NUM_PDO_SNK_MASK (0x3U << 0)
#define NUM_PDO_SRC_MASK (0x7U << 4)

/* =========================================================================
 * Masques – REG_FTP_CTRL_0 (0x96)
 * ========================================================================= */
#define FTP_CTRL0_SECT_NUM_MASK (0x07U << 0)
#define FTP_CTRL0_PWR_MASK      (0x07U << 3)
#define FTP_CTRL0_OPCODE_MASK   (0x07U << 6) /* sur 3 bits, spread entre 0x96 et 0x97 */
#define FTP_CTRL0_REQ           (1U << 7)

/* =========================================================================
 * Masques – REG_FTP_CTRL_1 (0x97)
 * ========================================================================= */
#define FTP_CTRL1_OPCODE_MASK   (0x07U << 0)
#define FTP_CTRL1_SECT_NUM_MASK (0x07U << 3)

/* =========================================================================
 * NVM_CUST_CTRL (0x41)
 * ========================================================================= */
/* Opcodes NVM */
#define NVM_MODE                    7U    /** Position du bit de mode NVM dans NVM_CUST_CTRL */
#define NVM_CUST_SELECT             3U    /** Position des bits indiquants le nombre de secteurs */
#define NVM_OPCODE_NOP              0x00U /** No operation */
#define NVM_OPCODE_READ_CUST        0x01U /** read NVM to SRC_PDO0 and SRC_PDO1 registers (8bytes) */
#define NVM_OPCODE_WRITE_CUST       0x02U /** copy SRC_PDO0 and SRC_PDO1 registers (8bytes) to NVM. */
#define NVM_OPCODE_WRITE_FUNC_REG   0x03U /** copy functional registers to NVM. */
#define NVM_OPCODE_WRITE_2_FUNC_REG 0x04U /** copy NVM to the functional registers */

/* =========================================================================
 * Device ID attendu
 * ========================================================================= */
#define STUSB4531_DEVICE_ID_VALUE 0x21U

#ifdef __cplusplus
}
#endif

#endif /* STUSB4531_REGS_H */
