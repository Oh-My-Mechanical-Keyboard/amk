/**
 * webusb.c
 */

#include "tusb.h"
#include "usb_descriptors.h"
#include "amk_printf.h"

#if TINYUSB_ENABLE

//--------------------------------------------------------------------+
// WebUSB use vendor class
//--------------------------------------------------------------------+

// Invoked when a control transfer occurred on an interface of this class
// Driver response accordingly to the request and the transfer stage (setup/data/ack)
// return false to stall control endpoint (e.g unsupported request)
bool tud_vendor_control_xfer_cb(uint8_t rhport, uint8_t stage, tusb_control_request_t const * request)
{
    // nothing to with DATA & ACK stage
    if (stage != CONTROL_STAGE_SETUP ) return true;

    switch (request->bRequest) {
        case VENDOR_REQUEST_WEBUSB:
        // match vendor request in BOS descriptor
        // Get landing page url
        return tud_control_xfer(rhport, request, (void*) tud_descriptor_url_cb(), tud_descriptor_url_size());

        case VENDOR_REQUEST_MICROSOFT:
            if ( request->wIndex == 7 ) {
                // Get Microsoft OS 2.0 compatible descriptor
                return tud_control_xfer(rhport, request, (void*) tud_descriptor_msos20_cb(), tud_descriptor_msos20_size());
            }else {
                return false;
            }

        //case 0x22:
        // Webserial simulate the CDC_REQUEST_SET_CONTROL_LINE_STATE (0x22) to
        // connect and disconnect.
        //web_serial_connected = (request->wValue != 0);

        // Always lit LED if connected
        //if ( web_serial_connected )
        //{
        //    board_led_write(true);
        //    blink_interval_ms = BLINK_ALWAYS_ON;

        //    tud_vendor_write_str("\r\nTinyUSB WebUSB device example\r\n");
        //}else
        //{
        //    blink_interval_ms = BLINK_MOUNTED;
        //}

        // response with status OK
        //return tud_control_status(rhport, request);

        default:
            // stall unknown request
            return false;
    }

    return true;
}
extern void amk_keymap_set(uint8_t layer, uint8_t row, uint8_t col, uint16_t keycode);
extern uint16_t amk_keymap_get(uint8_t layer, uint8_t row, uint8_t col);

static uint8_t webusb_buffer[CFG_TUD_VENDOR_EPSIZE];
void webusb_task(void)
{
    if (tud_vendor_available()) {
        uint32_t readed = tud_vendor_read(webusb_buffer, CFG_TUD_VENDOR_EPSIZE);
        if (readed > 0) {
            uint8_t cmd = webusb_buffer[0];
            amk_printf("WEBUSB readed: epnum=%d, readed=%d, commoand=%d\n", ITF_NUM_VENDOR, readed, cmd);
            switch(cmd) {
                case WEBUSB_KEYMAP_SET: {
                    uint8_t layer   = webusb_buffer[1];
                    uint8_t row     = webusb_buffer[2];
                    uint8_t col     = webusb_buffer[3];
                    uint16_t code   = (webusb_buffer[5]<<8) | webusb_buffer[4];
                    amk_keymap_set(layer, row, col, code);
                    amk_printf("webusb set keycode: layer=%d, row=%d, col=%d, keycode=%d\n", layer, row, col, code);
                    tud_vendor_write(webusb_buffer, 8);
                    }break;
                case WEBUSB_KEYMAP_GET: {
                    uint8_t layer   = webusb_buffer[1];
                    uint8_t row     = webusb_buffer[2];
                    uint8_t col     = webusb_buffer[3];
                    uint16_t code   = amk_keymap_get(layer, row, col);
                    amk_printf("webusb get keycode:layer=%d, row=%d, col=%d, keycode=%d\n", layer, row, col, code);
                    webusb_buffer[4] = code&0xFF;
                    webusb_buffer[5] = (code>>8)&0xFF;
                    tud_vendor_write(webusb_buffer, 8);
                    } break;
                default:
                    amk_printf("WEBUSB unknown command: %d\n", cmd);
                    break;
            }
        }
    }
}

#endif