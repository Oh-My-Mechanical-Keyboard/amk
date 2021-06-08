/**
 * generic_board.c
 */

#include "generic_hal.h"
#include "board.h"
#include "amk_printf.h"
#include "usb_interface.h"
#include "amk_keymap.h"

#include "report.h"
#include "host.h"
#include "keyboard.h"
#include "suspend.h"
#include "action_util.h"
#include "mousekey.h"
#include "amk_boot.h"

#ifdef SCREEN_ENABLE
#include "screen.h"
#endif

#ifdef WEBUSB_ENABLE
#include "webusb.h"
#endif

#ifdef MSC_ENABLE
#include "mscusb.h"
#endif

#include "rgb_led.h"

extern void system_init(void);
extern void custom_board_init(void);
extern void custom_board_task(void);

/**
 * tmk related stuff
 */
static uint8_t keyboard_leds(void);
static void send_keyboard(report_keyboard_t *report);
static void send_mouse(report_mouse_t *report);
static void send_system(uint16_t data);
static void send_consumer(uint16_t data);

uint8_t amk_led_state = 0;
static void remote_wakeup(void);

/* host struct */
host_driver_t amk_driver = {
    keyboard_leds,
    send_keyboard,
    send_mouse,
    send_system,
    send_consumer
};

static void amk_init(void);

void board_init(void)
{
    system_init();
    custom_board_init();
    usb_init();
    amk_init();
}


void board_task(void)
{
    usb_task();

    if (usb_suspended()) {
        if (suspend_wakeup_condition()) {
            // wake up remote
            remote_wakeup();
        }
    } else {
        //if (usb_ready()) {
        keyboard_task();
        //}
    }

#ifdef SCREEN_ENABLE
    screen_task();
#endif

#ifdef MSC_ENABLE
    msc_task();
#endif

#ifdef WEBUSB_ENABLE
    webusb_task();
#endif

    rgb_led_task();

    custom_board_task();
}

static void amk_init(void)
{
#ifdef SCREEN_ENABLE
    screen_init();
#endif

#ifdef MSC_ENABLE
    msc_init();
#endif

    keyboard_init();
    boot_init();
    host_set_driver(&amk_driver);
    amk_keymap_init();
}

// tmk integration
uint8_t keyboard_leds(void)
{
    return amk_led_state;
}

void send_keyboard(report_keyboard_t *report)
{
    usb_send_report(HID_REPORT_ID_KEYBOARD, report, sizeof(report_keyboard_t));
}

void send_mouse(report_mouse_t *report)
{
    usb_send_report(HID_REPORT_ID_MOUSE, report, sizeof(report_mouse_t));
}

void send_system(uint16_t data)
{
    usb_send_report(HID_REPORT_ID_SYSTEM, &data, sizeof(data));
}

void send_consumer(uint16_t data)
{
    usb_send_report(HID_REPORT_ID_CONSUMER, &data, sizeof(data));
}

void remote_wakeup(void)
{
    usb_remote_wakeup();

    suspend_wakeup_init();
    // cleanup the host keyboard state
    send_keyboard_report();
    mousekey_send();
}
