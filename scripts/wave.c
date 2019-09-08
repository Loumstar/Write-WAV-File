#include "wave.h"

void allocate_sample_data(Wave* wave, uint32_t numberof_samples){
    // Determine how many bytes the sample data will take up.
    wave->data_size = wave->numberof_samples * wave->bytes_per_sample * wave->header.numberof_channels;

    // Add this to the chunk_size and data_subchunk_size.
    wave->header.chunk_size += wave->data_size;
    wave->header.data_subchunk_size += wave->data_size;

    // Allocate this amount to wave.data.
    wave->data = malloc(wave->data_size);

    if(!(wave->data)){
        wave->data_size = 0;
    }
}

void remove_sample_data(Wave* wave){
    // Deallocate the memory
    free(wave->data);
    // Reset the data size of the wave to zero.
    wave->data_size = 0;
}

void add_samples(Wave* wave, const int16_t* sample_array){
    char* sample_byte_array;

    uint32_t data_index = 0;
    int16_t sample;

    // For each sample
    for(size_t i = 0; i < wave->numberof_samples; i++){
        sample = sample_array[i]; 
        
        // Convert to little endian
        sample_byte_array = (char*) little_endian(&sample, sizeof(sample));

        // For each channel
        for(size_t j = 0; j < wave->header.numberof_channels; j++){
            // Add byte-by-byte, the sample in little endian to wave.data
            for(size_t k = 0; k < wave->bytes_per_sample; k++){
                (wave->data)[data_index] = sample_byte_array[k];
                data_index++;
            }
        }
    }
}

Wave make_wave(const int16_t* sample_array, uint16_t numberof_channels, uint32_t numberof_samples, uint32_t sample_rate, uint16_t bits_per_sample){
    Wave wave;

    wave.header = make_header(sample_rate, numberof_channels, bits_per_sample);
    wave.numberof_samples = numberof_samples;
    wave.bytes_per_sample = bits_per_sample / 8;

    allocate_sample_data(&wave, numberof_samples);

    if(wave.data){
        add_samples(&wave, sample_array);
    } else {
        printf("Wave data malloc() error.\n");
    }

    return wave;
}

void print_metadata(Wave* wave){
    printf("[\n");

    printf("    {\n");
    printf("        chunkId: \'%.4s\',\n", wave->header.chunk_id);
    printf("        chunkSize: %i,\n", wave->header.chunk_size);
    printf("        format: \'%.4s\',\n", wave->header.format);
    printf("        formatSubchunkId: \'%.4s\',\n", wave->header.format_subchunk_id);
    printf("        formatSubchunkSize: %i,\n", wave->header.format_subchunk_size);
    printf("        audioFormat: %i,\n", wave->header.audio_format);
    printf("        nChannels: %i,\n", wave->header.numberof_channels);
    printf("        sampleRate: %i,\n", wave->header.sample_rate);
    printf("        byteRate: %i,\n", wave->header.byte_rate);
    printf("        blockAlign: %i,\n", wave->header.block_align);
    printf("        bitsPerSample: %i,\n", wave->header.bits_per_sample);
    printf("        dataSubchunkId: \'%.4s\',\n", wave->header.data_subchunk_id);
    printf("        dataSubchunkSize: %i\n", wave->header.data_subchunk_size);
    printf("    },\n");

    printf("    {\n");
    printf("        dataSize: %i,\n", wave->data_size);
    printf("        nSamples: %i,\n", wave->numberof_samples);
    printf("    }\n");

    printf("]\n");
}
