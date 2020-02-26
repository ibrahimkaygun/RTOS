/*----------------------------------------------------------------------------
 * CMSIS-RTOS 'main' function template
 *---------------------------------------------------------------------------*/

#define osObjectsPublic                     // define objects in main module
#include "osObjects.h"                      // RTOS object definitions
#include "stm32f10x.h"                  // Device header
#include "stdio.h"
#include "stdlib.h"
char str[49];
uint16_t Adc_Data;
uint16_t Adc_Read()
{
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
	while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
	return ADC_GetConversionValue(ADC1);
}
void Usart_Put(USART_TypeDef *USARTx ,volatile char *j)
{
	while(*j)
	{
		while(!(USARTx->SR & USART_SR_TC));
		USART_SendData(USARTx ,*j);
		*j++;
	}
}


/*
 * main: initialize and start the system
 */
 void Adc1_Thread(void const *argumant);
 void Usart1_Thread(void const *argument);
 void Led1_Thread(void const *argument);
 void Led2_Thread(void const *argument);
osThreadId Adc1_id ,Usart1_id ,Led1_id ,Led2_id;
osThreadDef(Adc1_Thread,osPriorityNormal,1,0);
osThreadDef(Usart1_Thread,osPriorityNormal,1,0);
osThreadDef(Led1_Thread,osPriorityNormal,1,0);
osThreadDef(Led2_Thread,osPriorityNormal,1,0);
int main (void) {
  osKernelInitialize ();                    // initialize CMSIS-RTOS

  // initialize peripherals here
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	 GPIO_InitTypeDef GPIO_IntStr;
	 GPIO_IntStr.GPIO_Mode = GPIO_Mode_Out_PP;;
	 GPIO_IntStr.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
	 GPIO_IntStr.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init(GPIOB ,&GPIO_IntStr);

  // create 'thread' functions that start executing,
  // example: tid_name = osThreadCreate (osThread(name), NULL);
  Adc1_id = osThreadCreate(osThread(Adc1_Thread) , NULL);
	Usart1_id = osThreadCreate(osThread(Usart1_Thread),NULL);
	Led1_id = osThreadCreate(osThread(Led1_Thread),NULL);
	Led2_id = osThreadCreate(osThread(Led2_Thread),NULL);
  osKernelStart (); 	// start thread execution 
	while(1)
	{
	 GPIO_SetBits(GPIOB ,GPIO_Pin_5);
	 osDelay(500);
	 GPIO_ResetBits(GPIOB ,GPIO_Pin_5);
	 osDelay(500);
	}
}

void Adc1_Thread(void const *argumant)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1 | RCC_APB2Periph_GPIOA ,ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	//uint16_t Adc_Data;
	ADC_InitTypeDef ADC_IntStr;
	GPIO_InitTypeDef GPIO_IntStr;
	GPIO_IntStr.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_IntStr.GPIO_Pin = GPIO_Pin_0;
	//GPIO_IntStr.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA,&GPIO_IntStr);
	
	ADC_IntStr.ADC_ContinuousConvMode = DISABLE;
	ADC_IntStr.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_IntStr.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_IntStr.ADC_Mode = ADC_Mode_Independent;
	ADC_IntStr.ADC_NbrOfChannel = 1;
	ADC_IntStr.ADC_ScanConvMode = DISABLE;
	ADC_Init(ADC1,&ADC_IntStr);
	
	ADC_Cmd(ADC1 , ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	ADC_RegularChannelConfig(ADC1,ADC_Channel_0,1,ADC_SampleTime_13Cycles5);
	
	while(1)
	{
		Adc_Data = Adc_Read();
		osDelay(100);
	}
}
 void Usart1_Thread(void const *argument)
 {
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_USART1 ,ENABLE);
	 USART_InitTypeDef USART_IntStr;
	 GPIO_InitTypeDef GPIO_IntStr;
	 GPIO_IntStr.GPIO_Mode = GPIO_Mode_AF_PP;
	 GPIO_IntStr.GPIO_Pin = GPIO_Pin_9;
	 GPIO_IntStr.GPIO_Speed = GPIO_Speed_50MHz;
	 GPIO_Init(GPIOA ,&GPIO_IntStr);
	 
	 USART_IntStr.USART_BaudRate = 9600;
	 USART_IntStr.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	 USART_IntStr.USART_Mode = USART_Mode_Tx;
	 USART_IntStr.USART_Parity = USART_Parity_No;
	 USART_IntStr.USART_StopBits = USART_StopBits_1;
	 USART_IntStr.USART_WordLength = USART_WordLength_8b;
	 USART_Init(USART1 ,&USART_IntStr);
	 
	 USART_Cmd(USART1 , ENABLE);
	 for(;;)
	 {
		 sprintf(str,"%d/n",Adc_Data);
		 Usart_Put(USART1,str);
		 osDelay(1000);
	 }
 }
 void Led1_Thread(void const *argument)
 {
	 while(1)
	 {
	 GPIO_SetBits(GPIOB ,GPIO_Pin_6);
	 osDelay(300);
	 GPIO_ResetBits(GPIOB ,GPIO_Pin_6);
	 osDelay(300);
	 }
 }
 void Led2_Thread(void const *argument)
 {
	 while(1)
	 {
	 GPIO_SetBits(GPIOB ,GPIO_Pin_7);
	 osDelay(500);
	 GPIO_ResetBits(GPIOB ,GPIO_Pin_7);
	 osDelay(500);
	 }
 }
