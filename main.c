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
        return c - 'a' + 0; // Adjust the offset based on the range of characters
    } else {
        return -1; // Invalid character
    }
}

char mt_int_to_char(int mt_int) {
    if (mt_int >= 0 && mt_int <= 61) {
        return (char)(mt_int + 'a');
    } else if (mt_int == 62) {
        return ' ';
    } else {
        // Handle invalid mt_int
        return '\0';  // You can choose another character as needed
    }
}


int main() {
    int input[] = {15, 25, 31, 42, 0}; // Added a zero at the end as a terminator
    uint8_t *convertedArray = int_to_u8(input);
    int *output = u8_to_int(convertedArray);

    printf("Original int array: ");
    for (int i = 0; input[i] != 0; ++i) {
        printf("%d ", input[i]);
    }
    printf("\n");

    printf("Converted back int array: ");
    for (int i = 0; output[i] != 0; ++i) {
        printf("%d ", output[i]);
    }
    printf("\n");

    free(convertedArray);
    free(output);

    return 0;
}