#include <stdlib.h>
#include <math.h>

#include "write_wave.h"
#include "wave.h"

int main(void){

    char read_filename[] = "a_major.wav";
    char write_filename[] = "a_major_copy.wav";

    Wave test_wave = read_wave(read_filename);

    printf("Data obtained from wav file:\n");
    print_metadata(&test_wave);

    uint32_t sample_array_size = test_wave.numberof_samples;
    uint16_t numberof_channels = test_wave.header.numberof_channels;
    uint32_t sample_rate = test_wave.header.sample_rate;
    uint16_t bits_per_sample = test_wave.header.bits_per_sample;

    int32_t* sample_array = malloc(sizeof(int32_t) * sample_array_size * numberof_channels);

    wave_data_to_array(sample_array, &test_wave);

    if(test_wave.data) remove_sample_data(&test_wave);

    if(sample_array){
        write_array_to_wav_file(write_filename, sample_array, sample_array_size, numberof_channels, sample_rate, bits_per_sample);
    } else {
        printf("Sample array malloc() error.\n");
        return 1;
    }

    free(sample_array);

    return 0;
}