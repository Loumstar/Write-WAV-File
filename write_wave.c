#include "write_wave.h"

void make_wave_struct_little_endian(Wave* wave){
    little_endian(&(wave->header.chunk_size), sizeof(uint32_t));
    little_endian(&(wave->header.format_subchunk_size), sizeof(uint32_t));
    
    little_endian(&(wave->header.audio_format), sizeof(uint16_t));
    little_endian(&(wave->header.numberof_channels), sizeof(uint16_t));

    little_endian(&(wave->header.sample_rate), sizeof(uint32_t));
    little_endian(&(wave->header.byte_rate), sizeof(uint32_t));

    little_endian(&(wave->header.block_align), sizeof(uint16_t));
    little_endian(&(wave->header.bits_per_sample), sizeof(uint16_t));

    little_endian(&(wave->header.data_subchunk_size), sizeof(uint32_t));
}

void make_wave_struct_system_endian(Wave* wave, char current_endianness){
    system_endianness(&(wave->header.chunk_size), current_endianness, sizeof(uint32_t));
    system_endianness(&(wave->header.format_subchunk_size), current_endianness, sizeof(uint32_t));

    system_endianness(&(wave->header.audio_format), current_endianness, sizeof(uint16_t));
    system_endianness(&(wave->header.numberof_channels), current_endianness, sizeof(uint16_t));

    system_endianness(&(wave->header.sample_rate), current_endianness, sizeof(uint32_t));
    system_endianness(&(wave->header.byte_rate), current_endianness, sizeof(uint32_t));

    system_endianness(&(wave->header.block_align), current_endianness, sizeof(uint16_t));
    system_endianness(&(wave->header.bits_per_sample), current_endianness, sizeof(uint16_t));
    
    system_endianness(&(wave->header.data_subchunk_size), current_endianness, sizeof(uint32_t));
}

void write_wave(Wave* wave, const char* filename){
    // Write file
    FILE *file;
    file = fopen(filename, "wb");
    
    // Convert values to little endian.
    make_wave_struct_little_endian(wave);

    fwrite(&(wave->header), sizeof(WaveHeader), 1, file);
    fwrite((wave->data), sizeof(char), wave->header.data_subchunk_size, file);
    
    // Convert back to system endianness
    make_wave_struct_system_endian(wave, 'l');

    fclose(file);

}

void write_array_to_wav_file(const char* filename, const int32_t* sample_array, uint32_t sample_array_size, uint16_t numberof_channels, uint32_t sample_rate, uint16_t bits_per_sample){
    // Create wave instance
    Wave wave = make_wave(sample_array, sample_array_size, numberof_channels, sample_rate, bits_per_sample);
    
    // If malloc() succeeds, write data to file
    if(wave.data){
        write_wave(&wave, filename);
        // Deallocate the data
        remove_sample_data(&wave);
    } else {
        printf("Wave sample data malloc() error. Wav file not created.\n");
    }
}

void get_bytes_from_file(FILE* fp, size_t offset, void* target, size_t size){
    // Go to the location of the value, given as a byte offset from start of the file
    fseek(fp, SEEK_SET, offset);

    // Get value byte by byte and put it into an array
    for(size_t i = 0; i < size; i++){
        ((char*)target)[i] = fgetc(fp);
    }
}

