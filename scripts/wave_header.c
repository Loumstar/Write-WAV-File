#include "wave_header.h"

WaveHeader make_header(uint32_t sample_rate, uint16_t numberof_channels, uint16_t bits_per_sample){
    WaveHeader header;

    // .riff .wav Header
    header.chunk_id[0] = 'R';
    header.chunk_id[1] = 'I';
    header.chunk_id[2] = 'F';
    header.chunk_id[3] = 'F';

    //header.chunk_size defined later once format_subchunk_size and dataSubChunk data_size has also be defined
    header.format[0] = 'W';
    header.format[1] = 'A';
    header.format[2] = 'V';
    header.format[3] = 'E';

    // Format subchunk
    header.format_subchunk_id[0] = 'f';
    header.format_subchunk_id[1] = 'm';
    header.format_subchunk_id[2] = 't';
    header.format_subchunk_id[3] = ' ';

    header.format_subchunk_size = 16;

    header.audio_format = 1; // FOR PCM
    header.numberof_channels = numberof_channels;
    header.sample_rate = sample_rate;
    header.bits_per_sample = bits_per_sample;
    header.byte_rate = header.sample_rate * header.numberof_channels * header.bits_per_sample / 8;
    header.block_align = header.numberof_channels * header.bits_per_sample / 8;

    // Data subchunk
    header.data_subchunk_id[0] = 'd';
    header.data_subchunk_id[1] = 'a';
    header.data_subchunk_id[2] = 't';
    header.data_subchunk_id[3] = 'a';

    header.data_subchunk_size = 0;

    header.chunk_size = 4 + (8 + header.format_subchunk_size) + (8 + header.data_subchunk_size);

    /*
    Whenever a sample is added:
    chunk_size += (nChannels * bits_per_sample/8)
    data_subchunk_size += (nChannels * bits_per_sample/8)
    */

    return header;
}
