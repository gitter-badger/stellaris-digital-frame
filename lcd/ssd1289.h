#ifndef _SSD1289_H_
#define _SSD1289_H_

#include <stdint.h>

//Pins and Ports:
#define LCD_DATAL_PERIPH        SYSCTL_PERIPH_GPIOD
#define LCD_DATAL_BASE          GPIO_PORTD_BASE
#define LCD_DATAH_PERIPH        SYSCTL_PERIPH_GPIOE
#define LCD_DATAH_BASE          GPIO_PORTE_BASE

#define LCD_COMMAND_PERIPH      SYSCTL_PERIPH_GPIOJ
#define LCD_COMMAND_BASE		GPIO_PORTJ_BASE

#define LCD_RST_PERIPH          SYSCTL_PERIPH_GPIOF //E5
#define LCD_RST_BASE            GPIO_PORTF_BASE
#define LCD_RST_PIN             GPIO_PIN_0

#define LCD_RS_BASE             GPIO_PORTJ_BASE			//e3
#define LCD_RS_PIN              GPIO_PIN_1
#define LCD_RD_BASE             GPIO_PORTJ_BASE		//E2 Pullup
#define LCD_RD_PIN              GPIO_PIN_2
#define LCD_WR_BASE             GPIO_PORTJ_BASE		//e1
#define LCD_WR_PIN              GPIO_PIN_3
#define LCD_CS_PERIPH           SYSCTL_PERIPH_GPIOJ		//d6
#define LCD_CS_BASE             GPIO_PORTJ_BASE
#define LCD_CS_PIN              GPIO_PIN_0


/* function will use (y,x) format */
typedef uint16_t yaxis_t; /* y axis */
typedef uint16_t xaxis_t; /* x axis */
typedef uint16_t radius_t; /* radius */
typedef uint16_t color_t;

#define SSD1289_WIDTH 320
#define SSD1289_HEIGHT 240

/* https://github.com/notro/fbtft */
#define SSD1289_REG_OSCILLATION      0x00
#define SSD1289_REG_DRIVER_OUT_CTRL  0x01
#define SSD1289_REG_LCD_DRIVE_AC     0x02
#define SSD1289_REG_POWER_CTRL_1     0x03
#define SSD1289_REG_DISPLAY_CTRL     0x07
#define SSD1289_REG_FRAME_CYCLE      0x0b
#define SSD1289_REG_POWER_CTRL_2     0x0c
#define SSD1289_REG_POWER_CTRL_3     0x0d
#define SSD1289_REG_POWER_CTRL_4     0x0e
#define SSD1289_REG_GATE_SCAN_START  0x0f
#define SSD1289_REG_SLEEP_MODE       0x10
#define SSD1289_REG_ENTRY_MODE       0x11
#define SSD1289_REG_POWER_CTRL_5     0x1e
#define SSD1289_REG_GDDRAM_DATA      0x22
#define SSD1289_REG_WR_DATA_MASK_1   0x23
#define SSD1289_REG_WR_DATA_MASK_2   0x24
#define SSD1289_REG_FRAME_FREQUENCY  0x25
#define SSD1289_REG_GAMMA_CTRL_1     0x30
#define SSD1289_REG_GAMME_CTRL_2     0x31
#define SSD1289_REG_GAMMA_CTRL_3     0x32
#define SSD1289_REG_GAMMA_CTRL_4     0x33
#define SSD1289_REG_GAMMA_CTRL_5     0x34
#define SSD1289_REG_GAMMA_CTRL_6     0x35
#define SSD1289_REG_GAMMA_CTRL_7     0x36
#define SSD1289_REG_GAMMA_CTRL_8     0x37
#define SSD1289_REG_GAMMA_CTRL_9     0x3a
#define SSD1289_REG_GAMMA_CTRL_10    0x3b
#define SSD1289_REG_V_SCROLL_CTRL_1  0x41
#define SSD1289_REG_V_SCROLL_CTRL_2  0x42
#define SSD1289_REG_H_RAM_ADR_POS    0x44
#define SSD1289_REG_V_RAM_ADR_START  0x45
#define SSD1289_REG_V_RAM_ADR_END    0x46
#define SSD1289_REG_FIRST_WIN_START  0x48
#define SSD1289_REG_FIRST_WIN_END    0x49
#define SSD1289_REG_SECND_WIN_START  0x4a
#define SSD1289_REG_SECND_WIN_END    0x4b
#define SSD1289_REG_GDDRAM_X_ADDR    0x4e
#define SSD1289_REG_GDDRAM_Y_ADDR    0x4f

#define ssd1289_rgb(r,g,b) ((color_t)(((r & 0x1F) << 11) | ((g & 0x3F) << 5) | (b & 0x1F)))

void ssd1289_char(color_t color, char ch, yaxis_t y, xaxis_t x);
void ssd1289_string(color_t color, char *string, yaxis_t y, xaxis_t x);
void ssd1289_line(color_t color, yaxis_t start_y, xaxis_t start_x, yaxis_t end_y, xaxis_t end_x);
void ssd1289_vline(color_t color, yaxis_t y, xaxis_t x, yaxis_t height);
void ssd1289_hline(color_t color, yaxis_t y, xaxis_t x, xaxis_t width);
void ssd1289_rectangle(color_t color, yaxis_t y, xaxis_t x, yaxis_t height, xaxis_t width);
void ssd1289_box(color_t color, yaxis_t y, xaxis_t x, yaxis_t height, xaxis_t width);
void ssd1289_rainbow();
void ssd1289_pixel(color_t color, yaxis_t y, xaxis_t x);
void ssd1289_circle(color_t color, yaxis_t y, xaxis_t x, radius_t radius);
void ssd1289_fill(color_t color);
void ssd1289_send_data(uint16_t raw);
void ssd1289_send_cmd(uint16_t raw);
void ssd1289_send(uint16_t cmd, uint16_t data);
void ssd1289_init();
void ssd1289_set_cursor(yaxis_t y, xaxis_t x);


#endif
