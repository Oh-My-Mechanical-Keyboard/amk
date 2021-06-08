/**
 * hhkbble_keymap.c
 *  default keymap for the hhkbble
 */

#include "hhkbble.h"
#include "keymap.h"

#define _______ KC_TRNS

const uint8_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0]=LAYOUT_default(
        KC_GESC,    KC_1,     KC_2, KC_3, KC_4, KC_5, KC_6, KC_7,    KC_8,   KC_9,    KC_0, KC_MINS,  KC_EQL, KC_BSLS, KC_GRV,
        KC_TAB,     KC_Q,     KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U,    KC_I,   KC_O,    KC_P, KC_LBRC, KC_RBRC, KC_BSPC,
        KC_FN4,    KC_A,     KC_S, KC_D, KC_F, KC_G, KC_H, KC_J,    KC_K,   KC_L, KC_SCLN, KC_QUOT,           KC_ENT,
        KC_LSFT,    KC_Z,     KC_X, KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH,          KC_RSFT,  KC_FN0,
                 KC_LGUI,  KC_LALT,                 KC_SPC,                                          KC_RALT,  KC_RCTL),
    [1]=LAYOUT_default(
        _______,   KC_F1,   KC_F3,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12, _______, KC_PSCR,
        KC_BTLD,  KC_FN2,  KC_FN3,  KC_FN5, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
        KC_CAPS, KC_VOLU, KC_VOLD, KC_CALC, _______, _______, KC_LEFT, KC_DOWN,   KC_UP,KC_RIGHT, _______, _______,          _______,
        _______,  KC_F21,  KC_F22,  KC_F23,  KC_F24, _______, _______, _______, _______, _______, _______,          _______, _______,
                 _______, _______,                   _______,                                                       _______,_______),
};

/*
 * Fn action definition
 */
const action_t PROGMEM fn_actions[] = {
    [0] = ACTION_LAYER_MOMENTARY(1),
    [1] = ACTION_LAYER_TAP_KEY(1, KC_SPC),
    [2] = ACTION_FUNCTION(AF_RGB_TOG),
    [3] = ACTION_FUNCTION(AF_RGB_MOD),
    [4] = ACTION_MODS_TAP_KEY(MOD_LCTL, KC_CAPS),
    [5] = ACTION_FUNCTION(AF_EEPROM_RESET),
};

const uint32_t keymaps_size = sizeof(keymaps);