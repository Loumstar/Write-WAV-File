#ifndef ENDIANNESS_H
    #define ENDIANNESS_H

    #include <stdbool.h>
    #include <stdlib.h>

    // Checks whether the computer system runs in big or small endian.
    bool is_big_endian();

    // Rearranges a value's byte array so that the first bit is the LSB
    void* big_endian(void* value, size_t value_size);

    // Rearranges a value's byte array so that the first bit is the MSB
    void* little_endian(void* value, size_t value_size);

#endif
