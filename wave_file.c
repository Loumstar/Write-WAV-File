#include "wave_file.h"

void write_wave(const char* filename, Wave* wave){
    // Write file
    FILE *file;
    file = fopen(filename, "wb");
    
    // Convert values to little endian.
    make_wave_header_little_endian(&(wave->header));

    fwrite(&(wave->header), sizeof(WaveHeader), 1, file);
    fwrite(wave->data, sizeof(char), wave->header.data_subchunk_size, file);
    
    // Convert back to system endianness
    make_wave_header_system_endian(&(wave->header), 'l');

    fclose(file);
}

bool check_match(char ch, const char* id_string, size_t i, size_t id_length, FILE* fp){
    // If length of the string is reached, then match is successful
    if(i == id_length){
        return true;
    // If all chars so far have matched, move to next one by invoking another check_match()
    } else if(ch == id_string[i]){
        ch = fgetc(fp);
        return check_match(ch, id_string, i+1, id_length, fp);
    // If the match fails, return false, stopping recursion.
    } else {
        return false;
    }
}

long find_id(FILE* fp, const char* id_string, size_t id_length){
    // Start from beginning of the file
    fseek(fp, 0, SEEK_SET);
    
    bool is_match = false;
    unsigned char ch;

    for(size_t i = 1; !is_match && (int) ch != EOF; i++){
        // get the byte
        ch = (unsigned char) fgetc(fp);
        // For each offset in the file, check for string
        is_match = check_match(ch, id_string, 0, id_length, fp);
        // Return back to the original offset to continue for() loop
        fseek(fp, i, SEEK_SET);
    }
    // Return location of previous ch if found, else return -1
    return is_match ? ftell(fp) - 1: -1;
}

void get_bytes_from_file(FILE* fp, long offset, void* target, size_t size){
    int ch;
    // Go to the location of the value, given as a byte offset from start of the file
    fseek(fp, offset, SEEK_SET);
    // Get value byte by byte and put it into an array while not EOF
    for(size_t i = 0; i < size; i++){
        ch = fgetc(fp);
        if(ch != EOF) ((unsigned char*) target)[i] = (unsigned char) ch;
    }
}

long get_file_size(FILE* fp){
    long initial_position = ftell(fp);
    long file_size;

    fseek(fp, 0, SEEK_END);
    file_size = ftell(fp);
    fseek(fp, initial_position, SEEK_SET);
    
    return file_size;
}

Wave read_wave(const char* filename){
    Wave wave = make_blank_wave();

    FILE *fp;
    uint32_t file_size;

    fp = fopen(filename, "r");

    if(!fp){
        printf("Failed to open file.\n");
    } else {
        file_size = get_file_size(fp);

        // Main chunk
        get_bytes_from_file(fp, 0, wave.header.chunk_id, 4);
        get_bytes_from_file(fp, 4, &wave.header.chunk_size, 4);
        get_bytes_from_file(fp, 8, wave.header.format, 4);

        system_endian(&wave.header.chunk_size, 'l', 4);

        // Check both the file formatting and size is correct
        if(strncmp("RIFF", wave.header.chunk_id, 4) || strncmp("WAVE", wave.header.format, 4)){
            printf("File not correct RIFF or WAVE format.\n");
        } else if(file_size != wave.header.chunk_size + 8){
            printf("Chunk size incorrect.\n");
        }
        
        // Format subchunk
        get_bytes_from_file(fp, 12, wave.header.format_subchunk_id, 4);

        // Check the format subchunk id
        if(strncmp("fmt ", wave.header.format_subchunk_id, 4)){
            printf("Format subchunk not found.\n");
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
        Find data subchunk id incase extra parameters were missed.
        */

       long data_subchunk_start = find_id(fp, "data", 4);

        get_bytes_from_file(fp, data_subchunk_start, wave.header.data_subchunk_id, 4);

        // If id is not found, then do not read subchunk data size or data as these will be incorrect.
        if(data_subchunk_start == -1){
            printf("Data subchunk not found.\n");
        } else {
            
            get_bytes_from_file(fp, data_subchunk_start + 4, &wave.header.data_subchunk_size, 4);

            /*
            Check subchunk sizes. 
            
            8 bytes accounts for the id and sizes for each subchunk. 
            4 bytes is for the format id in the main chunk.
            These are omitted in the calculation of the sizes when the wav file is created.
            */
        
            if(wave.header.chunk_size != 4 + (8 + wave.header.format_subchunk_size) + (8 + wave.header.data_subchunk_size)){
                printf("Format and Data subchunk sizes do not match.\n");
                printf("    This is likely due to extra subchunks in the file.\n");
                printf("    These will be ignored.\n");
            }

            wave.bytes_per_sample = wave.header.bits_per_sample / 8;

            if(wave.bytes_per_sample && wave.header.numberof_channels){
                wave.numberof_samples = wave.header.data_subchunk_size / (wave.bytes_per_sample * wave.header.numberof_channels);
            }

            // Convert all values back to the system endianness as wav files store values in little endian.
            make_wave_header_system_endian(&(wave.header), 'l');

            // Data
            wave.data = malloc(wave.header.data_subchunk_size);
            get_bytes_from_file(fp, data_subchunk_start + 8, wave.data, (size_t) wave.header.data_subchunk_size);

        }
        
        fclose(fp);

    }

    return wave;
}

void write_array_to_wav_file(const char* filename, const int32_t* sample_array, uint32_t sample_array_size, uint16_t numberof_channels, uint32_t sample_rate, uint16_t bits_per_sample){
    // Create wave instance
    Wave wave = make_wave(sample_array, sample_array_size, numberof_channels, sample_rate, bits_per_sample);
    
    // If malloc() succeeds, write data to file
    if(wave.data){
        write_wave(filename, &wave);
        // Deallocate the data
        remove_sample_data(&wave);
    } else {
        printf("Wave sample data malloc() error. Wav file not created.\n");
    }
}
