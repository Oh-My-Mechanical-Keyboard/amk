/**
 * usb_interface.h
 */

#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "usb_descriptors.h"

void usb_init(void);
void usb_task(void);
bool usb_ready(void);
bool usb_suspended(void);
void usb_remote_wakeup(void);
void usb_send_report(uint8_t report_type, const void* data, size_t size);