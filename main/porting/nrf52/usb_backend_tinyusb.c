/**
 * usb_backend_tinyusb.c
 *     usb backend with tinyusb
 */

#include "usb_interface.h"
#include "nrf_delay.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_power.h"
#include "usb_descriptors.h"
#include "suspend.h"
#include "report_queue.h"

static nrf_usb_event_handler_t nrf_usb_event;

static hid_report_queue_t report_queue;

// Forward USB interrupt events to TinyUSB IRQ Handler
void USBD_IRQHandler(void)
{
    tud_int_handler(0);
}

extern void tusb_hal_nrf_power_event(uint32_t event);

// power event handler
static void nrf_usb_power_event_handler(nrf_drv_power_usb_evt_t event);

static bool nrf_usb_itf_ready(uint32_t type)
{
    switch(type) {
    case NRF_REPORT_ID_KEYBOARD:
        return tud_hid_n_ready(ITF_NUM_HID_KBD);
    case NRF_REPORT_ID_MOUSE:
    case NRF_REPORT_ID_SYSTEM:
    case NRF_REPORT_ID_CONSUMER:
        return tud_hid_n_ready(ITF_NUM_HID_OTHER);
    default:
        break;
    }
    return false;
}


static bool usb_itf_send_report(uint32_t report_type, const void* data, uint32_t size)
{
    switch(report_type) {
    case NRF_REPORT_ID_KEYBOARD:
        if (!tud_hid_n_report(ITF_NUM_HID_KBD, HID_REPORT_ID_KEYBOARD, data, (uint8_t)size)) {
            NRF_LOG_INFO("failed to sent keyboard report");
            return false;
        }
        break;
    case NRF_REPORT_ID_MOUSE:
        if (!tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_MOUSE, data, (uint8_t)size)) {
            NRF_LOG_INFO("failed to sent mouse report");
            return false;
        }
        break;
    case NRF_REPORT_ID_SYSTEM:
        if (!tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_SYSTEM, data, (uint8_t)size)) {
            NRF_LOG_INFO("failed to sent system report");
            return false;
        }
        break;
    case NRF_REPORT_ID_CONSUMER:
        if (!tud_hid_n_report(ITF_NUM_HID_OTHER, HID_REPORT_ID_CONSUMER, data, (uint8_t)size)) {
            NRF_LOG_INFO("failed to sent consumer report");
            return false;
        }
        break;
    default:
        NRF_LOG_INFO("unknonw report type: %d", report_type);
        return false;
    }
    return true;
}

void nrf_usb_preinit(void)
{
    ret_code_t rc;
    if (!nrf_drv_clock_init_check()) {
        rc = nrf_drv_clock_init();
        APP_ERROR_CHECK(rc);
    }
}

void nrf_usb_init(nrf_usb_event_handler_t* eh)
{
    nrf_usb_event.disable_cb = eh->disable_cb;
    nrf_usb_event.enable_cb  = eh->enable_cb;
    nrf_usb_event.suspend_cb = eh->suspend_cb;
    nrf_usb_event.resume_cb  = eh->resume_cb;
    nrf_usb_event.leds_cb    = eh->leds_cb;

    hid_report_queue_init(&report_queue);
}

void nrf_usb_postinit(void)
{
    // Priorities 0, 1, 4 (nRF52) are reserved for SoftDevice
    // 2 is highest for application
    NVIC_SetPriority(USBD_IRQn, 2);

    // initialize usb power event
    ret_code_t ret;
    static const nrf_drv_power_usbevt_config_t config =
    {
        .handler = nrf_usb_power_event_handler 
    };

    ret = nrf_drv_power_usbevt_init(&config);
    APP_ERROR_CHECK(ret);

    tusb_init();
}

void nrf_usb_task(void)
{
    if (!hid_report_queue_empty(&report_queue)) {
        hid_report_t* item = hid_report_queue_peek(&report_queue);
        if (nrf_usb_itf_ready(item->type)) {
            NRF_LOG_INFO("ITF ready, type:%d, send report", item->type);
            hid_report_t report;
            hid_report_queue_get(&report_queue, &report);
            usb_itf_send_report(report.type, report.data, report.size);
        }
    }

    tud_task();

    if (tud_suspended()) {
        if (suspend_wakeup_condition()) {
            // wake up remote
            nrf_usb_wakeup();
        }
    }

}

void nrf_usb_send_report(nrf_report_id report, const void *data, size_t size)
{
    hid_report_t item;
    memcpy(item.data, data, size);
    item.type = report;
    item.size = size;
    hid_report_queue_put(&report_queue, &item);
}

void nrf_usb_wakeup(void) 
{
    tud_remote_wakeup();
}

void nrf_usb_prepare_sleep(void) { }

#define UF2_BOOT_MAGIC 0x57
void nrf_usb_reboot(void)
{
    sd_power_gpregret_set(RST_REGISTER, UF2_BOOT_MAGIC);
    sd_nvic_SystemReset();
}

void nrf_usb_power_event_handler(nrfx_power_usb_evt_t event)
{
    switch(event) {
    case NRFX_POWER_USB_EVT_DETECTED:
        NRF_LOG_INFO("USB detected");
        break;
    case NRFX_POWER_USB_EVT_REMOVED:
        NRF_LOG_INFO("USB removed");
        nrf_usb_event.disable_cb();
        break;
    case NRFX_POWER_USB_EVT_READY:
        NRF_LOG_INFO("USB ready");
        nrf_usb_event.enable_cb();
        break;
    default:
        break;
    }
    tusb_hal_nrf_power_event(event);
}

//=============================
// tusb callback
//=============================
// Invoked when device is mounted
void tud_mount_cb(void)
{
}

// Invoked when device is unmounted
void tud_umount_cb(void)
{
}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en)
{
    (void) remote_wakeup_en;
    nrf_usb_event.suspend_cb(true);
}

// Invoked when usb bus is resumed
void tud_resume_cb(void)
{
    nrf_usb_event.resume_cb();
}


// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t* buffer, uint16_t reqlen)
{
    // TODO not Implemented
    (void) itf;
    (void) report_id;
    (void) report_type;
    (void) buffer;
    (void) reqlen;

    return 0;
}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id, hid_report_type_t report_type, uint8_t const* buffer, uint16_t bufsize)
{
    // TODO set LED based on CAPLOCK, NUMLOCK etc...
    NRF_LOG_INFO("USBD SET_REPORT: interface=%d, report_id=%d, report_type=%d, size=%d", itf, report_id, report_type, bufsize);
    if (itf == ITF_NUM_HID_KBD && report_type == HID_REPORT_TYPE_OUTPUT) {
        if (bufsize>0) {
            nrf_usb_event.leds_cb(buffer[0]);
        }
    }
}