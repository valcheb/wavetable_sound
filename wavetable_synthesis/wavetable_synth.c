#include "wavetable_synth.h"

//ring buffer interface

typedef struct ring
{
    uint8_t *buffer;
    uint8_t in;
    uint8_t out;
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
    if (rbuf->current_size >= rbuf->size)
        return true;
    return false;
}

inline static bool ring_is_empty(ring_t *rbuf)
{
    if (rbuf->current_size == 0)
        return true;
    return false;
}

//wavetable and raw interface
#define COOK_SIZE 32
static uint8_t  cooked_data[COOK_SIZE];
static ring_t   data_ring;
static song_t song_st;

/*******/
/*common*/
inline static uint8_t wts_parse_value(uint16_t byte, uint16_t mask, uint8_t offset)
{
    return (uint8_t)
    (
        (byte>>offset) & mask
    );
;}

inline static bool wts_is_note_byte(uint16_t byte)
{
    if ( ((byte>>TYPE_OS) & TYPE_MASK) != 1 ) //MSB mask
        return true;
    return false;
}

/*synth*/
volatile static int a = 0;
inline static uint8_t wts_linear_interpole(uint16_t *arr, uint32_t i)
{
    uint32_t floor = i/100;
    uint32_t ceil = floor+1;
    int top = arr[ceil];
    int bottom = arr[floor];

    if (floor >= 252)
    {
        a++;
    }

    return (uint8_t)
    (
        (top - bottom) * (int)(i - floor*ACCURACY)/ACCURACY + bottom
    );
}

/*inline static bool proto_is_cook_continue()
{
    //готовить, если есть ноты для готовки
}*/

inline static uint32_t wts_calculate_duration(uint8_t dur, uint8_t bpm, uint8_t rate_i)
{
    return (uint32_t)rate[rate_i]*KILO*DURATION_BASE/bpm/durations[dur]; //bmp and rate mb global static
}

inline static uint32_t wts_calculate_increment(uint8_t note, uint16_t wave_len, uint8_t rate_i)
{
    return (uint32_t)wave_len*note_freq[note]/rate[rate_i]/KILO;
}

inline static void wts_init_song(song_t *song_st, uint16_t *song)
{
    uint16_t temp = song[0]; // header size and rate
    song_st->header_size = wts_parse_value(temp, HEADER_SIZE_MASK, HEADER_SIZE_OS);
    song_st->bpm = wts_parse_value(temp, BPM_MASK, BPM_OS);
    temp = song[1]; // rate and channel amount
    song_st->rate = wts_parse_value(temp, RATE_MASK, RATE_OS);
    song_st->chan_number = wts_parse_value(temp, CHAN_N_MASK, CHAN_N_OS)+1;
    song_st->wave_number = wts_parse_value(temp,WAVE_N_MASK,WAVE_N_OS);
    song_st->current_chan = 0;

    /**/
    uint16_t temp_offset = 2;
    for (int i = 0; i < song_st->chan_number; i++)
    {
        song_st->data_sizes[i] = song[temp_offset+i];
    }

    song_st->channel_offsets[0] = temp_offset+song_st->chan_number;
    for (int i = 1; i < song_st->chan_number; i++)
    {
        song_st->channel_offsets[i] = song_st->channel_offsets[0] + song_st->data_sizes[i-1];
    }

    /**/
    temp_offset = song_st->channel_offsets[song_st->chan_number-1] + song_st->data_sizes[song_st->chan_number-1];
    for (int i = 0; i < song_st->wave_number; i++)
    {
        song_st->wave_sizes[i] = song[temp_offset+i];
    }

    song_st->wave_offsets[0] = temp_offset + song_st->wave_number;
    for (int i = 1; i < song_st->wave_number; i++)
    {
        song_st->wave_offsets[i] = song_st->wave_offsets[0] + song_st->wave_sizes[i-1];
    }

    /**/
    for (int i = 0; i < song_st->chan_number; i++)
    {
        song_st->channels[i].current_idx = song_st->channel_offsets[i];
        song_st->channels[i].note_len = 0;
        song_st->channels[i].current_phase = 0;
    }

    song_st->song_len = 0;
    for (int i = 0; i < song_st->chan_number; i++)
    {
        uint8_t current_offset = song_st->channel_offsets[i];
        uint16_t current_size = song_st->data_sizes[i];
        while(current_size != 0) //TODO optimize increments like this: current_offset < song_st->channel_offsets[j] + song_st->data_sizes[j]
        {
            temp = song[current_offset++];
            if (wts_is_note_byte(temp))
                song_st->song_len += wts_calculate_duration(wts_parse_value(temp,DURATION_MASK,DURATION_OS),song_st->bpm,song_st->rate);
            current_size--;
        }
    }
}

/*service*/
void wts_init()
{
    ring_init(&data_ring,cooked_data,COOK_SIZE);
    wts_init_song(&song_st,song);
}

bool wts_is_full()
{
    return ring_is_full(&data_ring);
}

bool wts_is_empty()
{
    return ring_is_empty(&data_ring);
}

bool wts_is_continue()
{
    if (song_st.song_len > 0)
        return true;
    return false;
}

uint8_t wts_get_value()
{
    song_st.song_len--;
    return ring_pop(&data_ring);
}

inline static void wts_cook_channel(channel_t *channel)
{
    if (channel->note_len != 0)
    {
        uint8_t interpolated_value = wts_linear_interpole(&song[song_st.wave_offsets[channel->wavetable]],channel->current_phase);
        uint32_t vol = (channel->volume*ACCURACY/255);
        uint8_t calculated_value = (uint8_t)((interpolated_value * vol)/ACCURACY); /* * channels[num].current_smooth*/

        ring_put(&data_ring,calculated_value);

        channel->current_phase += channel->phase_increment;
        if (channel->current_phase >= (channel->wave_len-1)*ACCURACY )
            channel->current_phase -= (channel->wave_len-1)*ACCURACY;
        channel->note_len--;
    }
    else
    {
        uint16_t temp = song[channel->current_idx];
        //song_len_in_notes--;
        if (wts_is_note_byte(temp))
        {
            channel->phase_increment = wts_calculate_increment(wts_parse_value(temp,NOTE_MASK,NOTE_OS),channel->wave_len,song_st.rate);
            channel->note_len = wts_calculate_duration(wts_parse_value(temp,DURATION_MASK,DURATION_OS),song_st.bpm,song_st.rate);
            channel->current_phase = 0;
        }
        else
        {
            channel->wavetable = wts_parse_value(temp,WAVE_MASK,WAVE_OS);
            channel->wave_len = song_st.wave_sizes[channel->wavetable];
            channel->volume = wts_parse_value(temp,VOLUME_MASK,VOLUME_OS);
            channel->smooth = wts_parse_value(temp,SMOOTH_MASK,SMOOTH_OS);
        }
        channel->current_idx++;
    }
}

void wts_cook_data()
{
    wts_cook_channel(&song_st.channels[song_st.current_chan]);
    song_st.current_chan++;
    if (song_st.current_chan >= song_st.chan_number)
        song_st.current_chan = 0;
}
