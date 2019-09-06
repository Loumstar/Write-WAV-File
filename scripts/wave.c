#include "wave.h"

void set_duration(Wave* wave, uint32_t numberof_frames){
    wave->numberof_frames = numberof_frames;
    wave->data_size = wave->numberof_frames * wave->bytes_per_sample * wave->header.numberof_channels;

    wave->header.chunk_size += wave->data_size;
    wave->header.data_subchunk_size += wave->data_size;

    wave->data = malloc(wave->data_size);

    printf("%i, %p\n", wave->data_size, (void*)wave->data);
}

void remove_frame_data(Wave* wave){
    free(wave->data);
}

void add_sample(Wave* wave, const int16_t* sample_array){
    char* sample_byte_array;

    uint32_t index = 0;
    uint16_t sample;

    for(size_t i = 0; i < wave->numberof_frames; i++){

        sample = (1 + ((float) sample_array[i] / UINT16_MAX)) * (UINT16_MAX / 2);    
        
        sample_byte_array = (char*) &sample;
        printf("%i %i -->", sample_byte_array[0], sample_byte_array[1]);
        
        little_endian(&sample, sizeof(sample));

        sample_byte_array = (char*) &sample;
        printf(" %i %i:", sample_byte_array[0], sample_byte_array[1]);

        for(size_t j = 0; j < wave->header.numberof_channels; j++){
            for(size_t k = 0; k < wave->bytes_per_sample; k++){
                (wave->data)[index] = sample_byte_array[k];
                printf(" %i", (wave->data)[index]);
                index++;
            }
        }
        printf("\n");
    }
}

Wave make_mono_wave(const int16_t* sample_array, uint32_t numberof_frames, uint32_t sample_rate, uint16_t bits_per_sample){
    
    Wave wave;

    wave.header = make_header(sample_rate, 1, bits_per_sample);
    wave.bytes_per_sample = bits_per_sample / 8;

    set_duration(&wave, numberof_frames);
    add_sample(&wave, sample_array);

    return wave;
}
