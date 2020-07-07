#ifndef WAVETABLE_SYNTH_H_
#define WAVETABLE_SYNTH_H_

#include "stdbool.h"
#include "stdint.h"

#include "wavetable_dictionary.h"
#include "song.h"

typedef struct
{
    uint32_t phase;
    uint32_t phase_increment;
    uint32_t length;
} note_t;

typedef struct
{
    uint32_t phase;
    uint32_t counter;
    uint32_t step;
    uint8_t  number;
} smooth_t;

typedef struct
{
    uint16_t length;
    uint8_t  number;
} wave_t;

typedef struct
{
    uint16_t index;
    uint16_t size;
    uint16_t offset;
} note_data_t;

typedef struct
{
    note_t      note;
    smooth_t    smooth;
    wave_t      wave;
    note_data_t data;
    uint8_t     volume;
} channel_t;

typedef struct
{
    /*header*/
    uint8_t   header_size;
    uint8_t   bpm;
    uint8_t   rate;
    uint8_t   chan_number;
    uint8_t   wave_number;
    uint8_t   smooth_number;
    /*song info*/
    uint32_t  song_len;
    uint8_t   current_chan;
    channel_t channels[CHANNEL_MAX];
    /*wave*/
    uint16_t  wave_sizes[WAVE_MAX];
    uint16_t  wave_offsets[WAVE_MAX];
    /*smooth*/
    uint16_t  smooth_sizes[SMOOTH_MAX];
    uint16_t  smooth_offsets[SMOOTH_MAX];
} song_t;

void wts_init();
bool wts_is_full();
bool wts_is_empty();
bool wts_is_continue();
uint8_t wts_get_value();
void wts_cook_data();

#endif /*WAVETABLE_SYNTH_H_*/
