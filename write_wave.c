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
    // Convert values to little endian.
    make_wave_struct_little_endian(wave);

    // Write file
    FILE *file;
    file = fopen(filename, "wb");

    fwrite(&(wave->header), sizeof(WaveHeader), 1, file);
    fwrite((wave->data), sizeof(char), wave->header.data_subchunk_size, file);
    
    fclose(file);

    // Convert back to system endianness
    make_wave_struct_system_endian(wave, 'l');
}

void write_array_to_wav_file(const char* filename, const int32_t* sample_array, uint32_t sample_array_size, uint16_t numberof_channels, uint32_t sample_rate, uint16_t bits_per_sample){
    Wave wave = make_wave(sample_array, sample_array_size, numberof_channels, sample_rate, bits_per_sample);
    if(wave.data){
        write_wave(&wave, filename);
        print_metadata(&wave);
        remove_sample_data(&wave);
    } else {
        printf("Wave sample data malloc() error. Wav file not created.\n");
    }
}

void get_value_from_file(FILE* fp, size_t offset, void* target, size_t size){
    fseek(fp, SEEK_SET, offset);
    for(size_t i = 0; i < size; i++){
        ((unsigned char*)target)[i] = fgetc(fp);
    }
    fseek(fp, SEEK_CUR, -size);
}

Wave read_wave(char* filename){
    Wave wave;
    
    wave.header = make_blank_header();

    FILE *fp;
    uint32_t file_size;

    fp = fopen(filename, "r");

    if(fp){
        fseek(fp, 0, SEEK_END);
        file_size = ftell(fp);
        fseek(fp, 0, SEEK_SET);
    } else {
        printf("Failed to open file.\n");
        return wave;
    }

    // Main chunk
    get_value_from_file(fp, 0, wave.header.chunk_id, 4);
    get_value_from_file(fp, 4, &wave.header.chunk_size, 4);
    get_value_from_file(fp, 8, wave.header.format, 4);

    little_endian(&wave.header.chunk_size, 4);

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
    get_value_from_file(fp, 12, wave.header.format_subchunk_id, 4);

    if(strncmp("fmt ", wave.header.format_subchunk_id, 4)){
        printf("Format subchunk not found.\n");
        fclose(fp);
        return wave;
    }

    get_value_from_file(fp, 16, &wave.header.format_subchunk_size, 4);

    get_value_from_file(fp, 20, &wave.header.audio_format, 2);
    get_value_from_file(fp, 22, &wave.header.numberof_channels, 2);

    get_value_from_file(fp, 24, &wave.header.sample_rate, 4);
    get_value_from_file(fp, 28, &wave.header.byte_rate, 4);

    get_value_from_file(fp, 32, &wave.header.block_align, 2);
    get_value_from_file(fp, 34, &wave.header.bits_per_sample, 2);

    // No code for getting extra parameters

    // Data subchunk
    get_value_from_file(fp, 20 + wave.header.format_subchunk_size, wave.header.data_subchunk_id, 4);

    if(strncmp("data", wave.header.data_subchunk_id, 4)){
        printf("Data subchunk not found.\n");
        fclose(fp);
        return wave;
    }

    get_value_from_file(fp, 24 + wave.header.format_subchunk_size, &wave.header.data_subchunk_size, 4);

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
    get_value_from_file(fp, 24 + wave.header.format_subchunk_size, wave.data, (size_t) wave.header.data_subchunk_size);

    fclose(fp);

    make_wave_struct_system_endian(&wave, 'l');

    return wave;
}

void wave_data_to_array(int32_t* sample_array, Wave* wave){
    size_t wave_data_index = 0;
    size_t sample_index = 0;
    size_t i;

    while(wave_data_index < wave->header.data_subchunk_size){
        //sample_index = wave_data_index / wave->bytes_per_sample;

        char* sample_byte_array = (char*) &sample_array[sample_index];

        i = 0;
        while(i < wave->bytes_per_sample){
            //printf("Wave: %zu, Sample: %zu.\n", wave_data_index, sample_index);
            sample_byte_array[i] = (wave->data)[wave_data_index];
            wave_data_index++;
            i++;
        }
        // Only reads channel one so skips others
        wave_data_index += wave->bytes_per_sample * (wave->header.numberof_channels - 1);
        sample_index++;

        system_endianness(&sample_array[sample_index], 'l', 4);
    }
}
