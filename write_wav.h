typedef struct WaveHeader {
    
    // Riff Wave Header
    char chunkId[4];
    int  chunkSize;
    char format[4];

    // Format Subchunk
    char formatSubChunkId[4];
    int  formatSubChunkSize;
    short int audioFormat;
    short int numChannels;
    int sampleRate;
    int byteRate;
    short int blockAlign;
    short int bitsPerSample;
    //short int extraParamSize;

    // Data Subchunk
    char dataSubChunkId[4];
    int  dataSubChunkSize;

} WaveHeader;

typedef struct Wave {

    WaveHeader header;

    char* data;
    
    long long int index;
    long long int size;
    long long int nSamples;

} Wave;