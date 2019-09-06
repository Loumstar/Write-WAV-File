#ifndef ENDIANNESS_H
    #define ENDIANNESS_H

    #include <stdbool.h>
    #include <stdlib.h>

    bool is_big_endian();

    void big_endian(void* value, size_t value_size);

    void little_endian(void* value, size_t value_size);

#endif
