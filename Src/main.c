/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"
#include "usb_device.h"
#include "ws2812.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "audioplay.h"
#include "tlv493.h"
uint8_t sine_wave[256] = { 0x80, 0x83, 0x86, 0x89, 0x8C, 0x90, 0x93, 0x96, 0x99,
		0x9C, 0x9F, 0xA2, 0xA5, 0xA8, 0xAB, 0xAE, 0xB1, 0xB3, 0xB6, 0xB9, 0xBC,
		0xBF, 0xC1, 0xC4, 0xC7, 0xC9, 0xCC, 0xCE, 0xD1, 0xD3, 0xD5, 0xD8, 0xDA,
		0xDC, 0xDE, 0xE0, 0xE2, 0xE4, 0xE6, 0xE8, 0xEA, 0xEB, 0xED, 0xEF, 0xF0,
		0xF1, 0xF3, 0xF4, 0xF5, 0xF6, 0xF8, 0xF9, 0xFA, 0xFA, 0xFB, 0xFC, 0xFD,
		0xFD, 0xFE, 0xFE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFE,
		0xFE, 0xFE, 0xFD, 0xFD, 0xFC, 0xFB, 0xFA, 0xFA, 0xF9, 0xF8, 0xF6, 0xF5,
		0xF4, 0xF3, 0xF1, 0xF0, 0xEF, 0xED, 0xEB, 0xEA, 0xE8, 0xE6, 0xE4, 0xE2,
		0xE0, 0xDE, 0xDC, 0xDA, 0xD8, 0xD5, 0xD3, 0xD1, 0xCE, 0xCC, 0xC9, 0xC7,
		0xC4, 0xC1, 0xBF, 0xBC, 0xB9, 0xB6, 0xB3, 0xB1, 0xAE, 0xAB, 0xA8, 0xA5,
		0xA2, 0x9F, 0x9C, 0x99, 0x96, 0x93, 0x90, 0x8C, 0x89, 0x86, 0x83, 0x80,
		0x7D, 0x7A, 0x77, 0x74, 0x70, 0x6D, 0x6A, 0x67, 0x64, 0x61, 0x5E, 0x5B,
		0x58, 0x55, 0x52, 0x4F, 0x4D, 0x4A, 0x47, 0x44, 0x41, 0x3F, 0x3C, 0x39,
		0x37, 0x34, 0x32, 0x2F, 0x2D, 0x2B, 0x28, 0x26, 0x24, 0x22, 0x20, 0x1E,
		0x1C, 0x1A, 0x18, 0x16, 0x15, 0x13, 0x11, 0x10, 0x0F, 0x0D, 0x0C, 0x0B,
		0x0A, 0x08, 0x07, 0x06, 0x06, 0x05, 0x04, 0x03, 0x03, 0x02, 0x02, 0x02,
		0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x01, 0x02, 0x02, 0x02, 0x03, 0x03,
		0x04, 0x05, 0x06, 0x06, 0x07, 0x08, 0x0A, 0x0B, 0x0C, 0x0D, 0x0F, 0x10,
		0x11, 0x13, 0x15, 0x16, 0x18, 0x1A, 0x1C, 0x1E, 0x20, 0x22, 0x24, 0x26,
		0x28, 0x2B, 0x2D, 0x2F, 0x32, 0x34, 0x37, 0x39, 0x3C, 0x3F, 0x41, 0x44,
		0x47, 0x4A, 0x4D, 0x4F, 0x52, 0x55, 0x58, 0x5B, 0x5E, 0x61, 0x64, 0x67,
		0x6A, 0x6D, 0x70, 0x74, 0x77, 0x7A, 0x7D };

float pixelRad[LED_COUNT];


typedef struct _RGB
{
    BYTE R;
    BYTE G;
    BYTE B;
} RGB, *pRGB;

typedef struct _HSL
{
    float   H;  // color Hue (0.0 to 360.0 degrees)
    float   S;  // color Saturation (0.0 to 1.0)
    float   L;  // Luminance (0.0 to 1.0)
    float   V;  // Value (0.0 to 1.0)
} HSL, *pHSL;

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

I2S_HandleTypeDef hi2s2;
DMA_HandleTypeDef hdma_spi2_tx;

SD_HandleTypeDef hsd;

TIM_HandleTypeDef htim1;
TIM_HandleTypeDef htim3;
DMA_HandleTypeDef hdma_tim3_ch2;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2S2_Init(void);
static void MX_SDIO_SD_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
#define MAG_VAL 4
extern USBD_HandleTypeDef hUsbDeviceFS;
uint8_t HID_Buffer[8];

