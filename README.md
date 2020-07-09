# Wavetable sound player
The project generates sound based on [wavetable synthesis method](https://www.music.mcgill.ca/~gary/307/week4/wavetables.html) and plays with 48kHz PWM.

## Target platform
The project created for stm32f030 and stm32f407.
It can be ported to another stm32f microcontrollers.

## Requirements
The project uses CMSIS and SPL.
Put libraries files in follow directories:
```sh
wavetable_sound/devices/stm32fxxx/library/CMSIS/core
wavetable_sound/devices/stm32fxxx/library/CMSIS/device
wavetable_sound/devices/stm32fxxx/library/SPL/inc
wavetable_sound/devices/stm32fxxx/library/SPL/src
```

## Build
Use makefile from
```sh
wavetable_sound/devices/stm32fxxx
```
