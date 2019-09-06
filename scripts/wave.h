#ifndef WAVE_H
    #define WAVE_H

    #include <stdlib.h>
    #include <stdio.h>
    #include <math.h>

    #include "wave_header.h"
    #include "endianness.h"

    typedef struct Wave {

        WaveHeader header;

        char* data;
        
        uint32_t data_size;
        uint32_t numberof_frames;
        uint16_t bytes_per_sample;

    } Wave;

    Wave make_mono_wave(const int16_t* sample_array, uint32_t numberof_frames, uint32_t sample_rate, uint16_t bits_per_sample);

    void remove_frame_data(Wave* wave);

#endif
