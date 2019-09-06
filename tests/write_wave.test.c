#include <stdlib.h>
#include <math.h>

#include "write_wave.h"
#include "wave.h"

int16_t* create_signal(const double a[3][2], uint32_t sample_array_size, uint32_t sample_rate){
    int16_t* sample_signal = malloc(sizeof(int16_t) * sample_array_size);
    for(size_t i = 0; i < sample_array_size; i++){
        int16_t sum = 0;
        for(size_t j = 0; j < 3; j++){
            sum += a[j][1] * sin((double) 2 * M_PI * a[j][0] * i / sample_rate);
        }
        sample_signal[i] = sum;
    }
    return sample_signal;
}

int main(void){

    float length = 1;
    uint32_t sample_rate = 44100;

    Wave test_wave;

    uint32_t sample_array_size = length * sample_rate;

    double a[3][2] = {
        {440, pow(2, 15)},
        {1000, 0},
        {2000, 0}
    };

    int16_t* sample_array = create_signal(a, sample_array_size, sample_rate);

    if(sample_array){
        test_wave = make_mono_wave(sample_array, sample_array_size, sample_rate, sizeof(int16_t)*8);
    } else {
        printf("Sample Array malloc() error. Exiting\n");
        return 1;
    }

    char filename[] = "test_wave.wav";

    for(size_t idx = 0; idx < sample_array_size; idx++){
        uint16_t test_wave_val = (test_wave.data[(2 * idx) + 1] << 8) + test_wave.data[(2 * idx)];
        //printf("%i ", test_wave_val);
        //printf("%i\n", sample_array[idx]);
    }

    if(true){
        printf("{\n");
        printf("    \"%.4s\",\n", test_wave.header.chunk_id);
        printf("    %i,\n", test_wave.header.chunk_size);
        printf("    \"%.4s\",\n", test_wave.header.format);

        printf("    \"%.4s\",\n", test_wave.header.format_subchunk_id);
        printf("    %i,\n", test_wave.header.format_subchunk_size);
        printf("    %i,\n", test_wave.header.audio_format);
        printf("    %i,\n", test_wave.header.numberof_channels);
        printf("    %i,\n", test_wave.header.sample_rate);
        printf("    %i,\n", test_wave.header.byte_rate);
        printf("    %i,\n", test_wave.header.block_align);
        printf("    %i,\n", test_wave.header.bits_per_sample);

        printf("    \"%.4s\",\n", test_wave.header.data_subchunk_id);
        printf("    %i\n", test_wave.header.data_subchunk_size);
        printf("}\n\n");

        printf("{\n");
        printf("    %i,\n", test_wave.data_size);
        printf("    %i,\n", test_wave.numberof_frames);
        printf("}\n");
    }

    if(test_wave.data){
        write_wave(&test_wave, filename);
        remove_frame_data(&test_wave);
        free(sample_array);
    } else {
        printf("Test wave data malloc() error. Exiting\n");
        free(sample_array);
        return 1;
    }

    return 0;
}
