#ifndef WAVETABLE_SYNTH_H_
#define WAVETABLE_SYNTH_H_

#include "stdbool.h"
#include "stdint.h"

#include "wavetable_dictionary.h"
#include "song.h"

typedef struct
{
    /*note*/
    uint32_t current_phase;
    uint32_t phase_increment;
    uint32_t note_len;
    /*smooth*/
    uint32_t current_smooth;
    uint32_t smooth_counter;
    uint8_t  smooth_num;
    uint32_t smooth_step;
    /*wave*/
    uint16_t wave_len;
    uint8_t  wave_num;
    /*data*/
    uint8_t  volume;
    uint16_t data_idx;
    uint16_t data_size;
    uint16_t data_offset;
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
