#include <stdlib.h>
#include <math.h>

#include "write_wave.h"
#include "wave.h"

void create_signal(int16_t sample_array[], const double freqs[3][2], uint32_t sample_array_size, uint32_t sample_rate){
    for(size_t i = 0; i < sample_array_size; i++){
        int16_t sum = 0;
        for(size_t j = 0; j < 3; j++){
            sum += freqs[j][1] * sin((double) 2 * M_PI * freqs[j][0] * i / sample_rate);
        }
        sample_array[i] = sum;
    }
}

int main(void){
    Wave wave;

    char filename[] = "a_major.wav";

    double freqs[3][2] = {
        {440.00, pow(2, 13)},
        {659.26, pow(2, 13)},
        {554.36, pow(2, 13)}
    };

    float length = 10;

    uint32_t sample_rate = 44100;
    uint16_t numberof_channels = 2;

    uint32_t sample_array_size = length * sample_rate;

    uint32_t max_amplitude = freqs[0][1] + freqs[1][1] + freqs[2][1];
    
    if(max_amplitude > INT16_MAX){
        printf("Buffer Overflow error. Exiting.\n");
        return 1;
    }

    int16_t* sample_array = malloc(sizeof(int16_t) * sample_array_size);

    if(sample_array){
        create_signal(sample_array, freqs, sample_array_size, sample_rate);
        wave = make_wave(sample_array, sample_array_size, numberof_channels, sample_rate, 16);
    } else {
        printf("Sample Array malloc() error. Exiting.\n");
        return 1;
    }

    if(wave.data){
        print_metadata(&wave);
        printf("\nWriting %s.\n", filename);

        write_wave(&wave, filename);

        remove_sample_data(&wave);
        free(sample_array);
    } else {
        printf("Test wave data malloc() error. Exiting.\n");
        free(sample_array);
        return 1;
    }

    return 0;
}
