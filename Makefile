CC = gcc
CFLAGS = -pedantic -Wall

BINARIES = testWriteWave

INCLUDES = -I./ \
		   -I./scripts/

testWriteWave: write_wave.test.o endianness.o wave_header.o wave.o write_wave.o
	$(CC) $(CFLAGS) -o testWriteWave write_wave.test.o endianness.o wave_header.o wave.o write_wave.o

write_wave.test.o: tests/write_wave.test.c
	$(CC) $(CFLAGS) $(INCLUDES) -c tests/write_wave.test.c

endianness.o: scripts/endianness.c
	$(CC) $(CFLAGS) $(INCLUDES) -c scripts/endianness.c

wave_header.o: scripts/wave_header.c
	$(CC) $(CFLAGS) $(INCLUDES) -c scripts/wave_header.c

wave.o: scripts/wave.c
	$(CC) $(CFLAGS) $(INCLUDES) -c scripts/wave.c

write_wave.o: write_wave.c
	$(CC) $(CFLAGS) $(INCLUDES) -c write_wave.c

.PHONY: clean

clean:
	rm -v $(BINARIES) *.o