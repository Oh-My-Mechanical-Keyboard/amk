/**
 * @file st7735.c
 */

#include "st7735.h"
#include "spi.h"
#include "wait.h"

#define ST7735_IS_160X80 1
// normal directory
#define ST7735_XSTART 26
#define ST7735_YSTART 1
#define ST7735_WIDTH  80
#define ST7735_HEIGHT 160 
#define ST7735_ROTATION (ST7735_MADCTL_MX | ST7735_MADCTL_MY | ST7735_MADCTL_BGR)

#if 0
// rotate left
#define ST7735_XSTART       1
#define ST7735_YSTART       26
#define ST7735_WIDTH        160
#define ST7735_HEIGHT       80
#define ST7735_ROTATION (ST7735_MADCTL_MX | ST7735_MADCTL_MV | ST7735_MADCTL_BGR)

// rotate right
#define ST7735_XSTART 1
#define ST7735_YSTART 26
#define ST7735_WIDTH  160
#define ST7735_HEIGHT 80
#define ST7735_ROTATION (ST7735_MADCTL_MY | ST7735_MADCTL_MV | ST7735_MADCTL_BGR)
#endif

#define DELAY               0x80

#define ST7735_MADCTL_MY    0x80
#define ST7735_MADCTL_MX    0x40
#define ST7735_MADCTL_MV    0x20
#define ST7735_MADCTL_ML    0x10
#define ST7735_MADCTL_RGB   0x00
#define ST7735_MADCTL_BGR   0x08
#define ST7735_MADCTL_MH    0x04

// commands
#define ST7735_NOP          0x00
#define ST7735_SWRESET      0x01
#define ST7735_RDDID        0x04
#define ST7735_RDDST        0x09

#define ST7735_SLPIN        0x10
#define ST7735_SLPOUT       0x11
#define ST7735_PTLON        0x12
#define ST7735_NORON        0x13

#define ST7735_INVOFF       0x20
#define ST7735_INVON        0x21
#define ST7735_DISPOFF      0x28
#define ST7735_DISPON       0x29
#define ST7735_CASET        0x2A
#define ST7735_RASET        0x2B
#define ST7735_RAMWR        0x2C
#define ST7735_RAMRD        0x2E

#define ST7735_PTLAR        0x30
#define ST7735_COLMOD       0x3A
#define ST7735_MADCTL       0x36

#define ST7735_FRMCTR1      0xB1
#define ST7735_FRMCTR2      0xB2
#define ST7735_FRMCTR3      0xB3
#define ST7735_INVCTR       0xB4
#define ST7735_DISSET5      0xB6

#define ST7735_PWCTR1       0xC0
#define ST7735_PWCTR2       0xC1
#define ST7735_PWCTR3       0xC2
#define ST7735_PWCTR4       0xC3
#define ST7735_PWCTR5       0xC4
#define ST7735_VMCTR1       0xC5

#define ST7735_RDID1        0xDA
#define ST7735_RDID2        0xDB
#define ST7735_RDID3        0xDC
#define ST7735_RDID4        0xDD

#define ST7735_PWCTR6       0xFC

#define ST7735_GMCTRP1      0xE0
#define ST7735_GMCTRN1      0xE1

