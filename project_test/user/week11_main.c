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

uint16_t toggle = 0;
uint16_t led1 = 0;
uint16_t led2 = 0;

uint16_t div5 = 0;

uint16_t motorIdx[3] = {700, 1500, 2600};

void RCC_Configure(void);
void GPIO_Configure(void);
void NVIC_Configure(void);
void TIM_Configure(void);
void change(uint16_t pulse);
void TIM2_IRQHandler(void);

TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;

void RCC_Configure(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
        RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
}

void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
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
}



void TIM_Configure(){
  TIM_TimeBaseStructure.TIM_Period = 10000;         
    TIM_TimeBaseStructure.TIM_Prescaler = 7200;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
    TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
    TIM_ARRPreloadConfig(TIM2, ENABLE);

    TIM_TimeBaseStructure.TIM_Prescaler = (uint16_t) (SystemCoreClock / 1000000) - 1;
    TIM_TimeBaseStructure.TIM_Period = 20000-1;
    TIM_TimeBaseStructure.TIM_ClockDivision = 0;
    TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 1500; // us
    TIM_OC3Init(TIM3, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM3, TIM_OCPreload_Disable);
    TIM_ARRPreloadConfig(TIM3, ENABLE);

    TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM2, ENABLE);
    TIM_Cmd(TIM3, ENABLE);
  
  
}
void TIM2_IRQHandler(void) { 
  if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET){
    if(toggle){
     div5++;
     change(motorIdx[div5%3]);
     if(div5 % 5 == 0){
       if(led2 == 0){
          GPIO_ResetBits(GPIOD, GPIO_Pin_3); 
          led2 = 1;
       }
       else{
         GPIO_SetBits(GPIOD, GPIO_Pin_3);
         led2=0;
       }
       div5=0;
    }

       if(led1 == 0){
          GPIO_ResetBits(GPIOD, GPIO_Pin_2); 
          led1 = 1;
       }
       else{
         GPIO_SetBits(GPIOD, GPIO_Pin_2);
         led1=0;
       }
  }
  else{
       GPIO_ResetBits(GPIOD, GPIO_Pin_2);
       GPIO_ResetBits(GPIOD, GPIO_Pin_3);
  }
  TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
  }
}
void change(uint16_t pul){
  uint16_t pwm_pulse;
  pwm_pulse = pul;
  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = pwm_pulse; // us
  TIM_OC3Init(TIM3, &TIM_OCInitStructure);
}
int main() {
  

  SystemInit();
  RCC_Configure();
  GPIO_Configure();
  NVIC_Configure();
  TIM_Configure();
  
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
    LCD_ShowString(40, 40, "Team07", BLACK, WHITE);
    LCD_ShowString(50, 90, "BUT", RED, WHITE);
    LCD_DrawRectangle(40, 80, 80, 120);
    
    Touch_GetXY(&rawTouchX, &rawTouchY, 1); //Wait until Touched
    Convert_Pos(rawTouchX, rawTouchY, &touchX, &touchY);
    if(touchX>=40 && touchX<=80 && touchY>= 80 && touchY <= 120){
      if(toggle == 0){
        toggle = 1;
        LCD_ShowString(40, 60, "OFF", WHITE, WHITE);
        LCD_ShowString(40, 60, "ON", RED, WHITE);
      }
      else{
        toggle = 0;
        LCD_ShowString(40, 60, "ON", WHITE, WHITE);
        LCD_ShowString(40, 60, "OFF", RED, WHITE);
        div5=0;
      }
    }
  }
}