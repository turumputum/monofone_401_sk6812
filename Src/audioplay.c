/*
 * audioplay.c
 *
 *  Created on: 15 мар. 2020 г.
 *      Author: Dmitry
 */
#include "audioplay.h"
extern I2S_HandleTypeDef hi2s2;

uint8_t AudioPlay_Play(uint16_t *pBuffer, uint32_t Size)
{
	uint8_t ret=AUDIO_OK;

		if(HAL_I2S_Transmit_DMA(&hi2s2,(uint16_t*)pBuffer,
									DMA_MAX(Size/AUDIODATA_SIZE))!=HAL_OK)
		{
			ret=AUDIO_ERROR;
		}
	return ret;
}
uint8_t AudioPlay_Stop()
{
	uint8_t ret=AUDIO_OK;

		if(HAL_I2S_DMAStop(&hi2s2)!=HAL_OK)
		{
			ret=AUDIO_ERROR;
		}

	return ret;
}

void AudioPlay_ChangeBuffer(uint16_t *pData, uint16_t Size)
{
	/*for (int i=0; i<Size; i++)
	{
		volatile int16_t val=*(pData+i);
		if (val>3 || val<-3)
		{
			val=val;
		}
	}*/
	HAL_I2S_Transmit_DMA(&hi2s2,(uint16_t*)pData,Size);
}

void AudioPlay_HalfTransfer_CallBack(void)
{
	HalfTransfer_CallBack_FS();
}
//------------------------------------------------------
void AudioPlay_TransferComplete_CallBack(void)
{
	TransferComplete_CallBack_FS();
}
