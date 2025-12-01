#include <stdio.h>
#include <string.h>
#include <stdlib.h>

int main() {
    char riff[4];
    unsigned int file_size;
    char wave[4];
    char fmt[4];
    unsigned int fmt_size;
    unsigned short format;
    unsigned short channels;
    unsigned int sample_rate;
    unsigned int bytes_per_sec;
    unsigned short block_align;
    unsigned short bits_per_sample;
    char data[4];
    unsigned int data_size;

    if (fread(riff, 1, 4, stdin) != 4) { fprintf(stderr, "Error reading RIFF\n"); return 1; }
    if (strncmp(riff, "RIFF", 4) != 0) { fprintf(stderr, "Error! \"RIFF\" not found\n"); return 1; }

    if (fread(&file_size, 4, 1, stdin) != 1) { fprintf(stderr, "Error reading file size\n"); return 1; }
    if (fread(wave, 1, 4, stdin) != 4) { fprintf(stderr, "Error reading WAVE\n"); return 1; }
    if (strncmp(wave, "WAVE", 4) != 0) { fprintf(stderr, "Error! \"WAVE\" not found\n"); return 1; }

    if (fread(fmt, 1, 4, stdin) != 4) { fprintf(stderr, "Error reading fmt\n"); return 1; }
    if (strncmp(fmt, "fmt ", 4) != 0) { fprintf(stderr, "Error! \"fmt \" not found\n"); return 1; }

    if (fread(&fmt_size, 4, 1, stdin) != 1) return 1;
    if (fread(&format, 2, 1, stdin) != 1) return 1;
    if (fread(&channels, 2, 1, stdin) != 1) return 1;
    if (fread(&sample_rate, 4, 1, stdin) != 1) return 1;
    if (fread(&bytes_per_sec, 4, 1, stdin) != 1) return 1;
    if (fread(&block_align, 2, 1, stdin) != 1) return 1;
    if (fread(&bits_per_sample, 2, 1, stdin) != 1) return 1;

    if (fread(data, 1, 4, stdin) != 4) { fprintf(stderr, "Error reading data\n"); return 1; }
    if (strncmp(data, "data", 4) != 0) { fprintf(stderr, "Error! \"data\" not found\n"); return 1; }

    if (fread(&data_size, 4, 1, stdin) != 1) return 1;

    printf("size of file: %u\n", file_size + 8);
    printf("size of format chunk: %u\n", fmt_size);
    printf("WAVE type format: %u\n", format);
    printf("mono/stereo: %u\n", channels);
    printf("sample rate: %u\n", sample_rate);
    printf("bytes/sec: %u\n", bytes_per_sec);
    printf("block alignment: %u\n", block_align);
    printf("bits/sample: %u\n", bits_per_sample);
    printf("size of data chunk: %u\n", data_size);

    return 0;
}
