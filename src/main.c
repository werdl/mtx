#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

#define SENTINEL 63

int char_mt_int(char c) {
    if (strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789. ", c)) {
        return c - 'a' + 0; // Adjust the offset based on the range of characters
    } else {
        return -1; // Invalid character
    }
}

uint8_t* int_to_u8(const int *input, int numInts, int *numBytes) {
    *numBytes = (6 * numInts + 7) / 8;
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

int write(FILE *ptr, int *buffer) {
    fwrite(buffer, sizeof(buffer), 1, ptr);
}

uint8_t * read(FILE *ptr) {
    int *buffer = malloc(100 * sizeof(int));
    // Read the array data until the sentinel (63) is encountered
    size_t size = 0;
    int element;
    while (fread(&element, sizeof(int), 1, ptr) && element != SENTINEL) {
        buffer = realloc(buffer, (size + 1) * sizeof(int));

        if (buffer == NULL) {
            perror("Memory allocation error");
            exit(EXIT_FAILURE);
        }

        buffer[size++] = element;
    }

    int numInts = size;
    int numBytes;

    // Convert ints to uint8_t array
    uint8_t *convertedArray = int_to_u8(buffer, numInts, &numBytes);

    return convertedArray;
}

uint8_t * string_to_mintext(char *string) {
    size_t size = 0;
    int *arr = malloc((strlen(string) + 1) * sizeof(int));

    if (arr == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    while (*string) {
        if (strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ", *string)) {
            arr[size++] = char_mt_int(*string);
        } else {
            perror("Invalid input...");
        }
        string++;
    }

    // Add the sentinel to the array
    arr = realloc(arr, size * sizeof(int));
    if (arr == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    // Write the sentinel itself
    arr[size] = SENTINEL;

    int numInts = size + 1;
    int numBytes;

    // Convert ints to uint8_t array
    uint8_t *convertedArray = int_to_u8(arr, numInts, &numBytes);

    return convertedArray;
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

int main(int argc, char **argv) {
    FILE *x = fopen("w.bin", "wb");

    if (x == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    int *z = string_to_mintext("hello");
    write(x, z);

    fclose(x);

    // Reopen the file for reading
    x = fopen("w.bin", "rb");

    if (x == NULL) {
        perror("Error opening file for reading");
        exit(EXIT_FAILURE);
    }

    uint8_t *buffer = read(x);
    if (buffer == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    size_t i = 0;
    while (buffer[i]) {
        printf("%c", mt_int_to_char(buffer[i]));
        i++;
    }

    fclose(x);
    free(z);
    free(buffer);

    return 0;
}
