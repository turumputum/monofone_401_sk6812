#include "ws2812.h"
//----------------------------------------------------------------------------
extern TIM_HandleTypeDef htim3;
//----------------------------------------------------------------------------
uint16_t BUF_DMA [ARRAY_LEN] = {0};
uint8_t rgb_temp[12][3];
uint16_t DMA_BUF_TEMP[24];
//------------------------------------------------------------------

void ws2812_init(void)
{
  int i;
  for(i=DELAY_LEN;i<LED_COUNT*24;i++){
	  BUF_DMA[i] = LOW;

  }
}
//------------------------------------------------------------------
void ws2812_pixel_rgb_to_buf_dma(uint8_t Rpixel , uint8_t Gpixel, uint8_t Bpixel, uint16_t posX)
{
  volatile uint16_t i;
  for(i=0;i<8;i++)
  {
    if (BitIsSet(Rpixel,(7-i)) == 1)
    {
      BUF_DMA[DELAY_LEN+posX*24+i+8] = HIGH;
    }else
    {
      BUF_DMA[DELAY_LEN+posX*24+i+8] = LOW;
    }
    if (BitIsSet(Gpixel,(7-i)) == 1)
    {
      BUF_DMA[DELAY_LEN+posX*24+i+0] = HIGH;
    }else
    {
      BUF_DMA[DELAY_LEN+posX*24+i+0] = LOW;
    }
    if (BitIsSet(Bpixel,(7-i)) == 1)
    {
      BUF_DMA[DELAY_LEN+posX*24+i+16] = HIGH;
    }else
    {
      BUF_DMA[DELAY_LEN+posX*24+i+16] = LOW;
    }
  }
}



