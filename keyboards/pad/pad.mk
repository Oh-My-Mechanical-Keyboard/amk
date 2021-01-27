
SRCS += $(wildcard $(KEYBOARD_DIR)/*.c)

MCU = STM32F405 
USB_HOST_ENABLE = yes
RGB_MATRIX_ENABLE = yes
RGB_EFFECTS_ENABLE = ws2812

LINKER_PATH = $(KEYBOARD_DIR)