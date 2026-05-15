#include "core.h"
#include "motor.h"
#include "stm32g4xx_hal.h"
#include "usbd_cdc_if.h"
// #include <cstdlib>
#include <stdint.h>
#include <string.h>

#include "usb.h"
#include "stusb4531.h"

void setup() {
    motorInit();
}

void loop() {
    // HAL_Delay(200);

    /** Test d'envoi/réception */
    uint32_t usb_data_length = usb_data_available();
    if (usb_data_length) {
        uint8_t* data = malloc(usb_data_length);
        usb_get(data, usb_data_length);
        usb_send(data, usb_data_length);
        free(data);
    }
}

