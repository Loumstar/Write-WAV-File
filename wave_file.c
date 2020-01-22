#include "wave_file.h"

wave blank_wave(){
    /*
    Method to return a blank wave.

    This should be used as the starting point for any wave, as it removes the potential
    for wild pointer or random values.
    */
    wave w;

    w.header = make_blank_header();
    
    w.numberof_samples = 0;
    w.bytes_per_sample = 0;

    w.data = NULL;

    w.__is_valid = false;

    return w;
}

void allocate_samples(wave* w, uint32_t numberof_samples){
    /*
    Method to allocate memory to a wave, so it can hold the sample data.

    The amount of memory to allocate is calculated based on the number of samples, number
    of channels and the bytes per sample:

        size = no. of samples * no. of channels * bytes per sample

    If malloc succeeds, the number of bytes allocated will be added to the total and data
    chunk size fields.
    */
    int32_t data_size = w->numberof_samples * w->bytes_per_sample * w->header.numberof_channels;

    w->data = malloc(data_size);

    if(w->data){
        w->header.chunk_size += data_size;
        w->header.data_subchunk_size = data_size;
    }
}

void deallocate_samples(wave* w){
    /*
    Method to deallocate memory from a wave file.

    The number of bytes deallocated is removed from the relevant chunk size fields, and
    the wave is considered invalid for reading.
    */
    if(wave_is_valid(w)){
        free(w->data);

        w->header.chunk_size -= w->header.data_subchunk_size;
        w->header.data_subchunk_size = 0;
        w->__is_valid = false;
    }
}

void add_samples(wave* w, const int32_t* sample_array){
    /*
    Method to copy samples into a wave file in the correct format.

    Beginning at the first sample of the first channel, the values are added byte-by-byte
    in little endian. The sample for each channel is added before moving to the next
    sample in time.

    Note that 8-bit audio is not signed, while 16 and 32 bit are. For this reason, the
    value needs to be increased by 128.
    */
    char* sample_byte_array;

    size_t data_index = 0;
    int32_t sample;

    // For each sample
    for(size_t i = 0; i < w->numberof_samples; i++){
        
        // If 8-bit, make sample value unsigned
        if(w->bytes_per_sample == 1){
            sample = sample_array[i] + 128;
        } else {
            sample = sample_array[i];
        }

        // Convert to little endian
        sample_byte_array = (char*) little_endian(&sample, sizeof(sample));

        // For each channel
        for(size_t j = 0; j < w->header.numberof_channels; j++){
            // Add byte-by-byte, the sample in little endian
            for(size_t k = 0; k < w->bytes_per_sample; k++){
                (w->data)[data_index] = sample_byte_array[k];
                data_index++;
            }
        }
    }
}

wave create_wave(const int32_t* sample_array, uint32_t numberof_samples, uint16_t numberof_channels, uint32_t sample_rate, uint16_t bits_per_sample){
    /*
    Method to return a fully defined instance of a wave.

    Although the method can create a wave with multiple channels, all these channels will 
    have the sample sample at the same point in time.

    If malloc() is successful, the samples are added and this makes the wave file valid
    for reading.
    */
    wave w = blank_wave();

    w.header = make_header(sample_rate, numberof_channels, bits_per_sample);
    w.numberof_samples = numberof_samples;
    w.bytes_per_sample = bits_per_sample / 8;

    allocate_samples(&w, numberof_samples);

    if(w.data){ 
        add_samples(&w, sample_array);
        w.__is_valid = true;
    }
    return w;
}

bool wave_is_valid(const wave* w){
    // Method that returns whether it is safe to read the data from a wave file.
    return w->__is_valid;
}

