#ifndef WAVE_FILE_H
    #define WAVE_FILE_H

    #include <stdint.h>
    #include <string.h>

    #include "wave_header.h"
    #include "wave.h"
    #include "endianness.h"

    // Method to write a Wave instance to a file.
    void write_wave(const char* filename, Wave* wave);

    // Method to return a Wave instance read from a wav file.
    Wave read_wave(char* filename);

    // Method to take an array of samples and create a wav file so it can be played as audio.
    void write_array_to_wav_file(const char* filename, const int32_t* sample_array, uint32_t sample_array_size, uint16_t numberof_channels, uint32_t sample_rate, uint16_t bits_per_sample);

#endif