// based on Adafruit ST7735 library for Arduino
static const uint8_t
  init_cmds1[] = {            // Init for 7735R, part 1 (red or green tab)
    15,                       // 15 commands in list:
    ST7735_SWRESET,   DELAY,  //  1: Software reset, 0 args, w/delay
      150,                    //     150 ms delay
    ST7735_SLPOUT ,   DELAY,  //  2: Out of sleep mode, 0 args, w/delay
      255,                    //     500 ms delay
    ST7735_FRMCTR1, 3      ,  //  3: Frame rate ctrl - normal mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR2, 3      ,  //  4: Frame rate control - idle mode, 3 args:
      0x01, 0x2C, 0x2D,       //     Rate = fosc/(1x2+40) * (LINE+2C+2D)
    ST7735_FRMCTR3, 6      ,  //  5: Frame rate ctrl - partial mode, 6 args:
      0x01, 0x2C, 0x2D,       //     Dot inversion mode
      0x01, 0x2C, 0x2D,       //     Line inversion mode
    ST7735_INVCTR , 1      ,  //  6: Display inversion ctrl, 1 arg, no delay:
      0x07,                   //     No inversion
    ST7735_PWCTR1 , 3      ,  //  7: Power control, 3 args, no delay:
      0xA2,
      0x02,                   //     -4.6V
      0x84,                   //     AUTO mode
    ST7735_PWCTR2 , 1      ,  //  8: Power control, 1 arg, no delay:
      0xC5,                   //     VGH25 = 2.4C VGSEL = -10 VGH = 3 * AVDD
    ST7735_PWCTR3 , 2      ,  //  9: Power control, 2 args, no delay:
      0x0A,                   //     Opamp current small
      0x00,                   //     Boost frequency
    ST7735_PWCTR4 , 2      ,  // 10: Power control, 2 args, no delay:
      0x8A,                   //     BCLK/2, Opamp current small & Medium low
      0x2A,  
    ST7735_PWCTR5 , 2      ,  // 11: Power control, 2 args, no delay:
      0x8A, 0xEE,
    ST7735_VMCTR1 , 1      ,  // 12: Power control, 1 arg, no delay:
      0x0E,
    ST7735_INVOFF , 0      ,  // 13: Don't invert display, no args, no delay
    ST7735_MADCTL , 1      ,  // 14: Memory access control (directions), 1 arg:
      ST7735_ROTATION,        //     row addr/col addr, bottom to top refresh
    ST7735_COLMOD , 1      ,  // 15: set color mode, 1 arg, no delay:
      0x05 },                 //     16-bit color

#if (defined(ST7735_IS_128X128) || defined(ST7735_IS_160X128))
  init_cmds2[] = {            // Init for 7735R, part 2 (1.44" display)
    2,                        //  2 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F,             //     XEND = 127
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x7F },           //     XEND = 127
#endif // ST7735_IS_128X128

#ifdef ST7735_IS_160X80
  init_cmds2[] = {            // Init for 7735S, part 2 (160x80 display)
    3,                        //  3 commands in list:
    ST7735_CASET  , 4      ,  //  1: Column addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x4F,             //     XEND = 79
    ST7735_RASET  , 4      ,  //  2: Row addr set, 4 args, no delay:
      0x00, 0x00,             //     XSTART = 0
      0x00, 0x9F ,            //     XEND = 159
    ST7735_INVON, 0 },        //  3: Invert colors
#endif

  init_cmds3[] = {            // Init for 7735R, part 3 (red or green tab)
    4,                        //  4 commands in list:
    ST7735_GMCTRP1, 16      , //  1: Magical unicorn dust, 16 args, no delay:
      0x02, 0x1c, 0x07, 0x12,
      0x37, 0x32, 0x29, 0x2d,
      0x29, 0x25, 0x2B, 0x39,
      0x00, 0x01, 0x03, 0x10,
    ST7735_GMCTRN1, 16      , //  2: Sparkles and rainbows, 16 args, no delay:
      0x03, 0x1d, 0x07, 0x06,
      0x2E, 0x2C, 0x29, 0x2D,
      0x2E, 0x2E, 0x37, 0x3F,
      0x00, 0x00, 0x02, 0x10,
    ST7735_NORON  ,    DELAY, //  3: Normal display on, no args, w/delay
      10,                     //     10 ms delay
    ST7735_DISPON ,    DELAY, //  4: Main screen turn on, no args w/delay
      100 };                  //     100 ms delay

static spi_handle_t spi;

static void st7735_select(st7735_t *driver)
{
    gpio_write_pin(driver->cs, 0);
}

