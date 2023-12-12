#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define BITS_PER_VALUE 6

uint8_t* int_to_u8(const int *input) {
    int numBytes = (BITS_PER_VALUE * sizeof(input) + 7) / 8;
    uint8_t *output = (uint8_t *)malloc(numBytes * sizeof(uint8_t));
    uint8_t *final = (uint8_t *)malloc(numBytes * sizeof(uint8_t));

    for (int i = 0; ; ++i) {
        int bitOffset = i * BITS_PER_VALUE;
        int byteIndex = bitOffset / 8;
        int bitIndex = bitOffset % 8;

        if (byteIndex >= numBytes) {
            // Reached the end of the allocated bytes
            break;
        }

        output[byteIndex] |= (input[i] & 0x3F) << bitIndex;

        if (bitIndex > 2 && byteIndex < numBytes - 1) {
            output[byteIndex + 1] |= (input[i] & 0x3F) >> (8 - bitIndex);
        }
    }

    for (int i = 0; output[i]; i++) {
        if (output[i]!=0) {
            final[i]=output[i];
        }
    }

    return final;
}

int* u8_to_int(const uint8_t *input) {
    int numBytes = 0;

    while (input[numBytes] != 0) {
        ++numBytes;
    }

    int *output = (int *)malloc(((numBytes * 8) + (BITS_PER_VALUE - 1)) / BITS_PER_VALUE * sizeof(int));

    for (int i = 0; ; ++i) {
        int bitOffset = i * BITS_PER_VALUE;
        int byteIndex = bitOffset / 8;
        int bitIndex = bitOffset % 8;

        if (byteIndex >= numBytes) {
            // Reached the end of the allocated bytes
            break;
        }

        output[i] = (input[byteIndex] >> bitIndex) & 0x3F;

        if (bitIndex > 2 && byteIndex < numBytes - 1) {
            output[i] |= (input[byteIndex + 1] << (8 - bitIndex)) & 0x3F;
        }
    }

    return output;
}

int char_mt_int(char c) {
    if (strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789. ", c)) {
        return c - 'a';
    } else {
        return -1; // Invalid character
    }
}

char mt_int_char(int mt_int) {
    if (mt_int >= 0 && mt_int <= 61) {
        return (char)(mt_int + 'a');
    } else if (mt_int == 62) {
        return ' ';
    } else {
        // Handle invalid mt_int
        return '\0';  // You can choose another character as needed
    }
}

int len(int * arr) {
    int i = 0;
    while (arr[i]) i++;
    return i;
}

int * string_to_mt(const char * input) {
    int * output = malloc((strlen(input) + 2) * sizeof(int));
    int i = 0;
    for (i = 0; i < strlen(input); i++) {
        output[i] = char_mt_int(input[i]);
    }
    output[i++] = 0;
    return output;
}

char * mt_to_string(int * input) {
    char * output = malloc((len(input) + 1) * sizeof(char));
    for (int i = 0; i < len(input); i++) {
        output[i] = mt_int_char(input[i]);
    }
    return output;
}

char * read(const char * filename) {
    FILE * file = fopen(filename, "rb");
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        uint8_t *readArray = (uint8_t *)malloc(file_size * sizeof(uint8_t));
        fread(readArray, sizeof(uint8_t), file_size, file);

        int *decodedOutput = u8_to_int(readArray);

        printf("Decoded int array: ");
        for (int i = 0; decodedOutput[i] != 0; ++i) {
            printf("%d ", decodedOutput[i]);
        }
        printf("\n");

        fclose(file);

        return mt_to_string(decodedOutput);
    } else {
        printf("Error opening file for reading\n");
        return NULL;
    }
}

int write(const char * filename, const char * data) {
    int * input = string_to_mt(data);
    uint8_t *convertedArray = int_to_u8(input);

    // Writing to a binary file
    FILE *file = fopen(filename, "wb");
    if (file != NULL) {
        fwrite(convertedArray, sizeof(uint8_t), strlen((char*)convertedArray), file);
        fclose(file);
    } else {
        printf("Error opening file for writing\n");
        return -1;
    }

    return 0;
}

int main() {
    (void)write("t.bin", "hello");
    printf("%s", read("t.bin"));
    

    return 0;
}