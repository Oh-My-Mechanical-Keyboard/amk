
HHKBBLE_DIR := $(TOP_DIR)/keyboards/hhkbble

SRC_FILES += \
	$(HHKBBLE_DIR)/hhkbble.c \
	$(HHKBBLE_DIR)/hhkbble_keymap.c \

INC_FOLDERS += \
	$(HHKBBLE_DIR) \