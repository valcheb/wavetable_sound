#include "string.h"

#include "wavetable_synth.h"
#include "ring.h"

#define COOK_SIZE 32
static uint8_t cooked_data[COOK_SIZE];
static ring_t  data_ring;
static song_t  song_st;

inline static uint8_t wts_parse_value(uint16_t byte, uint16_t mask, uint8_t offset)
{
    return (uint8_t)
    (
        (byte >> offset) & mask
    );
}

inline static bool wts_is_note_byte(uint16_t byte)
{
    return ((byte >> TYPE_OS) & TYPE_MASK) != 1;
}

inline static uint8_t wts_linear_interpole(uint16_t *arr, uint32_t i)
{
    uint32_t floor = i;
    uint32_t ceil = floor + ACCURACY;
    uint8_t top = arr[ceil / ACCURACY];
    uint8_t bottom = arr[floor / ACCURACY];

    return (uint8_t)
    (
        (top - bottom) * (i - floor) / ACCURACY + bottom
    );
}

inline static uint32_t wts_calculate_duration(uint8_t dur, uint8_t dur_m, uint8_t bpm, uint8_t rate_i, uint8_t chan)
{
    uint32_t d = rate[rate_i] * KILO /*/ chan*/ * DURATION_BASE / bpm / durations[dur];
    switch(dur_m)
    {
        default:
        case DURATION_SIMPLE:
            return d;
        case DURATION_POINT:
            return 3 * d / 2;
    }
}

inline static uint32_t wts_calculate_increment(uint8_t note, uint16_t wave_len, uint8_t rate_i, uint8_t chan)
{
    return (uint32_t)wave_len * note_freq[note] / rate[rate_i] / KILO /** chan*/;
}

inline static void wts_fill_offsets(uint16_t *dest, uint16_t *sizes, uint8_t count, uint32_t start_offset)
{
    dest[0] = start_offset + count;
    for (int i = 1; i < count; i++)
    {
        dest[i] = dest[i-1] + sizes[i-1];
    }
}

inline static void wts_calculate_song_len(song_t *song_st, uint16_t *song)
{
    song_st->song_len = 0;
    for (int i = 0; i < song_st->chan_number; i++)
    {
        uint16_t current_offset = song_st->channels[i].data.offset;
        uint16_t current_size = song_st->channels[i].data.size;

        for (uint16_t i = current_offset; i < current_offset + current_size; i++)
        {
            uint16_t temp = song[i];
            if (wts_is_note_byte(temp))
                song_st->song_len += wts_calculate_duration(wts_parse_value(temp, DURATION_MASK, DURATION_OS),
                                                            wts_parse_value(temp, DURATION_P_MASK, DURATION_P_OS),
                                                            song_st->bpm,
                                                            song_st->rate,
                                                            song_st->chan_number);
        }
    }
}

inline static void wts_init_song(song_t *song_st, uint16_t *song)
{
    song_st->song = song;
    /*header*/
    uint16_t temp = song[0];
    song_st->header_size = wts_parse_value(temp, HEADER_SIZE_MASK, HEADER_SIZE_OS);
    song_st->bpm = wts_parse_value(temp, BPM_MASK, BPM_OS);
    temp = song[1];
    song_st->rate = wts_parse_value(temp, RATE_MASK, RATE_OS);
    song_st->chan_number = wts_parse_value(temp, CHAN_N_MASK, CHAN_N_OS) + 1;
    song_st->wave_number = wts_parse_value(temp, WAVE_N_MASK, WAVE_N_OS);
    song_st->smooth_number = wts_parse_value(temp, SMOOTH_N_MASK, SMOOTH_N_OS);
    song_st->current_chan = 0;

    /*data_offsets*/
    uint16_t temp_offset = 2;
    for (int i = 0; i < song_st->chan_number; i++)
    {
        song_st->channels[i].note.length = 0;
        song_st->channels[i].note.phase = 0;
        song_st->channels[i].data.size = song[temp_offset + i];

        if (i == 0)
        {
            song_st->channels[0].data.offset = temp_offset + song_st->chan_number;
        }
        else
        {
            song_st->channels[i].data.offset = song_st->channels[i-1].data.offset + song_st->channels[i-1].data.size;
        }

        song_st->channels[i].data.index = song_st->channels[i].data.offset;
    }

    /*wave_offsets*/
    temp_offset = song_st->channels[song_st->chan_number-1].data.offset + song_st->channels[song_st->chan_number-1].data.size;
    memcpy(song_st->wave_sizes, song+temp_offset, song_st->wave_number * sizeof(song_st->wave_sizes[0]));
    wts_fill_offsets(song_st->wave_offsets, song_st->wave_sizes, song_st->wave_number, temp_offset);

    /*smooth_offsets*/
    temp_offset = song_st->wave_offsets[song_st->wave_number-1] + song_st->wave_sizes[song_st->wave_number-1];
    memcpy(song_st->smooth_sizes, song + temp_offset, song_st->smooth_number * sizeof(song_st->smooth_sizes[0]));
    wts_fill_offsets(song_st->smooth_offsets, song_st->smooth_sizes, song_st->smooth_number, temp_offset);

    /*song_len*/
    wts_calculate_song_len(song_st, song);
}

