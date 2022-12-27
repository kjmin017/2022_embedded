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

void RCC_Configure(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
}

void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
 
    GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void NVIC_Configure(void) {

    NVIC_InitTypeDef NVIC_InitStructure;
    
    
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

    NVIC_EnableIRQ(ADC1_2_IRQn);
    NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0; // TODO
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
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
    ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
    ADC_Cmd(ADC1, ENABLE);
    ADC_ResetCalibration(ADC1);
    while(ADC_GetResetCalibrationStatus(ADC1)){ }
    ADC_StartCalibration(ADC1);
    while(ADC_GetCalibrationStatus(ADC1)){  }
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void ADC1_2_IRQHandler(void) { 
	if (ADC_GetITStatus(ADC1, ADC_IT_EOC) != RESET) {
          value = ADC_GetConversionValue(ADC1);
          ADC_ClearITPendingBit(ADC1, ADC_IT_EOC);
	}
}

int main() {
  
// LCD 관련 설정은 LCD_Init에 구현되어 있으므로 여기서 할 필요 없음
  SystemInit();
  RCC_Configure();
  GPIO_Configure();
  ADC_Configure();
  NVIC_Configure();
  
  
  // ------------------------------------
  LCD_Init();
  Touch_Configuration();
  Touch_Adjust();
  LCD_Clear(WHITE);
  uint16_t rawTouchX = 0;
  uint16_t rawTouchY = 0;
  uint16_t touchX = 0;
  uint16_t touchY = 0;
  while(1){
  // TODO : LCD 값 출력 및 터치 좌표 읽기
    Touch_GetXY(&rawTouchX, &rawTouchY, 1); //Wait until Touched
    Convert_Pos(rawTouchX, rawTouchY, &touchX, &touchY);
    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    LCD_ShowString(40, 40, "Team07", BLACK, WHITE);
    LCD_ShowNum(40, 60, touchX, 4, BLACK, WHITE);
    LCD_ShowNum(40, 80, touchY, 4, BLACK, WHITE);
    LCD_ShowNum(40, 100, value, 4, BLACK, WHITE);
    LCD_DrawCircle(touchX, touchY, 4);
  }
}