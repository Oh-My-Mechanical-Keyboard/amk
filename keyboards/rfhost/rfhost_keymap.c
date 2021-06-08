/**
 * rfhost_keymap.c
 *  default keymap for the rf host receiver 
 */

#include "rfhost.h"

const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    LAYOUT_default(TRNS)
};

/*
 * Fn action definition
 */
const action_t PROGMEM fn_actions[] = {
};