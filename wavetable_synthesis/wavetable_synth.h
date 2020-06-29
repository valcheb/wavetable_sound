#ifndef WAVETABLE_SYNTH_H_
#define WAVETABLE_SYNTH_H_

#include "stdbool.h"
#include "stdint.h"

#include "wavetable_dictionary.h"
#include "song.h"

typedef struct
{
    uint16_t current_idx; //position in data song array
    uint32_t    phase_increment;
    uint32_t note_len;
    uint32_t    current_phase;
    uint16_t wave_len;
    uint8_t  wavetable; //number of wavetable
    uint8_t  volume;
    uint8_t  smooth;
    uint8_t  smooth_increment;
    uint8_t  current_smooth;
} channel_t;

typedef struct
{
    /*header*/
    uint8_t   header_size;
    uint8_t   bpm;
    uint8_t   rate;
    uint8_t   chan_number;
    uint8_t   wave_number;
    /*song info*/
    uint8_t   current_chan;
    uint16_t  data_sizes[CHANNEL_MAX];
    uint16_t  channel_offsets[CHANNEL_MAX];
    uint16_t  wave_sizes[WAVE_MAX];
    uint16_t  wave_offsets[WAVE_MAX];
    channel_t channels[CHANNEL_MAX];
    /**/
    uint32_t  song_len;
} song_t;

void wts_init();
bool wts_is_full();
bool wts_is_empty();
bool wts_is_continue();
uint8_t wts_get_value();
void wts_cook_data();

#endif /*WAVETABLE_SYNTH_H_*/
