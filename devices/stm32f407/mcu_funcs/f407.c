#include "f407.h"

#define TIMER_PERIOD 1750 //48kHz PWM freq

inline static void f407_init_gpio()
{
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);

    GPIO_InitTypeDef init;
    GPIO_StructInit(&init);
    init.GPIO_Mode = GPIO_Mode_AF;
    init.GPIO_OType = GPIO_OType_PP;
    init.GPIO_Pin = GPIO_Pin_12;
    init.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOD, &init);

    GPIO_PinAFConfig(GPIOD, GPIO_PinSource12, GPIO_AF_TIM4);
}

inline static void f407_init_pwm()
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_TimeBaseInitTypeDef base_timer;
    TIM_TimeBaseStructInit(&base_timer);

    base_timer.TIM_Prescaler = 1-1;
    base_timer.TIM_Period = TIMER_PERIOD;
    base_timer.TIM_CounterMode = TIM_CounterMode_Up;
	base_timer.TIM_ClockDivision = TIM_CKD_DIV1;
	base_timer.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM4, &base_timer);

    TIM_OCInitTypeDef oc_init;
    TIM_OCStructInit(&oc_init);
    oc_init.TIM_OCMode = TIM_OCMode_PWM1;
    oc_init.TIM_OutputState = TIM_OutputState_Enable;
    oc_init.TIM_Pulse = 0;
    oc_init.TIM_OCPolarity = TIM_OCPolarity_High;

    TIM_OC1Init(TIM4, &oc_init);
    TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);

    TIM_ARRPreloadConfig(TIM4, ENABLE);
}

void f407_init_mcu()
{
    f407_init_gpio();
    f407_init_pwm();
}

void f407_enable_pwm()
{
    TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);
    TIM_Cmd(TIM4, ENABLE);
	NVIC_EnableIRQ(TIM4_IRQn);
}

void f407_disable_pwm()
{
    TIM_ITConfig(TIM4, TIM_IT_Update, DISABLE);
	TIM_Cmd(TIM4, DISABLE);
	NVIC_DisableIRQ(TIM4_IRQn);
}

/*pwm_player*/
volatile static bool need_data = true;

inline static void f407_send_to_pwm(uint8_t data)
{
    TIM4->CCR1 = data * TIMER_PERIOD / UINT8_MAX;
}

void f407_pwm_play(uint8_t data)
{
    need_data = false;
    f407_send_to_pwm(data);
}

bool f407_is_data_needed()
{
    return need_data;
}

void TIM4_IRQHandler()
{
    need_data = true;

    TIM_ClearITPendingBit(TIM4, TIM_IT_Update);
    while (TIM_GetITStatus(TIM4, TIM_IT_Update) == SET) {__NOP;}
}
