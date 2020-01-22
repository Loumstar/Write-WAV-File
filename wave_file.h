#ifndef WAVE_FILE_H
    #define WAVE_FILE_H

    #include <stdbool.h>
    #include <stdint.h>
    #include <string.h>
    #include <stdlib.h>
    #include <stdio.h>
    #include <math.h>

    #include "scripts/wave_header.h"
    #include "scripts/endianness.h"

    /*
    wave is a struct that prepares a sample for writing to a wav file.

    Only the header and data are written to the file, however the other fields are
    useful for writing the sample into the data field.
    */

    typedef struct wave {

        wave_header header;

        char* data;

        uint32_t numberof_samples;
        uint16_t bytes_per_sample;

        bool __is_valid;

    } wave;

    // Method to return an empty wave. All values except for the format_subchunk_size and audio_format fields are set to zero.
    wave blank_wave();

    // Method to return a wave instance that is ready to be written to a wav file.
    wave create_wave(const int32_t* sample_array, uint32_t numberof_samples, uint16_t numberof_channels, uint32_t sample_rate, uint16_t bits_per_sample);

    // Method to deallocate the memory used to hold the sample in data while in a w type.
    void deallocate_samples(wave* w);

    // Method to return whether the wave is fully defined and has a pointer for reading the oscillation data
    bool wave_is_valid(const wave* w);

    // Method to print all fields of the w struct in a json format.
    void print_metadata(const wave* w);


    // Method to return a wave instance read from a wav file.
    wave read_wav_file(const char* filename);

    // Method to return an array of the waveform read from a wav file.
    void wave_data_to_array(const wave* w, int32_t* sample_array);


    // Method to write a wave instance to a file.
    void write_wav_file(const char* filename, wave* w);

    // Method to take an array of samples and create a wav file so it can be played as audio.
    void write_array_to_wav_file(const char* filename, const int32_t* sample_array, uint32_t sample_array_size, uint16_t numberof_channels, uint32_t sample_rate, uint16_t bits_per_sample);

#endif