extern uint16_t BUF_DMA[ARRAY_LEN];

#define sizeOfFrontBuf  230
int frontBuf[sizeOfFrontBuf];
#define sizeOfeffectBuf  100
int effectBuf[sizeOfeffectBuf];
int effectTick = 0;

uint8_t val;

float beta = 0.0;

int ledBright = 0;

int lightLeds = 0;
int ledStep = 0;

int phoneUp = 0;
int phoneUp_ = 2;

float a = 0;
float b = 0;
float d = 0;

int increment = 400;
int targetBright;

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*
 * 	    0x09, 0xb5,                    //   USAGE (Scan Next Track)
 0x09, 0xb6,                    //   USAGE (Scan Previous Track)
 0x09, 0xb7,                    //   USAGE (Stop)
 0x09, 0xb0,                    //   USAGE (Play)
 0x09, 0xe2,                    //   USAGE (Mute)
 0x09, 0xe9,                    //   USAGE (Volume Up)
 0x09, 0xea,                    //   USAGE (Volume Down)
 */
static void hid_send_stop() {
	HID_Buffer[0] = 0;
	HID_Buffer[1] = 0;
	HID_Buffer[2] = 22;
	//HID_Buffer[1]=0x00;
	//HID_Buffer[2]=0;
	USBD_HID_SendReport(&hUsbDeviceFS, HID_Buffer, 8);
	HAL_Delay(20);

	HID_Buffer[0] = 0;
	HID_Buffer[1] = 0;
	HID_Buffer[2] = 0;
	//HID_Buffer[2]=0;
	USBD_HID_SendReport(&hUsbDeviceFS, HID_Buffer, 8);
}
static void hid_send_start() {
	HID_Buffer[0] = 0;
	HID_Buffer[1] = 0;
	HID_Buffer[2] = 19;
	USBD_HID_SendReport(&hUsbDeviceFS, HID_Buffer, 8);
	HAL_Delay(20);

	HID_Buffer[0] = 0;
	HID_Buffer[1] = 0;
	HID_Buffer[2] = 0;
	USBD_HID_SendReport(&hUsbDeviceFS, HID_Buffer, 8);
}


float   *fMin       (float *a, float *b)
{
    return *a <= *b?  a : b;
}

float   *fMax       (float *a, float *b)
{
    return *a >= *b? a : b;
}
void    RGBtoHSL    (pRGB rgb, pHSL hsl)
{
// See https://en.wikipedia.org/wiki/HSL_and_HSV
// rgb->R, rgb->G, rgb->B: [0 to 255]
    float r =       (float) rgb->R / 255;
    float g =       (float) rgb->G / 255;
    float b =       (float) rgb->B / 255;
    float *min =    fMin(fMin(&r, &g), &b);
    float *max =    fMax(fMax(&r, &g), &b);
    float delta =   *max - *min;

// L, V [0.0 to 1.0]
    hsl->L = (*max + *min)/2;
    hsl->V = *max;
// Special case for H and S
    if (delta == 0)
    {
        hsl->H = 0.0f;
        hsl->S = 0.0f;
    }
    else
    {
// Special case for S
        if((*max == 0) || (*min == 1))
            hsl->S = 0;
        else
// S [0.0 to 1.0]
            hsl->S = (2 * *max - 2*hsl->L)/(1 - fabsf(2*hsl->L - 1));
// H [0.0 to 360.0]
        if      (max == &r)     hsl->H = fmod((g - b)/delta, 6);    // max is R
        else if (max == &g)     hsl->H = (b - r)/delta + 2;         // max is G
        else                    hsl->H = (r - g)/delta + 4;         // max is B
        hsl->H *= 60;
    }
}

void    HSLtoRGB    (pHSL hsl, pRGB rgb)
{
// See https://en.wikipedia.org/wiki/HSL_and_HSV
    float a, k, fm1, fp1, f1, f2, *f3;
// L, V, S: [0.0 to 1.0]
// rgb->R, rgb->G, rgb->B: [0 to 255]
    fm1 = -1;
    fp1 = 1;
    f1 = 1-hsl->L;
    a = hsl->S * *fMin(&hsl->L, &f1);
    k = fmod(0 + hsl->H/30, 12);
    f1 = k - 3;
    f2 = 9 - k;
    f3 = fMin(fMin(&f1, &f2), &fp1) ;
    rgb->R = (BYTE) (255 * (hsl->L - a * *fMax(f3, &fm1)));

    k = fmod(8 + hsl->H/30, 12);
    f1 = k - 3;
    f2 = 9 - k;
    f3 = fMin(fMin(&f1, &f2), &fp1) ;
    rgb->G = (BYTE) (255 * (hsl->L - a * *fMax(f3, &fm1)));

    k = fmod(4 + hsl->H/30, 12);
    f1 = k - 3;
    f2 = 9 - k;
    f3 = fMin(fMin(&f1, &f2), &fp1) ;
    rgb->B = (BYTE) (255 * (hsl->L - a * *fMax(f3, &fm1)));
}


