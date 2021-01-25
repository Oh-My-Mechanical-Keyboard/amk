/**
 *  usb_descriptors.h
 */ 

#pragma once

#include "tusb.h"

#define NKRO_KEYCODE_SIZE   32

// Extra key report
#define TUD_HID_REPORT_DESC_EXTRA(system, consumer) \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP           )         ,\
    HID_USAGE      ( HID_USAGE_DESKTOP_SYSTEM_CONTROL )         ,\
    HID_COLLECTION ( HID_COLLECTION_APPLICATION       )         ,\
    HID_REPORT_ITEM( system, 8, RI_TYPE_GLOBAL, 1     )         ,\
    HID_LOGICAL_MIN_N( 0x0001, 2                           )    ,\
    HID_LOGICAL_MAX_N( 0x0037, 2                           )    ,\
    /* System Power Down */ \
    HID_USAGE_MIN_N  ( 0x0081, 2                           )    ,\
    /* System Display LCD Autoscale */ \
    HID_USAGE_MAX_N  ( 0x00B7, 2                           )    ,\
    HID_REPORT_COUNT ( 1                                   )    ,\
    HID_REPORT_SIZE  ( 16                                  )    ,\
    HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE )    ,\
    HID_COLLECTION_END, \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_CONSUMER    )               ,\
    HID_USAGE      ( HID_USAGE_CONSUMER_CONTROL )               ,\
    HID_COLLECTION ( HID_COLLECTION_APPLICATION )               ,\
    HID_REPORT_ITEM( consumer, 8, RI_TYPE_GLOBAL, 1        )    ,\
    HID_LOGICAL_MIN_N( 0x0001, 2                           )    ,\
    HID_LOGICAL_MAX_N( 0x029C, 2                           )    ,\
    HID_USAGE_MIN_N  ( 0x0001, 2                           )    ,\
    HID_USAGE_MAX_N  ( 0x029C, 2                           )    ,\
    HID_REPORT_COUNT ( 1                                   )    ,\
    HID_REPORT_SIZE  ( 16                                  )    ,\
    HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE )    ,\
    HID_COLLECTION_END \

#define TUD_HID_REPORT_DESC_NKRO(...) \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_DESKTOP           )         ,\
    HID_USAGE      ( HID_USAGE_DESKTOP_KEYBOARD       )         ,\
    HID_COLLECTION ( HID_COLLECTION_APPLICATION       )         ,\
    /* Report ID if any */ \
    __VA_ARGS__ \
    /* 8 bits Modifier Keys (Shfit, Control, Alt) */ \
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD )                       ,\
        HID_USAGE_MIN    ( 224                                    )  ,\
        HID_USAGE_MAX    ( 231                                    )  ,\
        HID_LOGICAL_MIN  ( 0                                      )  ,\
        HID_LOGICAL_MAX  ( 1                                      )  ,\
        HID_REPORT_COUNT ( 8                                      )  ,\
        HID_REPORT_SIZE  ( 1                                      )  ,\
        HID_INPUT        ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE )  ,\
    HID_USAGE_PAGE ( HID_USAGE_PAGE_KEYBOARD ) ,\
        HID_USAGE_MIN    ( 0                                   )  ,\
        HID_USAGE_MAX    ( (NKRO_KEYCODE_SIZE-1)*8             )  ,\
        HID_LOGICAL_MIN  ( 0                                   )  ,\
        HID_LOGICAL_MAX  ( 1                                   )  ,\
        HID_REPORT_COUNT ( (NKRO_KEYCODE_SIZE-1)*8             )  ,\
        HID_REPORT_SIZE  ( 1                                   )  ,\
        HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE )  ,\
    /* 5-bit LED Indicator Kana | Compose | ScrollLock | CapsLock | NumLock */ \
    HID_USAGE_PAGE  ( HID_USAGE_PAGE_LED ) ,\
        HID_USAGE_MIN    ( 1                                       ) ,\
        HID_USAGE_MAX    ( 5                                       ) ,\
        HID_REPORT_COUNT ( 5                                       ) ,\
        HID_REPORT_SIZE  ( 1                                       ) ,\
        HID_OUTPUT       ( HID_DATA | HID_VARIABLE | HID_ABSOLUTE  ) ,\
        /* led padding */ \
        HID_REPORT_COUNT ( 1                                       ) ,\
        HID_REPORT_SIZE  ( 3                                       ) ,\
        HID_OUTPUT       ( HID_CONSTANT                            ) ,\
    HID_COLLECTION_END \

