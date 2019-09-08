#include "endianness.h"

bool is_big_endian(){
    unsigned int test = 1;
    char *p = (char*) &test;
    return !p[0];
}

void reverse_endianness(void* value, size_t value_size){
    char result[16];

    for(size_t i = 0; i < value_size; i++){
        result[i] = ((char*)value)[value_size - i - 1];
    }
    for(size_t i = 0; i < value_size; i++){
        ((char*)value)[i] = result[i];
    }
}

void* big_endian(void* value, size_t value_size){
    if(!is_big_endian()) reverse_endianness(value, value_size);
    return value;
}

void* little_endian(void* value, size_t value_size){
    if(is_big_endian()) reverse_endianness(value, value_size);
    return value;
}

void* system_endianness(void* value, char current_endianness, size_t value_size){
    if((is_big_endian() && current_endianness == 'l') || (!is_big_endian() && current_endianness == 'b')){
        reverse_endianness(value, value_size);
    }
    return value;
}
