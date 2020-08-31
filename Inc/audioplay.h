/*
 * audioplay.h
 *
 *  Created on: 15 мар. 2020 г.
 *      Author: Dmitry
 */

#ifndef AUDIOPLAY_H_
#define AUDIOPLAY_H_

#include "stm32f4xx_hal.h"
#include "usbd_audio_if.h"
//------------------------------------------------
#define I2S2                            SPI2
#define AUDIO_OK                        0
#define AUDIO_ERROR                     1
#define AUDIO_TIMEOUT                   2
#define AUDIODATA_SIZE   2 /* 16-bits audio data size */
#define DMA_MAX_SZE   0xFFFF
#define 	DMA_MAX(_X_)   (((_X_) <= DMA_MAX_SZE)? (_X_):DMA_MAX_SZE)
//------------------------------------------------
uint8_t AudioOut_Init(uint16_t OutputDevice, uint8_t Volume, uint32_t AudioFreq);
void AudioPlay_Init(uint32_t AudioFreq);
void AudioPlay_HalfTransfer_CallBack(void);
void AudioPlay_TransferComplete_CallBack(void);
uint8_t AudioPlay_Stop();
uint8_t AudioPlay_Play(uint16_t *pBuffer, uint32_t Size);
void AudioPlay_ChangeBuffer(uint16_t *pData, uint16_t Size);
uint8_t AudioPlay_SetVolume(uint8_t Volume);
uint8_t AudioPlay_SetMute(uint32_t Cmd);


#endif /* AUDIOPLAY_H_ */