void print_metadata(const wave* w){
    printf("[\n");

    printf("    {\n");
    printf("        chunkId: \'%.4s\',\n", w->header.chunk_id);
    printf("        chunkSize: %i,\n", w->header.chunk_size);
    printf("        format: \'%.4s\',\n", w->header.format);
    printf("        formatSubchunkId: \'%.4s\',\n", w->header.format_subchunk_id);
    printf("        formatSubchunkSize: %i,\n", w->header.format_subchunk_size);
    printf("        audioFormat: %i,\n", w->header.audio_format);
    printf("        nChannels: %i,\n", w->header.numberof_channels);
    printf("        sampleRate: %i,\n", w->header.sample_rate);
    printf("        byteRate: %i,\n", w->header.byte_rate);
    printf("        blockAlign: %i,\n", w->header.block_align);
    printf("        bitsPerSample: %i,\n", w->header.bits_per_sample);
    printf("        dataSubchunkId: \'%.4s\',\n", w->header.data_subchunk_id);
    printf("        dataSubchunkSize: %i\n", w->header.data_subchunk_size);
    printf("    },\n");

    printf("    {\n");
    printf("        bytesPerSample: %i,\n", w->bytes_per_sample);
    printf("        nSamples: %i,\n\n", w->numberof_samples);
    printf("    }\n");

    printf("]\n");
}

bool check_match(int ch, const char* id_string, size_t i, size_t id_length, FILE* fp){
    /*
    Method to check if a sequence of bytes from a file fit a string chars.

    i denotes the index of the char in the string. If the first match succeeds, the index
    is incremented the next check is made.

    If a check fails, the whole recursion returns false. If the index reaches the end of
    the string and does not fail, the whole recursion will return true.
    */
    if(i == id_length) return true;
    else if((char) ch != id_string[i]) return false;
    else return check_match(fgetc(fp), id_string, i + 1, id_length, fp);
}

long find_id(FILE* fp, const char* id_string, size_t id_length){
    /*
    Method to search for a string of chars in a file.

    Starting from the beginning of the file, each byte is looped through and 
    check_match() is run. The method stops at the first successful match.
    */
    fseek(fp, 0, SEEK_SET);
    
    bool is_match = false;
    int ch = 0;

    // For each byte in the file
    for(size_t i = 1; !is_match && ch != EOF; i++){
        // Get the byte
        ch = fgetc(fp);
        // For each offset in the file, check for string
        is_match = check_match(ch, id_string, 0, id_length, fp);
        // Return back to the original offset to continue for() loop
        fseek(fp, i, SEEK_SET);
    }
    // Return location of previous char if found, else return -1
    return is_match ? ftell(fp) - 1: -1;
}

void get_bytes_from_file(FILE* fp, long offset, void* target, size_t size){
    /*
    Method to copy bytes from a file to an array.
    */
    fseek(fp, offset, SEEK_SET);
    for(size_t i = 0; i < size; i++){
        int ch = fgetc(fp);
        if(ch != EOF) ((char*) target)[i] = (char) ch;
    }
}

long get_file_size(FILE* fp){
    /*
    Method to return the number of bytes in a file.
    */
    long initial_position = ftell(fp);
    long file_size;

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, initial_position, SEEK_SET);
    
    return file_size;
}

int get_riff_chunk(FILE* fp, wave* w){
    /*
    Method to find the RIFF chunk in a RIFF file and save the value for each field to a
    wave.

    In the RIFF chunk, the fields to find are:

        - Chunk ID. This is a char array, and is 4 bytes long. The array should equal 
          'RIFF'.

        - Chunk Size. This is the number of bytes immediately following this field in the
          file. It is 4 bytes long and is in little endian.

        - Chunk Format. This is another char array and is 4 bytes long. The array should
          equal 'WAVE'.

    If the ID and format fields do not equal 'RIFF' and 'WAVE', the method will exit with
    1 (i.e. EXIT_FAILURE).
    */
    long riff_chunk_start = find_id(fp, "RIFF", 4);

    if(riff_chunk_start == -1){
        printf("File not RIFF format.\n");
        return 1;
    }

    get_bytes_from_file(fp, riff_chunk_start, w->header.chunk_id, 4);
    get_bytes_from_file(fp, riff_chunk_start + 4, &w->header.chunk_size, 4);
    get_bytes_from_file(fp, riff_chunk_start + 8, w->header.format, 4);

    system_endian(&w->header.chunk_size, 'l', 4);

    if(strncmp("WAVE", w->header.format, 4)){
        printf("File not WAVE format.\n");
        return 1;
    }
    
    return 0;
}

