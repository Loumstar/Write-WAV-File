#include "wave_header.h"

wave_header make_header(uint32_t sample_rate, uint16_t numberof_channels, uint16_t bits_per_sample){
    wave_header wh;

    // Chunk
    wh.chunk_id[0] = 'R';
    wh.chunk_id[1] = 'I';
    wh.chunk_id[2] = 'F';
    wh.chunk_id[3] = 'F';

    /*
    wh.chunk_size defined later once format_subchunk_size and
    data_subchunk_size have also be defined.
    */

    wh.format[0] = 'W';
    wh.format[1] = 'A';
    wh.format[2] = 'V';
    wh.format[3] = 'E';

    // Format subchunk
    wh.format_subchunk_id[0] = 'f';
    wh.format_subchunk_id[1] = 'm';
    wh.format_subchunk_id[2] = 't';
    wh.format_subchunk_id[3] = ' ';

    // The number of bytes required to hold all the values in the format subchunk minus
    // the format_subchunk_size andformat_subchunk_id
    wh.format_subchunk_size = 16;

    wh.audio_format = 1; // For PCM
    wh.numberof_channels = numberof_channels;
    wh.sample_rate = sample_rate;
    wh.bits_per_sample = bits_per_sample;
    wh.byte_rate = wh.sample_rate * (wh.bits_per_sample / 8) * wh.numberof_channels;
    wh.block_align = (wh.bits_per_sample / 8) * wh.numberof_channels;

    // Data subchunk
    wh.data_subchunk_id[0] = 'd';
    wh.data_subchunk_id[1] = 'a';
    wh.data_subchunk_id[2] = 't';
    wh.data_subchunk_id[3] = 'a';

    wh.data_subchunk_size = 0;

    wh.chunk_size = 4 + (8 + wh.format_subchunk_size) + (8 + wh.data_subchunk_size);

    /*
    Whenever a sample is added:
    chunk_size += (numberof_channels * bits_per_sample / 8)
    data_subchunk_size += (numberof_channels * bits_per_sample / 8)
    */

    return wh;
}

wave_header make_blank_header(){
    wave_header wh;

    wh.format_subchunk_size = 16;

    wh.audio_format = 1;
    wh.numberof_channels = 0;
    wh.sample_rate = 0;
    wh.bits_per_sample = 0;
    wh.byte_rate = 0;
    wh.block_align = 0;
    wh.data_subchunk_size = 0;

    wh.chunk_size = 20 + wh.format_subchunk_size;

    return wh;
}

void make_wave_header_little_endian(wave_header* wh){
    little_endian(&(wh->chunk_size), sizeof(uint32_t));
    little_endian(&(wh->format_subchunk_size), sizeof(uint32_t));
    
    little_endian(&(wh->audio_format), sizeof(uint16_t));
    little_endian(&(wh->numberof_channels), sizeof(uint16_t));

    little_endian(&(wh->sample_rate), sizeof(uint32_t));
    little_endian(&(wh->byte_rate), sizeof(uint32_t));

    little_endian(&(wh->block_align), sizeof(uint16_t));
    little_endian(&(wh->bits_per_sample), sizeof(uint16_t));

    little_endian(&(wh->data_subchunk_size), sizeof(uint32_t));
}

void make_wave_header_system_endian(wave_header* wh, char current_endianness){
    system_endian(&(wh->chunk_size), current_endianness, sizeof(uint32_t));
    system_endian(&(wh->format_subchunk_size), current_endianness, sizeof(uint32_t));

    system_endian(&(wh->audio_format), current_endianness, sizeof(uint16_t));
    system_endian(&(wh->numberof_channels), current_endianness, sizeof(uint16_t));

    system_endian(&(wh->sample_rate), current_endianness, sizeof(uint32_t));
    system_endian(&(wh->byte_rate), current_endianness, sizeof(uint32_t));

    system_endian(&(wh->block_align), current_endianness, sizeof(uint16_t));
    system_endian(&(wh->bits_per_sample), current_endianness, sizeof(uint16_t));
    
    system_endian(&(wh->data_subchunk_size), current_endianness, sizeof(uint32_t));
}
