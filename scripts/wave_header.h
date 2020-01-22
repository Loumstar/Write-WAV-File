#ifndef WAVE_HEADER_H
    #define WAVE_HEADER_H

    #include <stdint.h>

    #include "endianness.h"

    /*
    wave_header is a struct that contains all the fields that must be part of the
    metadata for a wav file.
    These are declared in the order they must be arranged, with the appropriate sizes.
    */
   
    typedef struct wave_header {
        
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

    } wave_header;

    // Creates a wave_header instance and defines each field.
    wave_header make_header(uint32_t sample_rate, uint16_t numberof_channels, uint16_t bits_per_sample);

    // Method to return an empty wave_header. All values except for the format_subchunk_size and audio_format fields are set to zero.
    wave_header make_blank_header();

    // Method that ensures all fields of a wave instance are little endian.
    void make_wave_header_little_endian(wave_header* wh);

    // Method that ensures all fields of a wave instance are the same as the system's endianness.
    void make_wave_header_system_endian(wave_header* wh, char current_endianness);

#endif
