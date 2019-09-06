#ifndef WAVE_HEADER_H
    #define WAVE_HEADER_H

    #include <stdint.h>

    typedef struct WaveHeader {
        
        // Riff Wave Header
        char chunk_id[4];

        uint32_t  chunk_size;

        char format[4];

        // Format Subchunk
        char format_subchunk_id[4];

        uint32_t format_subchunk_size;

        uint16_t audio_format;
        uint16_t numberof_channels;

        uint32_t sample_rate;
        uint32_t byte_rate;

        uint16_t block_align;
        uint16_t bits_per_sample;

        // Data Subchunk
        char data_subchunk_id[4];

        uint32_t  data_subchunk_size;

    } WaveHeader;

    WaveHeader make_header(uint32_t sample_rate, uint16_t numberof_channels, uint16_t bits_per_sample);

#endif