int get_format_subchunk(FILE* fp, wave* w){
    /*
    Method to find the format subchunk in a RIFF file and save the value for each field
    to a wave.

    In the format subchunk, the fields to find are:

        - Format Subchunk ID. This is a char array and is 4 bytes long.

        - Format Subchunk Size. This is the number of bytes immediately following this
          field in the subchunk. It is 4 bytes long and is in little endian.

        - Audio Format. This describes how the audio was quantised. Typically, PCM is
          used, which has a value of 1 for the audio format. Other values indicate some
          kind of compression. It is 2 bytes long and is little endian.

        - Number of Channels. 1 channel is mono, 2 channels is stereo, etc. It is 2 bytes
          long and little endian.

        - Sample Rate. The number of samples captured per second, typically 44100. It is
          4 bytes long and little endian.

        - Byte Rate. The number of bytes required to represent a second of audio. It is 4
          bytes long and little endian.

        - Block Align. The number of bytes between two consecutive samples from the same
          channel: block align = number of channels * bytes per sample
          It is 2 bytes long and little endian.

        - Bits per sample. It is 2 bytes long and little endian.

    All other fields are ignored.

    If the ID field does not equal 'fmt ', the method will exit with 1 (i.e. EXIT_FAILURE).
    */
    long format_subchunk_start = find_id(fp, "fmt ", 4);

    if(format_subchunk_start == -1){
        printf("Format subchunk not found.\n");
        return 1;
    }

    get_bytes_from_file(fp, format_subchunk_start, w->header.format_subchunk_id, 4);
    get_bytes_from_file(fp, format_subchunk_start + 4, &w->header.format_subchunk_size, 4);

    get_bytes_from_file(fp, format_subchunk_start + 8, &w->header.audio_format, 2);
    get_bytes_from_file(fp, format_subchunk_start + 10, &w->header.numberof_channels, 2);

    get_bytes_from_file(fp, format_subchunk_start + 12, &w->header.sample_rate, 4);
    get_bytes_from_file(fp, format_subchunk_start + 16, &w->header.byte_rate, 4);

    get_bytes_from_file(fp, format_subchunk_start + 20, &w->header.block_align, 2);
    get_bytes_from_file(fp, format_subchunk_start + 22, &w->header.bits_per_sample, 2);

    // Convert all values back to the system endianness as wav files store values in little endian.
    make_wave_header_system_endian(&(w->header), 'l');

    return 0;
}

int get_data_subchunk(FILE* fp, wave* w){
    /*
    Method to find the data subchunk in a RIFF file and save the value for each field
    to a wave.

    In the data subchunk, the fields to find are:

        - Data Subchunk ID. This is a char array and is 4 bytes long.

        - Data Subchunk Size. This is the number of bytes immediately following this
          field in the subchunk, (i.e. the size of the data field). It is 4 bytes long
          and is in little endian.

        - Data. This is the main field of the file. Its length is given in bytes by the
          data_subchunk_size and is in little endian.

    If the ID field does not equal 'data', the method will exit with 1 (i.e. EXIT_FAILURE).
    */
    long data_subchunk_start = find_id(fp, "data", 4);

    if(data_subchunk_start == -1){
        printf("Data subchunk not found.\n");
        return 1;
    }

    get_bytes_from_file(fp, data_subchunk_start, w->header.data_subchunk_id, 4);
    get_bytes_from_file(fp, data_subchunk_start + 4, &w->header.data_subchunk_size, 4);

    w->bytes_per_sample = w->header.bits_per_sample / 8;
    
    if(w->bytes_per_sample && w->header.numberof_channels){
        w->numberof_samples = w->header.data_subchunk_size / (w->bytes_per_sample * w->header.numberof_channels);
    }

    w->data = malloc(w->header.data_subchunk_size);
    get_bytes_from_file(fp, data_subchunk_start + 8, w->data, (size_t) w->header.data_subchunk_size);

    return 0;
}