// Interface number
enum {
    ITF_NUM_HID_KBD,
#ifdef SHARED_HID_EP
    #define ITF_NUM_HID_OTHER ITF_NUM_HID_KBD
#else
    ITF_NUM_HID_OTHER,
#endif
#ifdef WEBUSB_ENABLE
    ITF_NUM_VENDOR,
#endif
#ifdef MSC_ENABLE
    ITF_NUM_MSC,
#endif
    ITF_NUM_TOTAL
};

#ifdef SHARED_HID_EP
// Endpoint number
enum {
    EPNUM_HID_KBD       = 0x01,
#ifdef WEBUSB_ENABLE
    EPNUM_VENDOR_OUT    = 0x02,
    #if defined(STM32F103xB) || defined(NRF52840_XXAA)
    EPNUM_VENDOR_IN     = 0x03,
    #else
        #define EPNUM_VENDOR_IN EPNUM_VENDOR_OUT
    #endif
#endif
#ifdef MSC_ENABLE
    EPNUM_MSC_OUT       = 0x03,
    #if defined(STM32F103xB) || defined(NRF52840_XXAA)
    EPNUM_MSC_IN        = 0x04,
    #else
        #define EPNUM_MSC_IN    EPNUM_MSC_OUT
    #endif
#endif
    EPNUM_MAX
};
#else
enum {
    EPNUM_HID_KBD       = 0x01,
    EPNUM_HID_OTHER     = 0x02,
#ifdef WEBUSB_ENABLE
    EPNUM_VENDOR_OUT    = 0x03,
    #if defined(STM32F103xB) || defined(NRF52840_XXAA)
    EPNUM_VENDOR_IN     = 0x04,
    #else
        #define EPNUM_VENDOR_IN EPNUM_VENDOR_OUT
    #endif
#endif
#ifdef MSC_ENABLE
    EPNUM_MSC_OUT       = 0x03,
    #if defined(STM32F103xB) || defined(NRF52840_XXAA)
    EPNUM_MSC_IN        = 0x04,
    #else
        #define EPNUM_MSC_IN    EPNUM_MSC_OUT
    #endif
#endif
    EPNUM_MAX
};
#endif

// Report id
enum {
#ifdef SHARED_HID_EP
    HID_REPORT_ID_KEYBOARD = 1,
#else
    HID_REPORT_ID_KEYBOARD = 0,
#endif
    HID_REPORT_ID_MOUSE,
    HID_REPORT_ID_SYSTEM,
    HID_REPORT_ID_CONSUMER,
    HID_REPORT_ID_NKRO,
    HID_REPORT_ID_WEBUSB,
    HID_REPORT_ID_UNKNOWN,
};

// Vendor request id
enum {
    VENDOR_REQUEST_WEBUSB = 1,
    VENDOR_REQUEST_MICROSOFT = 2
};

uint8_t const* tud_descriptor_device_cb(void);
uint32_t tud_descriptor_device_size(void);

uint8_t const* tud_descriptor_configuration_cb(uint8_t index);
uint32_t tud_descriptor_configuration_size(uint8_t index);

#ifdef SHARED_HID_EP
uint8_t const* tud_hid_descriptor_report_cb(void);
#else
uint8_t const* tud_hid_descriptor_report_cb(uint8_t itf);
#endif

uint8_t const* tud_descriptor_hid_report_kbd_cb(void);
uint32_t tud_descriptor_hid_report_kbd_size(void);

uint8_t const* tud_descriptor_hid_report_other_cb(void);
uint32_t tud_descriptor_hid_report_other_size(void);

uint8_t const* tud_descriptor_hid_interface_kbd_cb(void);
uint32_t tud_descriptor_hid_interface_kbd_size(void);

uint8_t const* tud_descriptor_hid_interface_other_cb(void);
uint32_t tud_descriptor_hid_interface_other_size(void);

#ifdef WEBUSB_ENABLE
enum {
    WEBUSB_KEYMAP_SET = 1,
    WEBUSB_KEYMAP_GET,
};
uint8_t const* tud_descriptor_bos_cb(void);
uint32_t tud_descriptor_bos_size(void);

uint8_t const* tud_descriptor_url_cb(void);
uint32_t tud_descriptor_url_size(void);

uint8_t const* tud_descriptor_msos20_cb(void);
uint32_t tud_descriptor_msos20_size(void);
#endif

// String index
enum {
    DESC_STR_LANGID,
    DESC_STR_MANUFACTURE,
    DESC_STR_PRODUCT,
    DESC_STR_SERIAL,
    DESC_STR_CONFIGURATION,
    DESC_STR_INTERFACE_HID_KBD,
    DESC_STR_INTERFACE_HID_OTHER,
    DESC_STR_INTERFACE_WEBUSB,
};

uint16_t const* tud_descriptor_string_cb(uint8_t index, uint16_t langid);

uint8_t *get_descriptor_str(uint8_t index, uint16_t *length);