#ifndef WAVE_H
    #define WAVE_H

    #include <stdlib.h>
    #include <stdint.h>
    #include <stdio.h>
    #include <math.h>

    #include "wave_header.h"
    #include "endianness.h"

    /*
    Wave is a struct that prepares a sample for writing to a wav file.

    Only the header and data are written to the file, however the other fields are
    useful for writing the sample into the data field.
    */
    typedef struct Wave {

        WaveHeader header;

        char* data;
        
        uint32_t data_size;
        uint32_t numberof_samples;
        uint16_t bytes_per_sample;

    } Wave;

    // Method to return a Wave instance that is ready to be written to a wav file. For simplicity it has only one channel.
    Wave make_wave(const int32_t* sample_array, uint32_t numberof_samples, uint16_t numberof_channels, uint32_t sample_rate, uint16_t bits_per_sample);

    // Method to deallocate the memory used to hold the sample in data while the file is written to the wav file.
    void remove_sample_data(Wave* wave);

    // Method to print all fields of the wave struct in a json format.
    void print_metadata(Wave* wave);

#endif