Wave read_wave(char* filename){
    Wave wave = make_blank_wave();

    FILE *fp;
    uint32_t file_size;

    fp = fopen(filename, "r");

    // If file opens successfully, determine size of the file
    if(fp){
        fseek(fp, 0, SEEK_END);
        file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
    } else {
        printf("Failed to open file.\n");
        return wave;
    }

    // Main chunk
    get_bytes_from_file(fp, 0, wave.header.chunk_id, 4);
    get_bytes_from_file(fp, 4, &wave.header.chunk_size, 4);
    get_bytes_from_file(fp, 8, wave.header.format, 4);

    little_endian(&wave.header.chunk_size, 4);

    // Check both the file formatting and size is correct
    if(strncmp("RIFF", wave.header.chunk_id, 4) || strncmp("WAVE", wave.header.format, 4)){
        printf("File not correct RIFF or WAVE format.\n");
        fclose(fp);
        return wave;
    } else if(file_size != wave.header.chunk_size + 8){
        printf("File size incorrect.\n");
        printf("%i != %i", file_size, wave.header.chunk_size + 8);
        fclose(fp);
        return wave;
    }
    
    // Format subchunk
    get_bytes_from_file(fp, 12, wave.header.format_subchunk_id, 4);

    // Find the format subchunk id
    if(strncmp("fmt ", wave.header.format_subchunk_id, 4)){
        printf("Format subchunk not found.\n");
        fclose(fp);
        return wave;
    }

    get_bytes_from_file(fp, 16, &wave.header.format_subchunk_size, 4);

    get_bytes_from_file(fp, 20, &wave.header.audio_format, 2);
    get_bytes_from_file(fp, 22, &wave.header.numberof_channels, 2);

    get_bytes_from_file(fp, 24, &wave.header.sample_rate, 4);
    get_bytes_from_file(fp, 28, &wave.header.byte_rate, 4);

    get_bytes_from_file(fp, 32, &wave.header.block_align, 2);
    get_bytes_from_file(fp, 34, &wave.header.bits_per_sample, 2);

    // No code for getting extra parameters

    /*
    Data subchunk
    Offset includes format_subchunk_size incase extra parameters were missed.
    */

    get_bytes_from_file(fp, 20 + wave.header.format_subchunk_size, wave.header.data_subchunk_id, 4);

    // Find the data subchunk id
    if(strncmp("data", wave.header.data_subchunk_id, 4)){
        printf("Data subchunk not found.\n");
        fclose(fp);
        return wave;
    }

    get_bytes_from_file(fp, 24 + wave.header.format_subchunk_size, &wave.header.data_subchunk_size, 4);
    
    /*
    Check subchunk sizes. 
    
    8 bytes accounts for the id and sizes for each subchunk. 
    4 bytes is for the format id in the main chunk.
    These are omitted in the calculation of the sizes when the wav file is created.
    */
   
    if(wave.header.chunk_size != 4 + (8 + wave.header.format_subchunk_size) + (8 + wave.header.data_subchunk_size)){
        printf("Format or Data subchunk incorrect size\n");
        printf("%i != %i", wave.header.chunk_size, 4 + (8 + wave.header.format_subchunk_size) + (8 + wave.header.data_subchunk_size));
        fclose(fp);
        return wave;
    }

    wave.bytes_per_sample = wave.header.bits_per_sample / 8;
    wave.numberof_samples = wave.header.data_subchunk_size / (wave.bytes_per_sample * wave.header.numberof_channels);

    // Data
    wave.data = malloc(wave.header.data_subchunk_size);
    get_bytes_from_file(fp, 24 + wave.header.format_subchunk_size, wave.data, (size_t) wave.header.data_subchunk_size);

    fclose(fp);
    // Convert all values back to the system endianness as wav files store values in little endian.
    make_wave_struct_system_endian(&wave, 'l');

    return wave;
}

void write_wave_data_to_array(int32_t* sample_array, Wave* wave){
    size_t wave_data_index = 0;
    size_t sample_index = 0;

    char* sample_byte_array;

    while(wave_data_index < wave->header.data_subchunk_size){
        // Create a pointer to the sample location in sample_array so each byte can be manipulated individually
        sample_byte_array = (char*) &sample_array[sample_index];
        // As the return is only the first channel, skip all other channels by moving the wave data index forward
        wave_data_index = sample_index * wave->bytes_per_sample * wave->header.numberof_channels;

        for(size_t i = 0; i < wave->bytes_per_sample; i++){
            // Set, byte-by-byte, the value of the sample in sample_array by reading it straight from the wave data.
            sample_byte_array[i] = (wave->data)[wave_data_index + i];
        }
        // Convert to the system endianness
        system_endianness(&sample_array[sample_index], 'l', 4);
        // Move to the next sample
        sample_index++;
    }
}
