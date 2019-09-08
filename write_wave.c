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
    big_endian(&(wave->header.chunk_size), sizeof(uint32_t));
    big_endian(&(wave->header.format_subchunk_size), sizeof(uint32_t));

    big_endian(&(wave->header.audio_format), sizeof(uint16_t));
    big_endian(&(wave->header.numberof_channels), sizeof(uint16_t));

    big_endian(&(wave->header.sample_rate), sizeof(uint32_t));
    big_endian(&(wave->header.byte_rate), sizeof(uint32_t));

    big_endian(&(wave->header.block_align), sizeof(uint16_t));
    big_endian(&(wave->header.bits_per_sample), sizeof(uint16_t));
    
    big_endian(&(wave->header.data_subchunk_size), sizeof(uint32_t));
}
