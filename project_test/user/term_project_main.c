#include <stdio.h>
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

uint16_t div5 = 0;

uint32_t ADC_Values[4];

uint32_t tempValue = 0;
uint32_t darkValue = 0;
uint32_t dryValue = 0;
uint32_t waterEmpty = 0;

uint16_t motorFlag = 0;

//char wordFromUART2 = 'A';
u8* pmsg;

void RCC_Configure(void);
void GPIO_Configure(void);
void USART2_Init(void);
void NVIC_Configure(void);
void TIM_Configure(void);
void change(uint16_t pulse);
void TIM2_IRQHandler(void);

TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;

void sendDataUART2(uint16_t data);

void RCC_Configure(void)
{
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
      RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
      RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

      RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
      RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
      RCC_APB1PeriphClockCmd(RCC_APB1ENR_USART2EN, ENABLE);
}

void GPIO_Configure(void)
{
      GPIO_InitTypeDef GPIO_InitStructure;

      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
      GPIO_Init(GPIOD, &GPIO_InitStructure);

      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
      GPIO_Init(GPIOC, &GPIO_InitStructure);

      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
      GPIO_Init(GPIOB, &GPIO_InitStructure);

      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;

      GPIO_Init(GPIOA, &GPIO_InitStructure);

      //USART2 bluetooth
      //TX
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
      GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
      GPIO_Init(GPIOA, &GPIO_InitStructure);
      //RX
      GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
      GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
      GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void USART2_Init(void)
{
      USART_InitTypeDef USART2_InitStructure;

      // Enable the USART1 peripheral

      USART_Cmd(USART2, ENABLE);

      // TODO: Initialize the USART using the structure 'USART_InitTypeDef' and the function 'USART_Init'
      USART2_InitStructure.USART_BaudRate = 9600;
      USART2_InitStructure.USART_WordLength = (uint16_t) USART_WordLength_8b;
      USART2_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
      USART2_InitStructure.USART_Parity = (uint16_t) USART_Parity_No;
      USART2_InitStructure.USART_StopBits = (uint16_t) USART_StopBits_1;
      USART2_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;


      USART_Init(USART1, &USART2_InitStructure);
      USART_Init(USART2, &USART2_InitStructure);
      // TODO: Enable the USART1 RX interrupts using the function 'USART_ITConfig' and the argument value 'Receive Data register not empty interrupt'

      USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}


void NVIC_Configure(void) {

      NVIC_InitTypeDef NVIC_InitStructure;


      NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

      NVIC_EnableIRQ(TIM2_IRQn);
      NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0; // TODO
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0; // TODO
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);


      NVIC_EnableIRQ(ADC1_2_IRQn);
      NVIC_InitStructure.NVIC_IRQChannel = ADC1_2_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x1; // TODO
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0; // TODO
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);

      //USART2
      NVIC_EnableIRQ(USART2_IRQn);
      NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
      NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x2; // TODO
      NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0; // TODO
      NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
      NVIC_Init(&NVIC_InitStructure);

}



void TIM_Configure(){
      TIM_TimeBaseStructure.TIM_Period = 10000-1;
      TIM_TimeBaseStructure.TIM_Prescaler = 7200-1;
      TIM_TimeBaseStructure.TIM_ClockDivision = 0;
      TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
      TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
      TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
      TIM_ARRPreloadConfig(TIM2, ENABLE);

      TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
      TIM_Cmd(TIM2, ENABLE);

}

void DMA_Configure(void){
  
      DMA_InitTypeDef DMA_Instructure;
      DMA_DeInit(DMA1_Channel1);
      DMA_Instructure.DMA_PeripheralBaseAddr = (uint32_t)&ADC1->DR;
      DMA_Instructure.DMA_MemoryBaseAddr=(uint32_t)ADC_Values;

      DMA_Instructure.DMA_Mode=DMA_Mode_Circular;
      DMA_Instructure.DMA_Priority= DMA_Priority_High;
      DMA_Instructure.DMA_M2M= DMA_M2M_Disable;
      DMA_Instructure.DMA_BufferSize= 4;
      DMA_Instructure.DMA_DIR= DMA_DIR_PeripheralSRC;
      DMA_Instructure.DMA_MemoryInc=DMA_MemoryInc_Enable;
      DMA_Instructure.DMA_MemoryDataSize=DMA_MemoryDataSize_Word;
      DMA_Instructure.DMA_PeripheralDataSize=DMA_PeripheralDataSize_Word;
      DMA_Instructure.DMA_PeripheralInc=DMA_PeripheralInc_Disable;
      DMA_Init(DMA1_Channel1, &DMA_Instructure);
      DMA_Cmd(DMA1_Channel1, ENABLE);

}

