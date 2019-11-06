CC = gcc
CFLAGS = -pedantic -Wall

OBJECTS = endianness.o wave_header.o wave.o wave_file.o
BINARIES = testReadWave testWriteWave

testReadWave: wave_file.test_read.o endianness.o wave_header.o wave.o wave_file.o
	$(CC) $(CFLAGS) -o testReadWave wave_file.test_read.o endianness.o wave_header.o wave.o wave_file.o

testWriteWave: wave_file.test_write.o endianness.o wave_header.o wave.o wave_file.o
	$(CC) $(CFLAGS) -o testWriteWave wave_file.test_write.o endianness.o wave_header.o wave.o wave_file.o

wave_file.test_read.o: tests/wave_file.test_read.c
	$(CC) $(CFLAGS) -c tests/wave_file.test_read.c

wave_file.test_write.o: tests/wave_file.test_write.c
	$(CC) $(CFLAGS) -c tests/wave_file.test_write.c

endianness.o: scripts/endianness.c
	$(CC) $(CFLAGS) -c scripts/endianness.c

wave_header.o: scripts/wave_header.c
	$(CC) $(CFLAGS) -c scripts/wave_header.c

wave.o: scripts/wave.c
	$(CC) $(CFLAGS) -c scripts/wave.c

wave_file.o: wave_file.c
	$(CC) $(CFLAGS) -c wave_file.c

.PHONY: clean, objects, all

clean:
	rm -v $(BINARIES) *.o *.wav

objects:
	make $(OBJECTS)

all:
	make $(BINARIES)