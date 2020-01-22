#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "../wave_file.h"

int main(void){
    size_t start, end;

    char read_filename[] = "a_major.wav";
    char write_filename[] = "a_major_copy.wav";

    start = clock();
    wave test_wave = read_wav_file(read_filename);
    end = clock();

    printf("read_wave_metadata() took %.3f ms\n", ((double) end - start) / CLOCKS_PER_SEC);

    printf("Data obtained from wav file:\n");
    print_metadata(&test_wave);

    uint32_t sample_array_size = test_wave.numberof_samples;
    uint16_t numberof_channels = test_wave.header.numberof_channels;
    uint32_t sample_rate = test_wave.header.sample_rate;
    uint16_t bits_per_sample = test_wave.header.bits_per_sample;

    int32_t* sample_array = malloc(sizeof(int32_t) * sample_array_size * numberof_channels);

    start = clock();
    if(sample_array) wave_data_to_array(&test_wave, sample_array);
    end = clock();

    printf("Audio file length: %.3f s\n", (double) test_wave.numberof_samples / test_wave.header.sample_rate);
    printf("read_wave_to_array() took %.3f ms\n", ((double) end - start) * 1000 / CLOCKS_PER_SEC);

    if(wave_is_valid(&test_wave)) deallocate_samples(&test_wave);

    if(sample_array){
        printf("Writing %s\n", write_filename);
        write_array_to_wav_file(write_filename, sample_array, sample_array_size, numberof_channels, sample_rate, bits_per_sample);
        free(sample_array);
    } else {
        printf("Sample array malloc() error\n");
        return 1;
    }

    return 0;
}
