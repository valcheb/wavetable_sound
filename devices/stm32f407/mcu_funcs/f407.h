#ifndef F407_H_
#define F407_H_

#include "stm32f4xx.h"
#include "stm32f4xx_conf.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_tim.h"

#include "stdbool.h"

void f407_msDelay(uint32_t ms);

void f407_init_gpio();
void f407_init_pwm();
void f407_init_mcu();
void f407_disable_mcu();

void f407_pwm_play(uint8_t data);
bool f407_is_data_needed();

#endif /*F407_H_*/
