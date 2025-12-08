#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// read byte
int readb() {
    int c = getchar();
    if (c == EOF) return -1;
    return c & 0xFF;
}

// read n bytes
int read_n(unsigned char *buf, int n) {
    for (int i = 0; i < n; i++) {
        int x = readb();
        if (x < 0) return -1;
        buf[i] = (unsigned char)x;
    }
    return 0;
}

// convert little endian
unsigned int u32(unsigned char *b) {
    return (unsigned)b[0] |
           ((unsigned)b[1] << 8) |
           ((unsigned)b[2] << 16) |
           ((unsigned)b[3] << 24);
}

unsigned int u16(unsigned char *b) {
    return (unsigned)b[0] | ((unsigned)b[1] << 8);
}

// GLOBAL STORAGE for header
unsigned int SizeOfFile;
unsigned int FormatChunkSize;
unsigned int WaveType;
unsigned int Channels;
unsigned int SampleRate;
unsigned int BytesPerSec;
unsigned int BlockAlign;
unsigned int BitsPerSample;
unsigned int SizeOfData;

// for detecting insufficient data
unsigned int bytes_consumed = 0;

int parse_header() {
    unsigned char b[4];
    unsigned char s2[2];

    // "RIFF"
    if (read_n(b, 4) < 0) {
        fprintf(stderr, "Error! \"RIFF\" not found\n");
        return 1;
    }
    if (memcmp(b, "RIFF", 4) != 0) {
        fprintf(stderr, "Error! \"RIFF\" not found\n");
        return 1;
    }
    bytes_consumed += 4;

    // size of file
    if (read_n(b, 4) < 0) return 1;
    SizeOfFile = u32(b);
    bytes_consumed += 4;
    printf("size of file: %u\n", SizeOfFile);

    // "WAVE"
    if (read_n(b, 4) < 0) return 1;
    bytes_consumed += 4;
    if (memcmp(b, "WAVE", 4) != 0) {
        fprintf(stderr, "Error! \"WAVE\" not found\n");
        return 1;
    }

    // "fmt "
    if (read_n(b, 4) < 0) return 1;
    bytes_consumed += 4;
    if (memcmp(b, "fmt ", 4) != 0) {
        fprintf(stderr, "Error! \"fmt \" not found\n");
        return 1;
    }

    // format chunk size
    if (read_n(b, 4) < 0) return 1;
    FormatChunkSize = u32(b);
    bytes_consumed += 4;
    printf("size of format chunk: %u\n", FormatChunkSize);
    if (FormatChunkSize != 16) {
        fprintf(stderr, "Error! size of format chunk should be 16\n");
        return 1;
    }

    // WAVE type
    if (read_n(s2, 2) < 0) return 1;
    WaveType = u16(s2);
    bytes_consumed += 2;
    printf("WAVE type format: %u\n", WaveType);
    if (WaveType != 1) {
        fprintf(stderr, "Error! WAVE type format should be 1\n");
        return 1;
    }

    // mono/stereo
    if (read_n(s2, 2) < 0) return 1;
    Channels = u16(s2);
    bytes_consumed += 2;
    printf("mono/stereo: %u\n", Channels);
    if (!(Channels == 1 || Channels == 2)) {
        fprintf(stderr, "Error! mono/stereo should be 1 or 2\n");
        return 1;
    }

    // sample rate
    if (read_n(b, 4) < 0) return 1;
    SampleRate = u32(b);
    bytes_consumed += 4;
    printf("sample rate: %u\n", SampleRate);

    // bytes/sec
    if (read_n(b, 4) < 0) return 1;
    BytesPerSec = u32(b);
    bytes_consumed += 4;
    printf("bytes/sec: %u\n", BytesPerSec);

    // block alignment
    if (read_n(s2, 2) < 0) return 1;
    BlockAlign = u16(s2);
    bytes_consumed += 2;
    printf("block alignment: %u\n", BlockAlign);

    if (BytesPerSec != SampleRate * BlockAlign) {
        fprintf(stderr, "Error! bytes/second should be sample rate x block alignment\n");
        return 1;
    }

    // bits per sample
    if (read_n(s2, 2) < 0) return 1;
    BitsPerSample = u16(s2);
    bytes_consumed += 2;
    printf("bits/sample: %u\n", BitsPerSample);

    if (!(BitsPerSample == 8 || BitsPerSample == 16)) {
        fprintf(stderr, "Error! bits/sample should be 8 or 16\n");
        return 1;
    }

    unsigned int expected_align = (BitsPerSample / 8) * Channels;
    if (BlockAlign != expected_align) {
        fprintf(stderr, "Error! block alignment should be bits per sample / 8 x mono/stereo\n");
        return 1;
    }

    // "data"
    if (read_n(b, 4) < 0) return 1;
    bytes_consumed += 4;
    if (memcmp(b, "data", 4) != 0) {
        fprintf(stderr, "Error! \"data\" not found\n");
        return 1;
    }

    // size of data
    if (read_n(b, 4) < 0) return 1;
    SizeOfData = u32(b);
    bytes_consumed += 4;
    printf("size of data chunk: %u\n", SizeOfData);

    // check for insufficient data
    unsigned int total_expected = SizeOfFile + 8;
    unsigned int data_needed = bytes_consumed + SizeOfData;

    if (data_needed > total_expected) {
        fprintf(stderr, "Error! insufficient data\n");
        return 1;
    }

    // check for extra bytes past end
    // here we do not read them; main logic will detect later

    return 0;
}

/* ---------------- INFO ---------------- */

void run_info() {
    if (parse_header() != 0)
        exit(1);

    // Now check for extra bytes beyond file size
    unsigned int total_expected = SizeOfFile + 8;
    unsigned int count = bytes_consumed;

    while (count < total_expected) {
        int x = readb();
        if (x < 0) {
            fprintf(stderr, "Error! insufficient data\n");
            exit(1);
        }
        count++;
    }

    // If more bytes exist → error
    int x = readb();
    if (x != -1) {
        fprintf(stderr, "Error! bad file size (found data past the expected end of file)\n");
        exit(1);
    }
}

/* ---------------- MAIN ---------------- */

int main(int argc, char *argv[]) {
    if (argc < 2) return 1;

    if (strcmp(argv[1], "info") == 0) {
        run_info();
        return 0;
    }

    // other commands will be added next
    return 0;
}
