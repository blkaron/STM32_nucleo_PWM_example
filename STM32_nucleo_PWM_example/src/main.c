/**
  ******************************************************************************
  * PWM simple example for STM32 Nucleo L152RE board
  *
  * @author  Viktor Stoev
  * @version V1.0
  * @date    August-2017
  ******************************************************************************
*/

#include "stm32l1xx.h"
#include "stm32l1xx_gpio.h"
#include "stm32l1xx_rcc.h"
#include "stm32l1xx_nucleo.h"

#define LED2 GPIO_Pin_5

__IO uint32_t TimingDelay;

void Init_LED2()
{
	/* SystemCoreClock is 32MHz, dividing it by 1000 would give 1ms interrupts */
	if (SysTick_Config(SystemCoreClock / 1000))
		{
			while(1);
		}

	/* Initialize GPIO_Pin_5 */
	GPIO_InitTypeDef GPIOPortAInit;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_StructInit(&GPIOPortAInit);
	GPIOPortAInit.GPIO_Pin = LED2;
	GPIOPortAInit.GPIO_Mode = GPIO_Mode_AF;
	GPIOPortAInit.GPIO_OType = GPIO_OType_PP;
	GPIOPortAInit.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(GPIOA, &GPIOPortAInit);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_TIM2);
	}

void Init_TIM2()
{
	/* Initialize TIM2*/
	TIM_TimeBaseInitTypeDef TIM2Init;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	/* set PWM_Period to be 100 timer steps (clocks) and the occurrence
	 * of the PWM UEV event to be 100 times per second (PWM_Freq)*/
	uint32_t PWM_Period = 100;
	uint32_t PWM_Freq = 100;
	uint32_t COUNTER_Frequency = PWM_Period * PWM_Freq;
	uint32_t PSC_Value = (SystemCoreClock / COUNTER_Frequency) - 1;
	uint16_t ARR_Value = PWM_Period - 1;
	TIM_TimeBaseStructInit(&TIM2Init);

	TIM2Init.TIM_Period = ARR_Value;
	TIM2Init.TIM_Prescaler = PSC_Value;
	TIM_TimeBaseInit(TIM2, &TIM2Init);
	TIM_Cmd(TIM2, ENABLE);
}

void Init_OC1()
{
	/* Initialize OC1 (connected to GPIO_Pin_5) of TIM2 in PWM mode */
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	TIM_OCStructInit (&TIM_OCInitStructure);
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
	TIM_OCInitStructure.TIM_Pulse = 0;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
}

void Delay(__IO uint32_t time)
{
	/* time delay in the execution */
	TimingDelay = time;
	while(TimingDelay !=0);
}

int main()
{
	Init_LED2();
	Init_TIM2();
	Init_OC1();

	while(1)
	{
		for(int i=1; i < 100;i++)
		{
			/* delay 10ms for each of the 100 values of PWM - this gives 1s of total loop */
			Delay(10);
			TIM_SetCompare1(TIM2 , i);
		}
	}
	return 0;
}

void SysTick_Handler(void)
{
	/* Overwriting the default SysTick_Handler implementation */
	if(TimingDelay !=0)
	{
		TimingDelay--;
	}
}
