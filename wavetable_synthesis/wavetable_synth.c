#include "wavetable_synth.h"
#include "ring.h"

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
    return ((byte>>TYPE_OS) & TYPE_MASK) != 1;
}

/*synth*/
inline static uint8_t wts_linear_interpole(uint16_t *arr, uint32_t i)
{
    uint32_t floor = i;
    uint32_t ceil = floor+ACCURACY;
    uint8_t top = arr[ceil/ACCURACY];
    uint8_t bottom = arr[floor/ACCURACY];

    return (uint8_t)
    (
        (top - bottom) * (i - floor)/ACCURACY + bottom
    );
}

/*inline static bool proto_is_cook_continue()
{
    //готовить, если есть ноты для готовки
}*/

inline static uint32_t wts_calculate_duration(uint8_t dur, uint8_t dur_m, uint8_t bpm, uint8_t rate_i, uint8_t chan)
{
    uint32_t d = rate[rate_i]*KILO/chan*DURATION_BASE/bpm/durations[dur];
    switch(dur_m)
    {
        default:
        case DURATION_SIMPLE:
            return d;
        case DURATION_POINT:
            return 3*d/2;
    }
}

inline static uint32_t wts_calculate_increment(uint8_t note, uint16_t wave_len, uint8_t rate_i, uint8_t chan)
{
    return (uint32_t)wave_len*note_freq[note]/(rate[rate_i]*KILO/chan);
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
        song_st->channels[i].note_len = 0;
        song_st->channels[i].current_phase = 0;
        song_st->channels[i].data_size = song[temp_offset+i];

        if (i == 0)
        {
            song_st->channels[0].offset = temp_offset+song_st->chan_number;
        }
        else
        {
            song_st->channels[i].offset = song_st->channels[0].offset + song_st->channels[i-1].data_size; // [i-1].offset+[i-1].datasize?
        }

        song_st->channels[i].current_idx = song_st->channels[i].offset;
    }

    /**/
    temp_offset = song_st->channels[song_st->chan_number-1].offset + song_st->channels[song_st->chan_number-1].data_size;
    for (int i = 0; i < song_st->wave_number; i++)
    {
        song_st->wave_sizes[i] = song[temp_offset+i];
    }

    song_st->wave_offsets[0] = temp_offset + song_st->wave_number;
    for (int i = 1; i < song_st->wave_number; i++)
    {
        song_st->wave_offsets[i] = song_st->wave_offsets[i-1] + song_st->wave_sizes[i-1];
    }

    /**/
    song_st->song_len = 0;
    for (int i = 0; i < song_st->chan_number; i++)
    {
        uint16_t current_offset = song_st->channels[i].offset;
        uint16_t current_size = song_st->channels[i].data_size;

        for (uint16_t i = current_offset; i < current_offset + current_size; i++)
        {
            temp = song[i];
            if (wts_is_note_byte(temp))
                song_st->song_len += wts_calculate_duration(wts_parse_value(temp,DURATION_MASK,DURATION_OS),
                                                            wts_parse_value(temp,DURATION_P_MASK,DURATION_P_OS),
                                                            song_st->bpm,song_st->rate,
                                                            song_st->chan_number);
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
    return song_st.song_len > 0;
}

uint8_t wts_get_value()
{
    song_st.song_len--;
    return ring_pop(&data_ring);
}

inline static uint8_t wts_synth(uint16_t *wave, uint32_t phase, uint32_t volume, uint32_t smooth)
{
    uint8_t interpolated_value = wts_linear_interpole(wave, phase);
    uint32_t vol = volume * 1000 / 256; //TODO improve volume calculation
    return (uint8_t)
    (
        interpolated_value * vol / 1000 // * smooth
    );
}

inline static void wts_prepare_note(channel_t *channel, uint16_t note_byte, song_t *song)
{
    channel->phase_increment = wts_calculate_increment(wts_parse_value(note_byte,NOTE_MASK,NOTE_OS),
                                                       channel->wave_len,
                                                       song->rate,
                                                       song->chan_number);
    channel->note_len = wts_calculate_duration(wts_parse_value(note_byte,DURATION_MASK,DURATION_OS),
                                               wts_parse_value(note_byte,DURATION_P_MASK,DURATION_P_OS),
                                               song->bpm,
                                               song->rate,
                                               song->chan_number);
    channel->current_phase = 0;
}

inline static void wts_prepare_channel(channel_t *channel, uint16_t channel_byte, song_t *song)
{
    channel->wavetable = wts_parse_value(channel_byte,WAVE_MASK,WAVE_OS);
    channel->wave_len = song->wave_sizes[channel->wavetable];
    channel->volume = wts_parse_value(channel_byte,VOLUME_MASK,VOLUME_OS);
    channel->smooth = wts_parse_value(channel_byte,SMOOTH_MASK,SMOOTH_OS);
    channel->current_smooth = 0;
}

inline static void wts_cook_channel(channel_t *channel)
{
    if ( channel->note_len != 0 )
    {
        uint8_t synth_value = wts_synth(&song[song_st.wave_offsets[channel->wavetable]],
                                        channel->current_phase,
                                        channel->volume,
                                        channel->current_smooth);

        ring_put(&data_ring,synth_value);

        channel->current_phase += channel->phase_increment;
        if (channel->current_phase >= (channel->wave_len-1)*ACCURACY)
            channel->current_phase -= (channel->wave_len-1)*ACCURACY;

        channel->note_len--;
    }
    else if ( channel->current_idx < (channel->offset + channel->data_size) )
    {
        uint16_t temp = song[channel->current_idx];

        if (wts_is_note_byte(temp))
        {
            wts_prepare_note(channel, temp, &song_st);
        }
        else
        {
            wts_prepare_channel(channel, temp, &song_st);
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
