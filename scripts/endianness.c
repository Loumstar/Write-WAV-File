#include "endianness.h"

bool is_big_endian(){
    uint16_t test = 1;
    char *p = (char*) &test;

    return (bool) ~p[0];
}

void reverse_endianness(void* value, size_t value_size){
    char result[32];

    for(size_t i = 0; i < value_size; i++){
        result[i] = ((char*)value)[value_size - i - 1];
    }
    for(size_t i = 0; i < value_size; i++){
        ((char*)value)[i] = result[i];
    }
}

void big_endian(void* value, size_t value_size){
    if(!is_big_endian()) reverse_endianness(value, value_size);
}

void little_endian(void* value, size_t value_size){
    if(is_big_endian()) reverse_endianness(value, value_size);
}