void st7735_unselect(st7735_t *driver)
{
    gpio_write_pin(driver->cs, 1);
}

static void st7735_reset(st7735_t *driver)
{
    gpio_write_pin(driver->reset, 0);
    wait_ms(5);
    gpio_write_pin(driver->reset, 1);
}

static void write_command(st7735_t *driver, uint8_t cmd)
{
    gpio_write_pin(driver->dc, 0);
    spi_send(spi, &cmd, sizeof(cmd));
}

static void write_data(st7735_t *driver, const uint8_t* buff, size_t buff_size)
{
    gpio_write_pin(driver->dc, 1);
    spi_send(spi, buff, buff_size);
}

static void write_data_async(st7735_t *driver, const uint8_t* buff, size_t buff_size)
{
    gpio_write_pin(driver->dc, 1);
    spi_send_async(spi, buff, buff_size);
}

static void execute_commands(st7735_t *driver, const uint8_t *addr)
{
    uint8_t commands, args;
    uint16_t ms;

    commands = *addr++;
    while(commands--) {
        uint8_t cmd = *addr++;
        write_command(driver, cmd);

        args = *addr++;
        // If high bit set, delay follows args
        ms = args & DELAY;
        args &= ~DELAY;
        if(args) {
            write_data(driver, (uint8_t*)addr, args);
            addr += args;
        }

        if(ms) {
            ms = *addr++;
            if(ms == 255) ms = 500;
            wait_ms(ms);
        }
    }
}

static void set_address_window(st7735_t *driver, uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1)
{
    // column address set
    write_command(driver, ST7735_CASET);
    uint8_t data[] = { 0x00, x0 + ST7735_XSTART, 0x00, x1 + ST7735_XSTART };
    write_data(driver, data, sizeof(data));

    // row address set
    write_command(driver, ST7735_RASET);
    data[1] = y0 + ST7735_YSTART;
    data[3] = y1 + ST7735_YSTART;
    write_data(driver, data, sizeof(data));

    // write to RAM
    write_command(driver, ST7735_RAMWR);
}

#ifndef SSD1357_SPI_ID
#define SSD1357_SPI_ID  SPI_INSTANCE_1
#endif

void st7735_init(st7735_t *driver)
{
    spi = spi_init(SSD1357_SPI_ID);
    st7735_select(driver);
    st7735_reset(driver);
    execute_commands(driver, init_cmds1);
    execute_commands(driver, init_cmds2);
    execute_commands(driver, init_cmds3);
    set_address_window(driver, 0, 0, ST7735_WIDTH-1, ST7735_HEIGHT-1);
    uint16_t color = 0;
    for (int x = 0; x < ST7735_WIDTH; x++) {
        for (int y = 0; y < ST7735_HEIGHT; y++) {
            write_data(driver, (uint8_t*)&color, sizeof(color));
        }
    }
    st7735_unselect(driver);
}

void st7735_fill_rect(st7735_t *driver, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const void *data, size_t size)
{
    st7735_select(driver);
    set_address_window(driver, x, y, x+w-1, y+h-1);
    write_data(driver, data, size);
    st7735_unselect(driver);
}

void st7735_fill_rect_async(st7735_t *driver, uint32_t x, uint32_t y, uint32_t w, uint32_t h, const void *data, size_t size)
{
    st7735_select(driver);
    set_address_window(driver, x, y, x+w-1, y+h-1);
    write_data_async(driver, data, size);
    //st7735_unselect(driver);
}

bool st7735_fill_ready(st7735_t *driver)
{
    return spi_ready(spi);
}

void st7735_release(st7735_t *driver)
{
    st7735_unselect(driver);
}

void st7735_fill(st7735_t *driver, const void* data)
{
    st7735_fill_rect(driver, 0, 0, ST7735_WIDTH, ST7735_HEIGHT, data, ST7735_WIDTH*ST7735_HEIGHT*2);
}

void st7735_uninit(st7735_t *driver)
{}