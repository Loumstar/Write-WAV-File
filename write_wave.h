#ifndef WRITE_WAVE_H
    #define WRITE_WAVE_H

    #include "wave.h"
    #include "endianness.h"

    // Method to write a Wave instance to a file.
    void write_wave(Wave* wave, const char* filename);

#endif