/* USER CODE END 0 */

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void) {
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration--------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
   	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_USB_DEVICE_Init();
	MX_I2C1_Init();
	MX_I2S2_Init();
	MX_SDIO_SD_Init();
	MX_FATFS_Init();
	MX_TIM1_Init();
	MX_TIM3_Init();
	/* USER CODE BEGIN 2 */

	HAL_TIM_Base_Start_IT(&htim1);

	//init led

	//calc led pos in radian
	for (int t = 0; t < LED_COUNT; t++) {
		pixelRad[t] = (float) ((2 * 3.14) / LED_COUNT) * t;
	}

	//calc effectFront
	for (int t = 0; t < sizeOfFrontBuf; t++) {
		if ((t > 100) && (t < 115)){
			frontBuf[t] = 255*sin((float)(t-100)/20);
		}
		if ((t >= 115) && (t <= 200)) {
			frontBuf[t] = 255;
		}
		if ((t > 200) && (t < 215)){
			frontBuf[t] = 255*cos((float)(t-200)/20);
		}
	}

	ws2812_init();

	//HAL_Delay(1000);

	/* USER CODE END 2 */




	//_____________LedTEST_______________________
	/*
	for (int u = 0; u < 4; u++) {

		for (int i = 0; i < LED_COUNT; i++) {
			val = 254;

			switch (u) {
			case 0:
				ws2812_pixel_rgb_to_buf_dma(val, 0, 0, i);
				break;
			case 1:
				ws2812_pixel_rgb_to_buf_dma(0, 0, val, i);
				break;
			case 2:
				ws2812_pixel_rgb_to_buf_dma(0, val, 0, i);
				break;
			case 3:
				ws2812_pixel_rgb_to_buf_dma(val, val, val, i);
				break;

			}
		}
		ws2812_light();
		HAL_Delay(1000);
	}
	*/


	///______________LED_EFFECT____________________


	//chenge color
	/*
	for (int u = 0; u < 6; u++) {
		//move from front buff
		for (effectTick = 0; effectTick < sizeOfFrontBuf; effectTick++) {

			//calc ledEffectBuf
			memset(effectBuf, 0, sizeOfeffectBuf);

			for (int i = 0; i < sizeOfeffectBuf; i++) {
				int pos = effectTick + i;
				if (pos < sizeOfFrontBuf) {
					effectBuf[i] = frontBuf[pos];
				}

			}

			for (int i = 0; i < LED_COUNT; i++) {
				uint8_t posInBuf = 50 + (50 * sin(beta - pixelRad[i]));
				val = effectBuf[posInBuf];

				switch (u) {
				case 0:
					ws2812_pixel_rgb_to_buf_dma(0, val, 0, i);
					break;
				case 1:
					ws2812_pixel_rgb_to_buf_dma(0, val, val, i);
					break;
				case 2:
					ws2812_pixel_rgb_to_buf_dma(0, 0, val, i);
					break;
				case 3:
					ws2812_pixel_rgb_to_buf_dma(val, 0, val, i);
					break;
				case 4:
					ws2812_pixel_rgb_to_buf_dma(val, 0, 0, i);
					break;
				case 5:
					ws2812_pixel_rgb_to_buf_dma(val, val, 0, i);
					break;
				}
			}

			ws2812_light();
			HAL_Delay(2);
		}
		//if (beta < 6.28) {
			beta += 1.8;
		//} else {
			//beta = 0;
		//}
	}
	*/
	pHSL G_HSL;

	G_HSL->H=0;
	G_HSL->S=1;
	G_HSL->L=1;
	G_HSL->V=1;


	pRGB RGB;

	while(effectTick<10000){
		if(G_HSL->H<360){
			G_HSL->H++;
		}else{
			G_HSL->H=0;
		}

		HSLtoRGB  (G_HSL, RGB);

		for (int i = 0; i < LED_COUNT; i++){
			ws2812_pixel_rgb_to_buf_dma(RGB->R, RGB->G, RGB->B, i);
		}
		ws2812_light();
		HAL_Delay(60);


	}


	//___________Magnet Sensor INIT____________________
	if (tlv_init() == -1) {
		for (int i = 0; i < LED_COUNT; i++) {
			ws2812_pixel_rgb_to_buf_dma(100, 0, 0, i);

		}
		ws2812_light();
		while (1) {
		};
	}

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	while (1) {

		//a=getMagVal();

		a = 0.992 * a + 0.008 * getMagVal();
		b = 0.95 * b + 0.05 * getMagVal();
		d = a - b;
		if (d > MAG_VAL) {
			phoneUp = 1;
		} else if (d < -MAG_VAL) {
			phoneUp = 0;
		}

		if (phoneUp != phoneUp_) {
			if (phoneUp) {
				hid_send_start();

			} else {
				hid_send_stop();

			}
		}
		phoneUp_ = phoneUp;

		if ((lightLeds == 1) && (phoneUp)) {

			//cleanWSBuff
			for (int i = 0; i < LED_COUNT; i++) {
				ws2812_pixel_rgb_to_buf_dma(0, 0, 0, i);

			}

			//calc ledEffectBuf
			memset(effectBuf, 0, sizeOfeffectBuf);

			for (int i = 0; i < sizeOfeffectBuf; i++) {
				int pos = effectTick + i;
				//int revPos = sizeOfFrontBuf-(i+1);
				if (pos < sizeOfFrontBuf) {
					effectBuf[i] = frontBuf[pos];
				}

			}

			for (int i = 0; i < LED_COUNT; i++) {
				uint8_t posInBuf = 50 + (50 * sin(beta - pixelRad[i]));

				uint8_t val = effectBuf[posInBuf];

				ws2812_pixel_rgb_to_buf_dma(0, val, 0, i);

			}

			if (effectTick < 300) {
				effectTick++;
			} else {
				effectTick = 0;
			}

			if (beta < 6.28) {
				beta += 0.2;
			} else {
				beta = 0;
			}

			//lightLeds = 0;

		}

		//printf("d: %d \n",(int)(d*1000));
		//HAL_Delay(1);
	}
	/* USER CODE END 3 */
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void) {
	RCC_OscInitTypeDef RCC_OscInitStruct = { 0 };
	RCC_ClkInitTypeDef RCC_ClkInitStruct = { 0 };
	RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = { 0 };

	/** Configure the main internal regulator output voltage
	 */
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
	RCC_OscInitStruct.HSEState = RCC_HSE_ON;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
	RCC_OscInitStruct.PLL.PLLM = 8;
	RCC_OscInitStruct.PLL.PLLN = 250;
	RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV6;
	RCC_OscInitStruct.PLL.PLLQ = 8;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
		Error_Handler();
	}
	/** Initializes the CPU, AHB and APB busses clocks
	 */
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK
			| RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK) {
		Error_Handler();
	}
	PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_I2S;
	PeriphClkInitStruct.PLLI2S.PLLI2SN = 192;
	PeriphClkInitStruct.PLLI2S.PLLI2SR = 2;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK) {
		Error_Handler();
	}
}