//------------------------------------------------------------------
void ws2812_light(void)
{
  HAL_TIM_PWM_Start_DMA(&htim3,TIM_CHANNEL_2,(uint32_t*)&BUF_DMA,ARRAY_LEN*2);
}
//------------------------------------------------------------------
void ws2812_test01(void)
{
  uint8_t i,j;
  ws2812_prepareValue(128, 0, 0, 0, 128, 0, 0, 0, 128, 64, 64, 0,
      0, 64, 64, 64, 0, 64, 96, 32, 0, 96, 0, 32,
      32, 96, 0, 0, 96, 32, 0, 32, 96, 32, 0, 96);
  ws2812_setValue();
  for(j=0;j<50;j++)
  {
    memcpy((void*)DMA_BUF_TEMP,(void*)(BUF_DMA+48),48);
    for(i=0;i<143;i++)
    {
      memcpy((void*)(i*24+BUF_DMA+48),(void*)(i*24+BUF_DMA+72),48);
    }
    memcpy((void*)(BUF_DMA+48+3432),(void*)DMA_BUF_TEMP,48);
    ws2812_light();
    HAL_Delay(100);
  }
}
//----------------------------------------------------------------------------
void ws2812_test02(void)
{
  uint8_t i,j,jj;
  int k=0;
  for(jj=0;jj<10;jj++)
  {
    for(j=0;j<32;j++)
    {
      for(i=0;i<144;i++)
      {
        k=1000*(32-j)/32;
        ws2812_pixel_rgb_to_buf_dma(rgb_temp[i%12][0]*k/1000,rgb_temp[i%12][1]*k/1000,rgb_temp[i%12][2]*k/1000,i);
      }
      ws2812_light();
      HAL_Delay(10);
    }
    for(j=0;j<32;j++)
    {
      for(i=0;i<144;i++)
      {
        k=1000*(j+1)/32;
        ws2812_pixel_rgb_to_buf_dma(rgb_temp[i%12][0]*k/1000,rgb_temp[i%12][1]*k/1000,rgb_temp[i%12][2]*k/1000,i);
      }
      ws2812_light();
      HAL_Delay(10);
    }
  }
}
//-----------------------------------------------------------------------------
void ws2812_test03(void)
{
  uint8_t i,j,jj;
  int k=0;
  for(jj=0;jj<10;jj++)
  {
    for(j=0;j<32;j++)
    {
      for(i=0;i<144;i++)
      {
        if((i<12)||((i>=24)&&(i<36))||((i>=48)&&(i<60))||((i>=72)&&(i<84))||((i>=96)&&(i<108))||((i>=120)&&(i<132)))
        {
          k=1000*(32-j)/32;
          ws2812_pixel_rgb_to_buf_dma(rgb_temp[i%12][0]*k/1000,rgb_temp[i%12][1]*k/1000,rgb_temp[i%12][2]*k/1000,i);
        }
        else
        {
          k=1000*(j+1)/32;
          ws2812_pixel_rgb_to_buf_dma(rgb_temp[i%12][0]*k/1000,rgb_temp[i%12][1]*k/1000,rgb_temp[i%12][2]*k/1000,i);
        }
      }
      ws2812_light();
      HAL_Delay(15);
    }
    for(j=0;j<32;j++)
    {
      for(i=0;i<144;i++)
      {
        if((i<12)||((i>=24)&&(i<36))||((i>=48)&&(i<60))||((i>=72)&&(i<84))||((i>=96)&&(i<108))||((i>=120)&&(i<132)))
        {
          k=1000*(j+1)/32;
          ws2812_pixel_rgb_to_buf_dma(rgb_temp[i%12][0]*k/1000,rgb_temp[i%12][1]*k/1000,rgb_temp[i%12][2]*k/1000,i);
        }
        else
        {
          k=1000*(32-j)/32;
          ws2812_pixel_rgb_to_buf_dma(rgb_temp[i%12][0]*k/1000,rgb_temp[i%12][1]*k/1000,rgb_temp[i%12][2]*k/1000,i);
        }
      }
      ws2812_light();
      HAL_Delay(15);
    }
  }
}
//----------------------------------------------------------------------------
void ws2812_test04(uint8_t col)
{
  uint8_t i,j,jj;
  for(j=0;j<12;j++)
  {
    for(i=0;i<12;i++)
    {
      switch(col)
      {
        case 1:
          if(i<6)
            {rgb_temp[j*12+i][0]=128/((i+1)*2); rgb_temp[j*12+i][1]=0; rgb_temp[j*12+i][2]=0;}
          else
            {rgb_temp[j*12+i][0]=128/((12-i)*2); rgb_temp[j*12+i][1]=0; rgb_temp[j*12+i][2]=0;}
          break;
        case 2:
          if(i<6)
            {rgb_temp[j*12+i][0]=0; rgb_temp[j*12+i][1]=128/((i+1)*2); rgb_temp[j*12+i][2]=0;}
          else
            {rgb_temp[j*12+i][0]=0; rgb_temp[j*12+i][1]=128/((12-i)*2); rgb_temp[j*12+i][2]=0;}
          break;
        case 3:
          if(i<6)
            {rgb_temp[j*12+i][0]=0; rgb_temp[j*12+i][1]=0; rgb_temp[j*12+i][2]=128/((i+1)*2);}
          else
            {rgb_temp[j*12+i][0]=0; rgb_temp[j*12+i][1]=0; rgb_temp[j*12+i][2]=128/((12-i)*2);}
          break;
        case 4:
          if(i<6)
            {rgb_temp[j*12+i][0]=64/((i+1)*2); rgb_temp[j*12+i][1]=64/((i+1)*2); rgb_temp[j*12+i][2]=0;}
          else
            {rgb_temp[j*12+i][0]=64/((12-i)*2); rgb_temp[j*12+i][1]=64/((12-i)*2); rgb_temp[j*12+i][2]=0;}
          break;
        case 5:
          if(i<6)
            {rgb_temp[j*12+i][0]=64/((i+1)*2); rgb_temp[j*12+i][1]=0; rgb_temp[j*12+i][2]=64/((i+1)*2);}
          else
            {rgb_temp[j*12+i][0]=64/((12-i)*2); rgb_temp[j*12+i][1]=0; rgb_temp[j*12+i][2]=64/((12-i)*2);}
          break;
        case 6:
          if(i<6)
            {rgb_temp[j*12+i][0]=0; rgb_temp[j*12+i][1]=64/((i+1)*2); rgb_temp[j*12+i][2]=64/((i+1)*2);}
          else
            {rgb_temp[j*12+i][0]=0; rgb_temp[j*12+i][1]=64/((12-i)*2); rgb_temp[j*12+i][2]=64/((12-i)*2);}
          break;
      }
    }
  }
  ws2812_setValue();
  ws2812_light();
  for(jj=0;jj<144;jj++)
  {
    memcpy((void*)DMA_BUF_TEMP,(void*)(BUF_DMA+48),48);
    for(i=0;i<143;i++)
    {
      memcpy((void*)(i*24+BUF_DMA+48),(void*)(i*24+BUF_DMA+72),48);
    }
    memcpy((void*)(BUF_DMA+48+3432),(void*)DMA_BUF_TEMP,48);
    ws2812_light();
    HAL_Delay(50);
  }
}
//----------------------------------------------------------------------------
void ws2812_test05(void)
{
  uint8_t i,j,jj;
  int k=0;
  for(jj=0;jj<40;jj++)
  {
    for(j=0;j<32;j++)
    {
      for(i=0;i<144;i++)
      {
        if(j<16) k=1000*(j)/16;
        else k=1000*(32-j)/16;
        if(i%12==0) ws2812_pixel_rgb_to_buf_dma(128*k/1000,128*(1000-k)/1000,0,i);
        if(i%12==1) ws2812_pixel_rgb_to_buf_dma(0,128*k/1000,128*(1000-k)/1000,i);
        if(i%12==2) ws2812_pixel_rgb_to_buf_dma(128*k/1000,0,128*(1000-k)/1000,i);
        if(i%12==3) ws2812_pixel_rgb_to_buf_dma(128*(1000-k)/1000,128*k/2000,128*k/2000,i);
        if(i%12==4) ws2812_pixel_rgb_to_buf_dma(128*k/2000,128*k/2000,128*(1000-k)/1000,i);
        if(i%12==5) ws2812_pixel_rgb_to_buf_dma(128*k/2000,128*(1000-k)/1000,128*k/2000,i);
        if(i%12==6) ws2812_pixel_rgb_to_buf_dma(128*(1000-k)/2000,128*(1000-k)/2000,128*k/1000,i);
        if(i%12==7) ws2812_pixel_rgb_to_buf_dma(128*k/1000,128*(1000-k)/2000,128*(1000-k)/2000,i);
        if(i%12==8) ws2812_pixel_rgb_to_buf_dma(128*(1000-k)/2000,128*k/1000,128*(1000-k)/2000,i);
        if(i%12==9) ws2812_pixel_rgb_to_buf_dma(128*(1000-k)/3000,128*(1000-k)/667,128*k/1000,i);
        if(i%12==10) ws2812_pixel_rgb_to_buf_dma(128*k/3000,128*(1000-k)/667,128*(1000-k)/1000,i);
        if(i%12==11) ws2812_pixel_rgb_to_buf_dma(128*(1000-k)/3000,128*k/667,128*(1000-k)/3000,i);
      }
      ws2812_light();
      HAL_Delay(20);
    }
  }
}
//----------------------------------------------------------------------------
