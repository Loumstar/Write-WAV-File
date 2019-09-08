#ifndef WRITE_WAVE_H
    #define WRITE_WAVE_H

    #include <stdint.h>

    #include "wave.h"
    #include "endianness.h"

    // Method to write a Wave instance to a file.
    void write_wave(Wave* wave, const char* filename);

    // Method to take an array of samples and create a wav file so it can be played as audio.
    void write_array_to_wav_file(const char* filename, const int16_t* sample_array, uint32_t sample_array_size, uint16_t numberof_channels, uint32_t sample_rate, uint16_t bits_per_sample);

#endif
