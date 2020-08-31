#ifndef WS2812_H_
#define WS2812_H_
//--------------------------------------------------
#include "stm32f4xx_hal.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
//--------------------------------------------------
#define DELAY_LEN 80
#define LED_COUNT 24
#define ARRAY_LEN 2*DELAY_LEN + LED_COUNT*24
#define HIGH 65
#define LOW 25
//--------------------------------------------------
#define BitIsSet(reg, bit) ((reg & (1<<bit)) != 0)
//--------------------------------------------------
void ws2812_init(void);
void ws2812_pixel_rgb_to_buf_dma(uint8_t Rpixel , uint8_t Gpixel, uint8_t Bpixel, uint16_t posX);
void ws2812_setValue(void);
void ws2812_light(void);
void ws2812_clearBuf();
void ws2812_setPixel_gammaCorrection(uint8_t Rpixel , uint8_t Gpixel, uint8_t Bpixel, uint16_t pos);

//--------------------------------------------------
#endif /* WS2812_H_ */
