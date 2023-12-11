#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

uint8_t* int_to_u8(const int *input, int numInts, int *numBytes) {
    *numBytes = (BITS_PER_VALUE * numInts + 7) / 8;
    uint8_t *output = (uint8_t *)malloc(*numBytes * sizeof(uint8_t));

    for (int i = 0; i < numInts; ++i) {
        int bitOffset = i * 6;
        int byteIndex = bitOffset / 8;
        int bitIndex = bitOffset % 8;

        output[byteIndex] |= (input[i] & 0x3F) << bitIndex;

        if (bitIndex > 2 && byteIndex < *numBytes - 1) {
            output[byteIndex + 1] |= (input[i] & 0x3F) >> (8 - bitIndex);
        }
    }

    return output;
}

int* u8_to_int(const uint8_t *input, int numInts, int numBytes) {
    int *output = (int *)malloc(numInts * sizeof(int));

    for (int i = 0; i < numInts; ++i) {
        int bitOffset = i * 6;
        int byteIndex = bitOffset / 8;
        int bitIndex = bitOffset % 8;
        output[i] = (input[byteIndex] >> bitIndex) & 0x3F;
        if (bitIndex > 2 && byteIndex < numBytes - 1) {
            output[i] |= (input[byteIndex + 1] << (8 - bitIndex)) & 0x3F;
        }
    }

    return output;
}

int main() {
    int input[] = {15, 25, 31, 42};
    int numInts = sizeof(input) / sizeof(input[0]);
    int numBytes;

    // Convert ints to uint8_t array
    uint8_t *convertedArray = convertIntsToUint8Array(input, numInts, &numBytes);

    // Convert back to int array
    int *output = convertUint8ArrayToInts(convertedArray, numInts, numBytes);

    printf("Original int array: ");
    for (int i = 0; i < numBytes; ++i)
        printf("%02X ", convertedArray[i]);
    printf("\n");

    free(convertedArray);
    free(output);

    return 0;
}
