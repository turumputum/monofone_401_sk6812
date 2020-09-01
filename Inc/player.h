#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
//#include "sine.h"
#include "stdio.h"
#include "string.h"
#include "stm32f4xx_hal.h"

#define    DWT_CYCCNT    *(volatile unsigned long *)0xE0001004
#define    DWT_CONTROL   *(volatile unsigned long *)0xE0001000
#define    SCB_DEMCR     *(volatile unsigned long *)0xE000EDFC

#define false 0
#define true 1


void init();
void end();
int playWavFile(const char *fname);
void player();
void writeErrorTxt(char *buff, int len);
