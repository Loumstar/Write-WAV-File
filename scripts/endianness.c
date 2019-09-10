#include "endianness.h"

bool is_big_endian(){
    // Creates 00 01 in big endian
    unsigned short int test = 1;
    // Create a pointer to an array of these two bytes
    char *p = (char*) &test;
    // Returns true if first byte in array is 00
    return !p[0];
}

void reverse_endianness(void* value, size_t value_size){
    char result[4];
    // Places the bytes of the value in reverse order into result
    for(size_t i = 0; i < value_size; i++){
        result[i] = ((char*)value)[value_size - i - 1];
    }
    // Copies the bytes of result back into the value
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
