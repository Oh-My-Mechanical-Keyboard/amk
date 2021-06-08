
SRCS += $(KEYBOARD_DIR)/hhkbble.c

MCU = NRF52832
RGB_EFFECTS_ENABLE = all
EECONFIG_FDS = yes

ifeq (yes,$(strip $(ACTIONMAP_ENABLE)))
	SRCS += $(KEYBOARD_DIR)/hhkbble_action.c
else
	SRCS += $(KEYBOARD_DIR)/hhkbble_keymap.c
endif