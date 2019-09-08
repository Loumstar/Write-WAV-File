#include <stdlib.h>
#include <math.h>

#include "write_wave.h"
#include "wave.h"

void create_signal(int16_t sample_array[], const double freq[3][2], uint32_t sample_array_size, uint32_t sample_rate){
    int16_t sum;
    for(size_t i = 0; i < sample_array_size; i++){
        sum = 0;
        for(size_t j = 0; j < 3; j++){
            sum += freq[j][1] * sin((double) 2 * M_PI * freq[j][0] * i / sample_rate);
        }
        sample_array[i] = sum;
    }
}

int main(void){
    Wave test_wave;

    float length = 10;
    uint32_t sample_rate = 44100;
    uint32_t sample_array_size = length * sample_rate;

    double test_freq[3][2] = {
        {440.00, pow(2, 14)},
        {659.26, pow(2, 14)},
        {554.36, pow(2, 12)}
    };

    int16_t* sample_array = malloc(sizeof(int16_t) * sample_array_size);

    if(sample_array){
        create_signal(sample_array, test_freq, sample_array_size, sample_rate);
        test_wave = make_wave(sample_array, 2, sample_array_size, sample_rate, sizeof(int16_t)*8);
    } else {
        printf("Sample Array malloc() error. Exiting\n");
        return 1;
    }

    char filename[] = "test_wave.wav";

    if(test_wave.data){
        print_metadata(&test_wave);
        write_wave(&test_wave, filename);

        remove_sample_data(&test_wave);
        free(sample_array);
    } else {
        printf("Test wave data malloc() error. Exiting\n");
        free(sample_array);
        return 1;
    }

    return 0;
}
