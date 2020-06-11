#ifndef WAVETABLE_SYNTH_H_
#define WAVETABLE_SYNTH_H_

#include "stdbool.h"
#include "math.h"
#include "stdint.h"

//ring buffer interface
typedef struct ring
{
    uint8_t *buffer;
    uint8_t in;
    uint8_t out;
    uint16_t size;
    uint16_t current_size;
} ring_t;

void ring_init(ring_t *rbuf, uint8_t *src_buf, uint16_t size);
void ring_put(ring_t *rbuf, uint8_t data);
uint8_t ring_pop(ring_t *rbuf);
bool ring_is_full(ring_t *rbuf);
bool ring_is_empty(ring_t *rbuf);

//synthesis interface
void wts_init();
bool wst_is_full();
bool wst_is_empty();
bool wst_is_continue();
uint8_t wts_get_value();
void wts_cook_data();

void debug_wst_reset_song();

#endif /*WAVETABLE_SYNTH_H_*/