inline static uint8_t wts_synth(uint16_t *wave, uint32_t phase, uint16_t *smooth, uint32_t smooth_i, uint32_t volume)
{
    uint8_t interpolated_wave = wts_linear_interpole(wave, phase);
    uint32_t interpolated_smooth = smooth[smooth_i] * 1000 / 256;
    uint32_t vol = volume * 1000 / 256; //TODO improve volume calculation
    return (uint8_t)
    (
        interpolated_wave * interpolated_smooth * vol / 1000 / 1000
    );
}

inline static void wts_prepare_note(channel_t *channel, uint16_t note_byte, song_t *song)
{
    channel->note.phase_increment = wts_calculate_increment(wts_parse_value(note_byte, NOTE_MASK, NOTE_OS),
                                                            channel->wave.length,
                                                            song->rate,
                                                            song->chan_number);
    channel->note.length = wts_calculate_duration(wts_parse_value(note_byte, DURATION_MASK, DURATION_OS),
                                                  wts_parse_value(note_byte, DURATION_P_MASK, DURATION_P_OS),
                                                  song->bpm,
                                                  song->rate,
                                                  song->chan_number);
    channel->note.phase = 0;

    channel->smooth.counter = 0;
    channel->smooth.step = channel->note.length / song->smooth_sizes[channel->smooth.number];
    channel->smooth.phase = 0;
}

inline static void wts_prepare_channel(channel_t *channel, uint16_t channel_byte, song_t *song)
{
    channel->wave.number = wts_parse_value(channel_byte, WAVE_MASK, WAVE_OS);
    channel->wave.length = song->wave_sizes[channel->wave.number];
    channel->volume = wts_parse_value(channel_byte, VOLUME_MASK, VOLUME_OS);
    channel->smooth.number = wts_parse_value(channel_byte, SMOOTH_MASK, SMOOTH_OS);
}

inline static void wts_note_phase_routine(note_t *note, uint32_t limit)
{
    note->phase += note->phase_increment;
    if (note->phase >= limit)
        note->phase -= limit;
}

inline static void wts_smooth_phase_routine(smooth_t *smooth)
{
    smooth->counter++;
    if (smooth->counter > smooth->step)
    {
        smooth->phase++;
        smooth->counter = 0;
    }
}

inline static uint8_t wts_cook_channel(channel_t *channel)
{
    uint8_t synth_value = wts_synth(&song_st.song[song_st.wave_offsets[channel->wave.number]],
                                        channel->note.phase,
                                        &song_st.song[song_st.smooth_offsets[channel->smooth.number]],
                                        channel->smooth.phase,
                                        channel->volume);

    wts_smooth_phase_routine(&channel->smooth);
    wts_note_phase_routine(&channel->note, (channel->wave.length-1)*ACCURACY);
    channel->note.length--;

    return synth_value;
}

void wts_init(uint16_t *song)
{
    ring_init(&data_ring, cooked_data, COOK_SIZE);
    wts_init_song(&song_st, song);
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
    return song_st.song_len > 0;
}

uint8_t wts_get_value()
{
    song_st.song_len -= song_st.chan_number;
    return ring_pop(&data_ring);
}

inline static void wts_mix_channels_headroom(uint8_t data, uint8_t *cur_chan, uint8_t chan_num)
{
    static uint16_t sum = 0;
    sum += data;
    (*cur_chan)++;

    if ( (*cur_chan) >= chan_num)
    {
        ring_put(&data_ring, (uint8_t)(sum / chan_num));
        sum = 0;
        *cur_chan = 0;
    }
}

/*inline static uint16_t wts_heron_square(uint32_t a)
{
    uint16_t x = ACCURACY;
    a *= ACCURACY * ACCURACY;

    for (int i = 0; i < 10; i++)
    {
        uint16_t xnew = (uint16_t)(x + a / x) / 2;

        if (xnew == x)
            return xnew;
        else
            x = xnew;
    }

    return x;
}

inline static void mix_channels_rms(uint8_t data, uint8_t *cur_chan, uint8_t chan_num)
{
    static uint32_t sum = 0;
    sum += data * data;
    (*cur_chan)++;

    if ( (*cur_chan) >= chan_num)
    {
        ring_put(&data_ring, (uint8_t)(wts_heron_square(sum / chan_num) / ACCURACY) );
        sum = 0;
        *cur_chan = 0;
    }
}*/

inline static bool wts_is_note_end(note_t *note)
{
    return note->length == 0;
}

inline static bool wts_is_channel_data_end(channel_t *channel)
{
    return channel->data.index == (channel->data.offset + channel->data.size);
}

void wts_cook_data()
{
    channel_t *channel = &song_st.channels[song_st.current_chan];
    uint8_t synth_value = 0;

    if (!wts_is_note_end(&channel->note))
    {
        synth_value = wts_cook_channel(channel);
    }
    else if (!wts_is_channel_data_end(channel))
    {
        uint16_t temp = song_st.song[channel->data.index];

        if (wts_is_note_byte(temp))
        {
            wts_prepare_note(channel, temp, &song_st);
        }
        else
        {
            wts_prepare_channel(channel, temp, &song_st);
        }

        channel->data.index++;
    }

    wts_mix_channels_headroom(synth_value, &song_st.current_chan, song_st.chan_number);
}
