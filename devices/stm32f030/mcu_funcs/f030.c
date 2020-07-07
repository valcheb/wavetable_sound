#include "f030.h"

#define TIMER_PERIOD 6000/6 //48kHz PWM freq

void f030_msDelay(uint32_t ms)
{
    volatile uint32_t i;
    RCC_ClocksTypeDef rcc;

    RCC_GetClocksFreq(&rcc);
    i = (rcc.HCLK_Frequency/10/1000)*ms;
    for (i; i != 0; i--);
}

inline static void f030_init_gpio()
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA,ENABLE);

    GPIO_InitTypeDef init;
    init.GPIO_Mode = GPIO_Mode_AF;
    init.GPIO_OType = GPIO_OType_PP;
    init.GPIO_Pin = GPIO_Pin_4;
    init.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA,&init);

    GPIO_PinAFConfig(GPIOA,GPIO_PinSource4,GPIO_AF_4);
}

inline static void f030_init_pwm()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);

    TIM_TimeBaseInitTypeDef base_timer;
    TIM_TimeBaseStructInit(&base_timer);

    base_timer.TIM_Prescaler = 1-1;
    base_timer.TIM_Period = TIMER_PERIOD;
    base_timer.TIM_CounterMode = TIM_CounterMode_Up;
	base_timer.TIM_ClockDivision = TIM_CKD_DIV1;
	base_timer.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM14, &base_timer);

    TIM_OCInitTypeDef oc_init;
    TIM_OCStructInit(&oc_init);
    oc_init.TIM_OCMode = TIM_OCMode_PWM1;
    oc_init.TIM_OutputState = TIM_OutputState_Enable;
    oc_init.TIM_Pulse = 0;
    oc_init.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC1Init(TIM14,&oc_init);
    TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM14,ENABLE);
}

void f030_init_mcu()
{
    f030_init_gpio();
    f030_init_pwm();
}

void f030_enable_pwm()
{
    TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM14,ENABLE);
    NVIC_EnableIRQ(TIM14_IRQn);
}

void f030_disable_pwm()
{
    TIM_ITConfig(TIM14, TIM_IT_Update, DISABLE);
	TIM_Cmd(TIM14,DISABLE);
	NVIC_DisableIRQ(TIM14_IRQn);
}

/*pwm_player*/
volatile static bool need_data = true;

inline static void f030_send_to_pwm(uint8_t data)
{
    TIM14->CCR1 = data*TIMER_PERIOD/UINT8_MAX;
}

void f030_pwm_play(uint8_t data)
{
    need_data = false;
    f030_send_to_pwm(data);
}

bool f030_is_data_needed()
{
    return need_data;
}

void TIM14_IRQHandler()
{
    need_data = true;

    TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
    while (TIM_GetITStatus(TIM14,TIM_IT_Update) == SET) {__NOP;}
}
