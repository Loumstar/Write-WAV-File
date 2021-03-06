#include <stdlib.h>
#include <math.h>
#include <time.h>

#include "../wave_file.h"

void create_signal(int32_t sample_array[], const double freqs[3][2], uint32_t sample_array_size, uint32_t sample_rate){
    for(size_t i = 0; i < sample_array_size; i++){
        int32_t sum = 0;
        for(size_t j = 0; j < 3; j++){
            sum += freqs[j][1] * sin((double) 2 * M_PI * freqs[j][0] * i / sample_rate);
        }
        sample_array[i] = sum;
    }
}

int main(void){
    size_t start, end;

    wave wave;

    float length = 10.0;

    uint32_t sample_rate = 44100;
    uint16_t bits_per_sample = 16;
    uint16_t numberof_channels = 2;

    uint32_t sample_array_size = length * sample_rate;

    char filename[] = "a_major.wav";

    double freqs[3][2] = {
        {440.00, pow(2, bits_per_sample - 3)}, // A
        {554.36, pow(2, bits_per_sample - 3)}, // C#
        {659.26, pow(2, bits_per_sample - 3)}  // E
    };

    uint32_t max_amplitude = freqs[0][1] + freqs[1][1] + freqs[2][1];
    
    if(max_amplitude > pow(2, bits_per_sample)){
        printf("Buffer Overflow error. Exiting.\n");
        return 1;
    }

    int32_t* sample_array = malloc(sizeof(int32_t) * sample_array_size);

    if(sample_array){
        start = clock();
        create_signal(sample_array, freqs, sample_array_size, sample_rate);
        end = clock();

        printf("Sample created in %.3f ms\n", ((double) end - start) / CLOCKS_PER_SEC);

        wave = create_wave(sample_array, sample_array_size, numberof_channels, sample_rate, bits_per_sample);
    } else {
        printf("Sample Array malloc() error. Exiting.\n");
        return 1;
    }

    if(wave_is_valid(&wave)){
        printf("Metadata for the wave file:\n");
        print_metadata(&wave);
        
        printf("Writing %s.\n", filename);

        start = clock();
        write_wav_file(filename, &wave);
        end = clock();

        printf("Wave data written to file in %.3f ms\n", ((double) end - start) / CLOCKS_PER_SEC);

        deallocate_samples(&wave);
        free(sample_array);
    } else {
        printf("Test wave data malloc() error. Exiting.\n");
        free(sample_array);
        return 1;
    }

    return 0;
}
