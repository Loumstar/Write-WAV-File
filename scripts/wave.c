#include "wave.h"

void allocate_sample_data(Wave* wave, uint32_t numberof_samples){
    // Determine how many bytes the sample data will take up.
    int32_t data_size = wave->numberof_samples * wave->bytes_per_sample * wave->header.numberof_channels;

    // Allocate this amount to wave.data.
    wave->data = malloc(data_size);

    // If malloc() does not fail, set chunk sizes
    if(wave->data){
        wave->header.chunk_size += data_size;
        wave->header.data_subchunk_size += data_size;
    }
}

void remove_sample_data(Wave* wave){
    // Deallocate the memory
    free(wave->data);

    // Reset chunk sizes back to zero.
    wave->header.chunk_size -= wave->header.data_subchunk_size;
    wave->header.data_subchunk_size = 0;
}

void add_samples(Wave* wave, const int32_t* sample_array){
    char* sample_byte_array;

    size_t data_index = 0;
    int32_t sample;

    for(size_t i = 0; i < wave->numberof_samples; i++){
        
        // 8-bit audio is unsigned while 16- and 32-bit is signed
        if(wave->header.bits_per_sample == 8){
            sample = sample_array[i] + INT8_MAX;
        } else {
            sample = sample_array[i];
        }

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

Wave make_wave(const int32_t* sample_array, uint32_t numberof_samples, uint16_t numberof_channels, uint32_t sample_rate, uint16_t bits_per_sample){
    Wave wave;

    wave.header = make_header(sample_rate, numberof_channels, bits_per_sample);
    wave.numberof_samples = numberof_samples;
    wave.bytes_per_sample = bits_per_sample / 8;

    // Allocate memory so the Wave instance can hold the samples
    allocate_sample_data(&wave, numberof_samples);

    // If malloc() is successful, add the samples.
    if(wave.data) add_samples(&wave, sample_array);
    else printf("Wave data malloc() error.\n");

    return wave;
}

Wave make_blank_wave(){
    Wave wave;

    wave.header = make_blank_header();
    
    wave.numberof_samples = 0;
    wave.bytes_per_sample = 0;

    wave.data = NULL;

    return wave;
}

void read_wave_data_to_array(const Wave* wave, int32_t* sample_array){
    size_t wave_data_index = 0;
    size_t sample_index = 0;

    char* sample_byte_array;

    while(wave_data_index < wave->header.data_subchunk_size){
        // Create a pointer to the sample location in sample_array so each byte can be manipulated individually
        sample_byte_array = (char*) &sample_array[sample_index];

        for(size_t i = 0; i < wave->bytes_per_sample; i++){
            // Set, byte-by-byte, the value of the sample in sample_array by reading it straight from the wave data.
            sample_byte_array[i] = (wave->data)[wave_data_index + i];
        }
        // Convert to the system endianness
        system_endian(&sample_array[sample_index], 'l', 4);
        
        // As the return is only the first channel, skip the samples from other channels by moving the data index forward
        wave_data_index += wave->bytes_per_sample * wave->header.numberof_channels;
        // Move to the next sample
        sample_index++;
    }
}

void print_metadata(const Wave* wave){
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
    printf("        bytesPerSample: %i,\n", wave->bytes_per_sample);
    printf("        nSamples: %i,\n\n", wave->numberof_samples);
    printf("    }\n");

    printf("]\n");
}
