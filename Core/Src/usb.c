#include "usb.h"
#include "usbd_cdc_if.h"
#include <stdint.h>
#include <string.h>

#define USB_BUFF_SIZE 100

uint8_t usb_buff[USB_BUFF_SIZE];
uint32_t usb_data_length = 0;
bool usb_rx_sth          = false;

/** Fonction privée copiant les données dès leur réception usb */
void USB_MemBuff(uint8_t *buff, uint32_t len);

uint32_t usb_data_available() { return usb_rx_sth ? usb_data_length : 0; }

void usb_get(uint8_t *data, uint32_t len) {
    if (len > usb_data_length)
        len = usb_data_length;

    memcpy(data, usb_buff, len);
    usb_rx_sth = false;
}

void usb_get_str(char *str, uint32_t max_len) {
    uint32_t len = usb_data_available();
    if (len >= max_len)
        len = max_len - 1;

    memcpy(str, usb_buff, len);
    usb_rx_sth = false;
    str[len]   = '\0';
}

void usb_send(uint8_t *data, uint32_t len) { CDC_Transmit_FS(data, len); }

void USB_MemBuff(uint8_t *buff, uint32_t len) {
    if (len >= USB_BUFF_SIZE) {
        len = USB_BUFF_SIZE - 1;
    }
    memcpy(usb_buff, buff, len);
    usb_data_length = len;
    usb_rx_sth      = true;
}

/** Redirection de printf vers USB */
int _write(int file, char *ptr, int len) {
    CDC_Transmit_FS((uint8_t *)ptr, len);
    return len;
}