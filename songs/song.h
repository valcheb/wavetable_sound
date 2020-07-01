#include "wavetable_dictionary.h"
#include "stdint.h"

/*typedef enum
    {
        WAVE_SQUARE = 0,
        WAVE_TRIANGLE,
        WAVE_SIN,
    } wave_e;

    static uint8_t square[] = {};
    static uint8_t triangle[] = {};
    static uint8_t sin[] = {};

    static uint8_t* wavetable[] = {square, triangle, sin};
*/
#define NOTE_ID(note,octave) ((note) + 12*(octave))
#define SHIFT_LEFT(x,y) ((x)<<(y))

/*header*/
#define HEADER_SIZE_S(x)   SHIFT_LEFT(x,HEADER_SIZE_OS)
#define BPM_S(x)           SHIFT_LEFT(x,BPM_OS)
#define RATE_S(x)          SHIFT_LEFT(x,RATE_OS)
#define CHAN_N_S(x)        SHIFT_LEFT(x,CHAN_N_OS)
#define WAVE_N_S(x)        SHIFT_LEFT(x,WAVE_N_OS)
/*note*/
#define TYPE_S(x)          SHIFT_LEFT(x,TYPE_OS)
#define NOTE_S(x)          SHIFT_LEFT(x,NOTE_OS)
#define DURATION_S(x)      SHIFT_LEFT(x,DURATION_OS)
#define DURATION_P_S(x)    SHIFT_LEFT(x,DURATION_P_OS)
/*channel*/
#define SMOOTH_S(x)        SHIFT_LEFT(x,SMOOTH_OS)
#define WAVE_S(x)          SHIFT_LEFT(x,WAVE_OS)
#define VOLUME_S(x)        SHIFT_LEFT(x,VOLUME_OS)

#define NOTE(note, octave, dur, dur_param) TYPE_S(0) + NOTE_S(NOTE_ID(note,octave)) + DURATION_S(dur) + DURATION_P_S(dur_param)
#define CHANNEL(smooth, wave, volume)      TYPE_S(1) + SMOOTH_S(smooth) + WAVE_S(wave) + VOLUME_S(volume)
#define HEADER(bpm, rate_, chan_n, wave)   HEADER_SIZE_S(2+(chan_n)) + BPM_S(bpm), RATE_S(rate_) + CHAN_N_S((chan_n)-1) + WAVE_N_S(wave)
#define DATA_SIZE(x)                       (sizeof(x)/sizeof((x)[0]))

typedef enum
{
    WAVE_1 = 0
} wave_e;

extern uint8_t test_array1[];
/*extern uint8_t test_array2[];*/
extern uint8_t *wavetables[];
extern uint16_t song[];
