#ifndef ENDIANNESS_H
    #define ENDIANNESS_H

    #include <stdbool.h>
    #include <stdlib.h>

    //Note size_t size in each method is measured in bytes.

    // Checks whether the computer system is in big or small endian.
    bool is_big_endian();

    // Rearranges a value's byte array so that the first bit is the LSB
    void* big_endian(void* value, size_t value_size);

    // Rearranges a value's byte array so that the first bit is the MSB
    void* little_endian(void* value, size_t value_size);

    /*
    Converts a values endianness back to the system endianness.
    current_endianness must be 'l' for little or 'b' for big.
    */

    void* system_endian(void* value, char current_endianness, size_t value_size);

#endif
