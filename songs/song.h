#include "wavetable_dictionary.h"
#include "stdint.h"

#define NOTE_ID(note,octave) ((note) + 12*(octave))
#define SHIFT_LEFT(x,y)      ((x)<<(y))

/*header*/
#define HEADER_SIZE_S(x)   SHIFT_LEFT(x,HEADER_SIZE_OS)
#define BPM_S(x)           SHIFT_LEFT(x,BPM_OS)
#define RATE_S(x)          SHIFT_LEFT(x,RATE_OS)
#define CHAN_N_S(x)        SHIFT_LEFT(x,CHAN_N_OS)
#define SMOOTH_N_S(x)      SHIFT_LEFT(x,SMOOTH_N_OS)
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

#define DATA_SIZE(x)                               (sizeof(x)/sizeof((x)[0]))
#define NOTE(note, octave, dur, dur_param)         TYPE_S(0) + NOTE_S(NOTE_ID(note,octave)) + DURATION_S(dur) + DURATION_P_S(dur_param)
#define CHANNEL(smooth, wave, volume)              TYPE_S(1) + SMOOTH_S(smooth) + WAVE_S(wave) + VOLUME_S(volume)
#define HEADER(bpm, rate_, chan_n, wave, smooth)   HEADER_SIZE_S(2+(chan_n)) + BPM_S(bpm),\
                                                   RATE_S(rate_) + CHAN_N_S((chan_n)-1) + WAVE_N_S(wave) + SMOOTH_N_S(smooth)

extern uint16_t song[];
