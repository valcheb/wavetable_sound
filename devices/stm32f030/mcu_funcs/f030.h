#ifndef F030_H_
#define F030_H_

#include "stm32f0xx.h"
#include "stm32f0xx_conf.h"
#include "stm32f0xx_rcc.h"
#include "stm32f0xx_gpio.h"
#include "stm32f0xx_tim.h"

#include "stdbool.h"

void f030_msDelay(uint32_t ms);
void f030_init_mcu();
void f030_enable_pwm();
void f030_disable_pwm();
void f030_pwm_play(uint8_t data);
void f030_set_expand(uint8_t value);
bool f030_is_data_needed();

#endif /*F030_H_*/