/**
 * @brief I2C1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2C1_Init(void) {

	/* USER CODE BEGIN I2C1_Init 0 */

	/* USER CODE END I2C1_Init 0 */

	/* USER CODE BEGIN I2C1_Init 1 */

	/* USER CODE END I2C1_Init 1 */
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2C1_Init 2 */

	/* USER CODE END I2C1_Init 2 */

}

/**
 * @brief I2S2 Initialization Function
 * @param None
 * @retval None
 */
static void MX_I2S2_Init(void) {

	/* USER CODE BEGIN I2S2_Init 0 */

	/* USER CODE END I2S2_Init 0 */

	/* USER CODE BEGIN I2S2_Init 1 */

	/* USER CODE END I2S2_Init 1 */
	hi2s2.Instance = SPI2;
	hi2s2.Init.Mode = I2S_MODE_MASTER_TX;
	hi2s2.Init.Standard = I2S_STANDARD_PHILIPS;
	hi2s2.Init.DataFormat = I2S_DATAFORMAT_16B;
	hi2s2.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
	hi2s2.Init.AudioFreq = I2S_AUDIOFREQ_48K;
	hi2s2.Init.CPOL = I2S_CPOL_LOW;
	hi2s2.Init.ClockSource = I2S_CLOCK_PLL;
	hi2s2.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
	if (HAL_I2S_Init(&hi2s2) != HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN I2S2_Init 2 */

	/* USER CODE END I2S2_Init 2 */

}

/**
 * @brief SDIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_SDIO_SD_Init(void) {

	/* USER CODE BEGIN SDIO_Init 0 */

	/* USER CODE END SDIO_Init 0 */

	/* USER CODE BEGIN SDIO_Init 1 */

	/* USER CODE END SDIO_Init 1 */
	hsd.Instance = SDIO;
	hsd.Init.ClockEdge = SDIO_CLOCK_EDGE_RISING;
	hsd.Init.ClockBypass = SDIO_CLOCK_BYPASS_DISABLE;
	hsd.Init.ClockPowerSave = SDIO_CLOCK_POWER_SAVE_DISABLE;
	hsd.Init.BusWide = SDIO_BUS_WIDE_1B;
	hsd.Init.HardwareFlowControl = SDIO_HARDWARE_FLOW_CONTROL_DISABLE;
	hsd.Init.ClockDiv = 0;
	/* USER CODE BEGIN SDIO_Init 2 */

	/* USER CODE END SDIO_Init 2 */

}

