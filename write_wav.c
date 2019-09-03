#include "write_wav.h"

WaveHeader makeWaveHeader(int const sampleRate, short int const numChannels, short int const bitsPerSample){
    WaveHeader header;

    // .riff .wav Header
    header.chunkId[0] = 'R';
    header.chunkId[1] = 'I';
    header.chunkId[2] = 'F';
    header.chunkId[3] = 'F';

    //header.chunkSize defined later once formatSubChunkSize and dataSubChunk size has also be defined

    header.format[0] = 'W';
    header.format[1] = 'A';
    header.format[2] = 'V';
    header.format[3] = 'E';

    // Format subchunk
    header.formatSubChunkId[0] = 'f';
    header.formatSubChunkId[1] = 'm';
    header.formatSubChunkId[2] = 't';
    header.formatSubChunkId[3] = ' ';

    header.formatSubChunkSize = 16;

    header.audioFormat = 1; // FOR PCM
    header.numChannels = numChannels;
    header.sampleRate = sampleRate;
    header.bitsPerSample = bitsPerSample;
    header.byteRate = header.sampleRate * header.numChannels * header.bitsPerSample / 8;
    header.blockAlign = header.numChannels * header.bitsPerSample / 8;

    // Data subchunk
    header.dataSubChunkId[0] = 'd';
    header.dataSubChunkId[1] = 'a';
    header.dataSubChunkId[2] = 't';
    header.dataSubChunkId[3] = 'a';

    header.dataSubChunkSize = 0;

    header.chunkSize = 4 + (8 + header.formatSubChunkSize) + (8 + header.dataSubChunkSize);

    /*
    Whenever a sample is added:
    chunkSize += (nChannels * bitsPerSample/8)
    dataSubChunkSize += (nChannels * bitsPerSample/8)
    */

    return header;
}