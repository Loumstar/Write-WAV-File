#ifndef WRITE_WAVE_H
    #define WRITE_WAVE_H

    #include <stdint.h>
    #include <string.h>

    #include "wave_header.h"
    #include "wave.h"
    #include "endianness.h"

    // Method to write a Wave instance to a file.
    void write_wave(Wave* wave, const char* filename);

    // Method to take an array of samples and create a wav file so it can be played as audio.
    void write_array_to_wav_file(const char* filename, const int32_t* sample_array, uint32_t sample_array_size, uint16_t numberof_channels, uint32_t sample_rate, uint16_t bits_per_sample);

    // Method to return a Wave instance read from a wav file.
    Wave read_wave(char* filename);

    // Method to return an array of the waveform read from a wav file.
    void write_wave_data_to_array(int32_t* sample_array, Wave* wave);

#endif