/**
 * @brief TIM1 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM1_Init(void) {

	/* USER CODE BEGIN TIM1_Init 0 */

	/* USER CODE END TIM1_Init 0 */

	TIM_ClockConfigTypeDef sClockSourceConfig = { 0 };
	TIM_MasterConfigTypeDef sMasterConfig = { 0 };

	/* USER CODE BEGIN TIM1_Init 1 */

	/* USER CODE END TIM1_Init 1 */
	htim1.Instance = TIM1;
	htim1.Init.Prescaler = 1000;
	htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim1.Init.Period = 2000;
	htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim1.Init.RepetitionCounter = 0;
	htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_Base_Init(&htim1) != HAL_OK) {
		Error_Handler();
	}
	sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;

	if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}

	/* USER CODE BEGIN TIM1_Init 2 */

	/* USER CODE END TIM1_Init 2 */

}

/**
 * @brief TIM3 Initialization Function
 * @param None
 * @retval None
 */
static void MX_TIM3_Init(void) {

	/* USER CODE BEGIN TIM3_Init 0 */

	/* USER CODE END TIM3_Init 0 */

	TIM_MasterConfigTypeDef sMasterConfig = { 0 };
	TIM_OC_InitTypeDef sConfigOC = { 0 };

	/* USER CODE BEGIN TIM3_Init 1 */

	/* USER CODE END TIM3_Init 1 */
	htim3.Instance = TIM3;
	htim3.Init.Prescaler = 0;
	htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
	htim3.Init.Period = 80;
	htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
	htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
	if (HAL_TIM_PWM_Init(&htim3) != HAL_OK) {
		Error_Handler();
	}
	sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
	sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
	if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig)
			!= HAL_OK) {
		Error_Handler();
	}
	sConfigOC.OCMode = TIM_OCMODE_PWM1;
	sConfigOC.Pulse = 0;
	sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
	sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
	if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2)
			!= HAL_OK) {
		Error_Handler();
	}
	/* USER CODE BEGIN TIM3_Init 2 */

	/* USER CODE END TIM3_Init 2 */
	HAL_TIM_MspPostInit(&htim3);

}

/** 
 * Enable DMA controller clock
 */
static void MX_DMA_Init(void) {

	/* DMA controller clock enable */
	__HAL_RCC_DMA1_CLK_ENABLE();

	/* DMA interrupt init */
	/* DMA1_Stream4_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream4_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream4_IRQn);
	/* DMA1_Stream5_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);

}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void) {
	GPIO_InitTypeDef GPIO_InitStruct = { 0 };

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOH_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	/*Configure GPIO pin : CardDet_Pin */
	GPIO_InitStruct.Pin = CardDet_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	HAL_GPIO_Init(CardDet_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
void HAL_I2S_TxCpltCallback(I2S_HandleTypeDef *hi2s2) {
	if (hi2s2->Instance == I2S2) {
		AudioPlay_TransferComplete_CallBack();
	}
}
//---------------------------------------------------------
void HAL_I2S_TxHalfCpltCallback(I2S_HandleTypeDef *hi2s2) {
	if (hi2s2->Instance == I2S2) {
		AudioPlay_HalfTransfer_CallBack();
	}
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void) {
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */

	/* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