void ADC_Configure(){
      ADC_InitTypeDef ADC_InitStructure;

      ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
      ADC_InitStructure.ADC_ScanConvMode = ENABLE;
      ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
      ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
      ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
      ADC_InitStructure.ADC_NbrOfChannel = 4;

      ADC_Init(ADC1, &ADC_InitStructure);

      ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_55Cycles5); //수위
      ADC_RegularChannelConfig(ADC1, ADC_Channel_4, 2, ADC_SampleTime_55Cycles5); //온도

      ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 3, ADC_SampleTime_55Cycles5); //조도
      ADC_RegularChannelConfig(ADC1, ADC_Channel_6, 4, ADC_SampleTime_55Cycles5); //습도

      //ADC_ITConfig(ADC1, ADC_IT_EOC, ENABLE);
      ADC_DMACmd(ADC1, ENABLE);
      ADC_Cmd(ADC1, ENABLE);

      while(ADC_GetCalibrationStatus(ADC1)){ }
      ADC_ResetCalibration(ADC1);

      while(ADC_GetResetCalibrationStatus(ADC1)){ }
      ADC_StartCalibration(ADC1);

      ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}


void USART2_IRQHandler() {
      if(USART_GetITStatus(USART2,USART_IT_RXNE)!=RESET){
      // the most recent received data by the USART1 peripheral
      char temp = USART_ReceiveData(USART2);
      // clear 'Read data register not empty' flag
  }
}

void sendDataUART2(uint16_t data) {
/* Wait till TC is set */
	USART_SendData(USART2, data);
    while ((USART2->SR & USART_SR_TC) == 0);
}

void sendMessage_water(int8_t data){
  char msg[50];
  char * boundaryMsg = (data == 1 ? "min" : "max");
  snprintf(msg,50,"LED%s", boundaryMsg);
  pmsg = "NOT ENOUGH WATER.\r\n";
  while(*pmsg!=0){
      sendDataUART2(*pmsg);
      pmsg++;
  }
}

void sendMessage_tempcold(int8_t data){
  char msg[50];
  char * boundaryMsg = (data == 1 ? "min" : "max");
  snprintf(msg,50,"LED%s", boundaryMsg);
  pmsg = "TOO COLD.\r\n";
  while(*pmsg!=0){
      sendDataUART2(*pmsg);
      pmsg++;
  }
}

void sendMessage_temphot(int8_t data){
  char msg[50];
  char * boundaryMsg = (data == 1 ? "min" : "max");
  snprintf(msg,50,"LED%s", boundaryMsg);
  pmsg = "TOO HOT.\r\n";
  while(*pmsg!=0){
      sendDataUART2(*pmsg);
      pmsg++;
  }
}


void TIM2_IRQHandler(void) {
  if((TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)){ 
   if(motorFlag){
      GPIO_SetBits(GPIOD, GPIO_Pin_3);
      div5++;

      if(div5 % 5 == 0){
      GPIO_ResetBits(GPIOD, GPIO_Pin_3);
      div5=0;
      motorFlag = 0;
      }
    }
  else{
    GPIO_SetBits(GPIOD, GPIO_Pin_3);
  }
    TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }
}


void Delay(void) {
    int i;

    for (i = 0; i < 2000000; i++) {}
}

int main() {

  SystemInit();
  RCC_Configure();
  GPIO_Configure();
  ADC_Configure();
  DMA_Configure();
  NVIC_Configure();
  TIM_Configure();
  USART2_Init();
  // ------------------------------------
  //USART1_Init();
  LCD_Init();

  LCD_Clear(WHITE);
  
  uint8_t message = 5;

  while(1){
    // TODO : LCD 값 출력 및 터치 좌표 읽기
    LCD_ShowString(40, 40, "Team07", BLACK, WHITE);

    ADC_SoftwareStartConvCmd(ADC1, ENABLE);
    
    waterEmpty = ADC_Values[0];
    tempValue = ADC_Values[1];
    darkValue = ADC_Values[2];
    dryValue = ADC_Values[3];
    
    
    LCD_ShowString(20, 60, "empty:", BLACK, WHITE);
    LCD_ShowString(20, 100, "temp: ", BLACK, WHITE);
    LCD_ShowString(20, 140, "dark: ", BLACK, WHITE);
    LCD_ShowString(20, 180, "dry: ", BLACK, WHITE);
    
    
    LCD_ShowNum(70, 60, waterEmpty/30, 4, BLACK, WHITE);
    LCD_ShowNum(70, 100, tempValue/100, 4, BLACK, WHITE);
    LCD_ShowNum(70, 140, darkValue/50, 4, BLACK, WHITE);
    LCD_ShowNum(70, 180, dryValue/50, 4, BLACK, WHITE);
    
    
    
    if(dryValue > 2400)  //건조 시 물 (건조할수록 값이 큼)
    {
      if(darkValue > 700)  //조도. 밝을수록 값이 작아짐
      {
        if(waterEmpty > 3000)  //물이 없을수록 값이 큼
          motorFlag = 1;
        else
          sendMessage_water(message);
      }
    }
    
    if(tempValue < 2300){
      sendMessage_tempcold(message);
      Delay();
    }
    else if(tempValue > 3000){
      sendMessage_temphot(message);
      Delay();
    }
    
    Delay();
  }

}