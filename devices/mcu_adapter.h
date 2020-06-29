#ifndef MCU_ADAPTER_H_
#define MCU_ADAPTER_H_

#include "stdbool.h"
#include "stdint.h"

void ma_init_mcu();
void ma_pwm_play(uint8_t data);
bool ma_is_data_needed();
void ma_enable_pwm();
void ma_disable_pwm();

#endif /*MCU_ADAPTER_H_*/
