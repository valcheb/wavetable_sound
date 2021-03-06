#ifndef WAVETABLE_DICTIONARY_H
#define WAVETABLE_DICTIONARY_H

#include "stdint.h"

#define ACCURACY           1024
/* about ACCURACY
this value was chosen because of several reasons.
1) should avoid float point calculation, so multiply and divide values on this constant
2) several expressions in wavetable_synth module may have wrong results if ACCURACY will less than 1000
3) stm32f030 haven't hardware division, but division on the degree of two faster than others dividers
*/
#define CHANNEL_MAX        4
#define WAVE_MAX           16
#define SMOOTH_MAX         8

/*offsets*/
/*header*/
#define HEADER_SIZE_OS     8
#define BPM_OS             0
#define RATE_OS            10
#define CHAN_N_OS          8
#define SMOOTH_N_OS        4
#define WAVE_N_OS          0
/*note*/
#define TYPE_OS            15
#define NOTE_OS            8
#define DURATION_OS        5
#define DURATION_P_OS      0
/*channel*/
#define SMOOTH_OS          12
#define WAVE_OS            8
#define VOLUME_OS          0

/*bit masks*/
#define HEADER_SIZE_MASK     0x00ff
#define BPM_MASK             0x00ff
#define RATE_MASK            0x003f
#define CHAN_N_MASK          0x0003
#define SMOOTH_N_MASK        0x000f
#define WAVE_N_MASK          0x000f
/*note*/
#define TYPE_MASK            0x0001
#define NOTE_MASK            0x00ff
#define DURATION_MASK        0x0007
#define DURATION_P_MASK      0x001f
/*channel*/
#define SMOOTH_MASK          0x0007
#define WAVE_MASK            0x000f
#define VOLUME_MASK          0x00ff

/*note freq*/
#define NOTE_FREQ_SIZE 98
#define NOTE_PAUSE     97

typedef enum
{
    NOTE_C = -4,
    NOTE_Cd,
    NOTE_D,
    NOTE_Dd,
    NOTE_E,
    NOTE_F,
    NOTE_Fd,
    NOTE_G,
    NOTE_Gd,
    NOTE_A,
    NOTE_Ad,
    NOTE_B,
} note_e;

extern uint32_t note_freq[NOTE_FREQ_SIZE];

/*note duration*/
#define DURATION_BASE 4*60 //seconds
#define KILO 1000 //for hertz

typedef enum
{
    DURATION_WHOLE = 0,
    DURATION_HALF,
    DURATION_QUARTER,
    DURATION_EIGHTH,
    DURATION_SIXTEENTH,
    DURATION_THIRTY_SECOND,
    DURATION_SIXTY_FOURTH
} duration_e;

extern uint8_t durations[];

typedef enum
{
    DURATION_SIMPLE = 0,
    DURATION_POINT,
    DURATION_TRIOLET
} duration_mode_e;

/*rate*/
typedef enum
{
    RATE_8  = 0,
    RATE_16,
    RATE_48
} rate_e;

extern uint8_t rate[];

/*channels*/
typedef enum
{
    CHAN_NUM_1 = 0,
    CHAN_NUM_2,
    CHAN_NUM_3,
    CHAN_NUM_4
} chan_num_e;

/*smooth*/
typedef enum
{
    WAVE_1 = 0,
    WAVE_2,
    WAVE_3,
    WAVE_4,
} wave_e;

typedef enum
{
    SMOOTH_0 = 0,
    SMOOTH_1,
    SMOOTH_2,
    SMOOTH_3,
    SMOOTH_4,
    SMOOTH_5,
    SMOOTH_6,
    SMOOTH_7
} smooth_e;

#endif /*WAVETABLE_DICTIONARY_H*/
