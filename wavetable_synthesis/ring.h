#ifndef RING_H_
#define RING_H_

#include "stdint.h"

typedef struct ring
{
    uint8_t  *buffer;
    uint8_t  in;
    uint8_t  out;
    uint16_t size;
    uint16_t current_size;
} ring_t;

inline static void ring_init(ring_t *rbuf, uint8_t *src_buf, uint16_t size)
{
    rbuf->buffer = src_buf;
    rbuf->in = 0;
    rbuf->out = 0;
    rbuf->size = size;
    rbuf->current_size = 0;
}

inline static void ring_put(ring_t *rbuf, uint8_t data)
{
    rbuf->buffer[rbuf->in++] = data;
    if (rbuf->in >= rbuf->size)
        rbuf->in = 0;
    ++rbuf->current_size;
}

inline static uint8_t ring_pop(ring_t *rbuf)
{
    uint8_t res = rbuf->buffer[rbuf->out++];
    if (rbuf->out >= rbuf->size)
        rbuf->out = 0;
    --rbuf->current_size;
    return res;
}

inline static bool ring_is_full(ring_t *rbuf)
{
    return rbuf->current_size >= rbuf->size;
}

inline static bool ring_is_empty(ring_t *rbuf)
{
    return rbuf->current_size == 0;
}

#endif /*RING_H_*/
