/**
 * custom_action.c
 *      action function defintion for custom behavior
 */
#include "custom_action.h"
#include "rgb_effects.h"

#include "action.h"

extern void keyboard_set_rgb(bool on);

void action_function(keyrecord_t *record, uint8_t id, uint8_t opt)
{
    if (!record->event.pressed) return; // do not press key released event

    switch (id) {
        case AF_RGB_TOG:
            rgb_effects_toggle();
            keyboard_set_rgb(rgb_effects_enabled());
            break;
        case AF_RGB_MOD:
            rgb_effects_inc_mode();
            break;
        case AF_RGB_HUEI:
            rgb_effects_inc_hue();
            break;
        case AF_RGB_HUED:
            rgb_effects_dec_hue();
            break;
        case AF_RGB_SATI:
            rgb_effects_inc_sat();
            break;
        case AF_RGB_SATD:
            rgb_effects_dec_sat();
            break;
        case AF_RGB_VALI:
            rgb_effects_inc_val();
            break;
        case AF_RGB_VALD:
            rgb_effects_dec_val();
            break;
    }
}