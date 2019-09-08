#include "write_wave.h"

void write_wave(Wave* wave, const char* filename){
    // Convert values to little endian.
    little_endian(&(wave->header.chunk_size), sizeof(uint32_t));
    little_endian(&(wave->header.format_subchunk_size), sizeof(uint32_t));
    
    little_endian(&(wave->header.audio_format), sizeof(uint16_t));
    little_endian(&(wave->header.numberof_channels), sizeof(uint16_t));

    little_endian(&(wave->header.sample_rate), sizeof(uint32_t));
    little_endian(&(wave->header.byte_rate), sizeof(uint32_t));

    little_endian(&(wave->header.block_align), sizeof(uint16_t));
    little_endian(&(wave->header.bits_per_sample), sizeof(uint16_t));

    little_endian(&(wave->header.data_subchunk_size), sizeof(uint32_t));

    // Write file
    FILE *file;
    file = fopen(filename, "wb");

    fwrite(&(wave->header), sizeof(WaveHeader), 1, file);
    fwrite((wave->data), sizeof(char), wave->data_size, file);
    
    fclose(file);

    // Convert back to system endianness
    system_endianness(&(wave->header.chunk_size), 'l', sizeof(uint32_t));
    system_endianness(&(wave->header.format_subchunk_size), 'l', sizeof(uint32_t));

    system_endianness(&(wave->header.audio_format), 'l', sizeof(uint16_t));
    system_endianness(&(wave->header.numberof_channels), 'l', sizeof(uint16_t));

    system_endianness(&(wave->header.sample_rate), 'l', sizeof(uint32_t));
    system_endianness(&(wave->header.byte_rate), 'l', sizeof(uint32_t));

    system_endianness(&(wave->header.block_align), 'l', sizeof(uint16_t));
    system_endianness(&(wave->header.bits_per_sample), 'l', sizeof(uint16_t));
    
    system_endianness(&(wave->header.data_subchunk_size), 'l', sizeof(uint32_t));
}

void write_array_to_wav_file(const char* filename, const int16_t* sample_array, uint32_t sample_array_size, uint16_t numberof_channels, uint32_t sample_rate, uint16_t bits_per_sample){
    Wave wave = make_wave(sample_array, sample_array_size, numberof_channels, sample_rate, bits_per_sample);

    write_wave(&wave, filename);
}
