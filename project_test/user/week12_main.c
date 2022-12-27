#include "stm32f10x.h"
#include "core_cm3.h"
#include "misc.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_usart.h"
#include "stm32f10x_adc.h"
#include "lcd.h"
#include "touch.h"
int color[12] = {WHITE,CYAN,BLUE,RED,MAGENTA,LGRAY,GREEN,YELLOW,BROWN,BRRED,GRAY}; 

uint16_t value = 0;
volatile uint32_t ADC_Value[1] ;

uint32_t colorFlag = WHITE;

void RCC_Configure(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
        RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
}

void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void ADC_Configure(){
    ADC_InitTypeDef ADC_InitStructure;
  
    ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
    ADC_InitStructure.ADC_NbrOfChannel = 1;
    
    ADC_Init(ADC1, &ADC_InitStructure);
    
    ADC_RegularChannelConfig(ADC1, ADC_Channel_2, 1, ADC_SampleTime_55Cycles5);
    
    ADC_DMACmd(ADC1, ENABLE);
    
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1)){ }
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1)){  }
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


void DMA_Configure(void){
  DMA_InitTypeDef DMA_Instructure;
  DMA_DeInit(DMA1_Channel1);
  DMA_Instructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
  DMA_Instructure.DMA_MemoryBaseAddr=(uint32_t)&ADC_Value[0];
  
  DMA_Instructure.DMA_Mode=DMA_Mode_Circular;
  DMA_Instructure.DMA_Priority= DMA_Priority_VeryHigh;
  DMA_Instructure.DMA_M2M= DMA_M2M_Disable;
  DMA_Instructure.DMA_BufferSize=2;
  DMA_Instructure.DMA_DIR= DMA_DIR_PeripheralSRC;
  DMA_Instructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
  DMA_Instructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Byte;
  DMA_Instructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Byte;
  DMA_Instructure.DMA_PeripheralInc=DMA_PeripheralInc_Enable;
  DMA_Init(DMA1_Channel1, &DMA_Instructure);
  DMA_Cmd(DMA1_Channel1, ENABLE);
  
  
}
  
int main() {
  
// LCD 관련 설정은 LCD_Init에 구현되어 있으므로 여기서 할 필요 없음
  SystemInit();
  RCC_Configure();
  GPIO_Configure();
  ADC_Configure();
  DMA_Configure();
  
  // ------------------------------------
  LCD_Init();
  Touch_Configuration();
  Touch_Adjust();
  LCD_Clear(WHITE);
  
  while(1){
  // TODO : LCD 값 출력 및 터치 좌표 읽기
    
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    if(ADC_Value[0] > 1500){
      if(colorFlag != WHITE){
        LCD_Clear(WHITE);
      }
      LCD_ShowString(40, 40, "Team07", BLACK, WHITE);
      LCD_ShowNum(40, 100, ADC_Value[0], 4, BLACK, WHITE);
      colorFlag = WHITE;
    }
    else{
      if(colorFlag != GRAY){
        LCD_Clear(GRAY);
      }
      LCD_ShowString(40, 40, "Team07", WHITE, GRAY);
      LCD_ShowNum(40, 100, ADC_Value[0], 4, WHITE, GRAY);
      colorFlag = GRAY;
    }
    
    
  }
}