wave read_wav_file(const char* filename){
    /*
    Method to read the data from a wav file and return a wave type.

    The wav file is split into three chunks:
        - The riff "RIFF" chunk
        - The format "fmt " subchunk
        - The data "data" subchunk

    If any of the chunks cannot be found, the read process is stopped and a wave with all
    the already obtained information is returned.

    If all chunks are found, the wave is made valid, i.e. it can be read safely.
    */
    wave w = blank_wave();
    FILE *fp = fopen(filename, "r");

    if(!fp){
        printf("Failed to open file.\n");
        return w;
    }
    
    if(get_riff_chunk(fp, &w) || 
       get_format_subchunk(fp, &w) || 
       get_data_subchunk(fp, &w)){
           return w;
    }

    // This library cannot read higher than 32-bit samples
    if(w.bytes_per_sample <= 4){
        w.__is_valid = true;
    }

    fclose(fp);

    return w;
}
/*
void correct_sign_bit_location(int32_t* sample, uint16_t bytes_per_sample){
    
    Method to correct the location of the sign bit when a bytes from an int of a
    different size is copied to 32-bit sample byte array.

    When a n-bit integer is negative, its sign bit will be the (n-1)th bit.
    Therefore this needs to be always at the 31st bit of the sample byte array as it is a
    32-bit integer.

    Due to two's complement, the difference between the value evaluated in the 32-bit int
    and the actual value from the n-bit int is always 2^(n-1).

    Therefore, subtract 2^(n-1) from the value held in the 32-bit sample.
    
    char* sample_byte_array = (void*) sample;

    if(sample_byte_array[bytes_per_sample - 1] < 0){
        *sample -= pow(2, (8 * bytes_per_sample) - 1);
    }
}
*/
void wave_data_to_array(const wave* w, int32_t* sample_array){
    /*
    Method to read the data from a wave type and write them into a int32_t array samples
    for one channel.

    At the moment, only the sample data from the first channel can be written to an
    array, however this will be updated to allow the user to choose any channel.

    The bytes stored in a wave type are little endian, so they are converted to the
    system endianess.
    */
    size_t wave_data_index;
    char* sample_byte_array;

    for(size_t sample_index = 0; wave_is_valid(w) && sample_index < w->numberof_samples; sample_index++){
        // Find the location of the sample in the wave data, this skips the samples from other channels
        wave_data_index = sample_index * w->header.block_align;
        
        // Set, byte-by-byte, the value of the sample into sample_array[i] using sample_byte_array
        sample_byte_array = (char*) &sample_array[sample_index];
        for(size_t i = 0; i < w->bytes_per_sample; i++){
            sample_byte_array[i] = (w->data)[wave_data_index + i];
        }
        
        // Convert to the system endianness to allow for arithmetic on samples.
        system_endian(&sample_array[sample_index], 'l', 4);

        //If 8-bit audio, the data is unsigned so subtract 128 from each sample.    
        if(w->bytes_per_sample == 1){
            sample_array[sample_index] -= 128;
        }
    }
}

void write_wav_file(const char* filename, wave* w){
    /*
    Method to write a wave to a wav file.

    The wave header is written into the file first, followed by the data.
    Note all values in the wave header must be in little endian, except for chunk IDs.

    After the file has been written, the values are converted back to the system
    endianess.
    */
    FILE *file;
    file = fopen(filename, "wb");
    
    if(wave_is_valid(w)){
        // Convert values to little endian.
        make_wave_header_little_endian(&(w->header));

        // Write the header first, followed by the data.
        fwrite(&(w->header), sizeof(wave_header), 1, file);
        fwrite(w->data, sizeof(char), w->header.data_subchunk_size, file);
        
        // Convert back to system endianness
        make_wave_header_system_endian(&(w->header), 'l');
    } else {
        printf("wave file did not initalise correctly. Empty file will be returned.\n");
    }

    fclose(file);
}

void write_array_to_wav_file(const char* filename, const int32_t* sample_array, uint32_t sample_array_size, uint16_t numberof_channels, uint32_t sample_rate, uint16_t bits_per_sample){
    /*
    Method to create a wav file straight from an array, without the user needing to use
    the wave datatype.
    */
    wave w = create_wave(sample_array, sample_array_size, numberof_channels, sample_rate, bits_per_sample);

    if(wave_is_valid(&w)){
        write_wav_file(filename, &w);
        deallocate_samples(&w);
    } else {
        printf("wave sample data malloc() error. Wav file not created.\n");
    }
}
