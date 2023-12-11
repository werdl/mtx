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

int len(int * buf) {
    size_t i = 0;
    while (buf[i]) i++;
    return i;
}

int write(FILE *ptr, int * buffer) {
    int numInts = len(buffer);
    int numBytes;

    // Convert ints to uint8_t array
    uint8_t *convertedArray = int_to_u8(buffer, numInts, &numBytes);

    fwrite(convertedArray, numBytes, 1, ptr);
}

int* read(FILE *ptr) {
    fseek(ptr, 0, SEEK_END);
    long fileSize = ftell(ptr);
    fseek(ptr, 0, SEEK_SET);

    uint8_t *rawBuffer = (uint8_t *)malloc(fileSize * sizeof(uint8_t));
    if (rawBuffer == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    fread(rawBuffer, sizeof(uint8_t), fileSize, ptr);

    // Calculate the number of integers
    int numInts = (fileSize * 8) / 6;
    
    // Convert uint8_t array back to ints
    int *buffer = u8_to_int(rawBuffer, numInts, fileSize);

    // Free the allocated memory
    free(rawBuffer);

    return buffer;
}


int * string_to_mintext(char *string) {
    size_t size = 0;
    int * arr = malloc((strlen(string) + 1) * sizeof(int));

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

    return arr;
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

int write_file(const char * filename, const char * data) {
    FILE *x = fopen(filename, "wb");
    if (x == NULL) {
        return -1;
    }

    int * z = string_to_mintext(data);
    write(x, z);

    fclose(x);

    free(z);

    return 0;
}


int read_file(const char * filename) {
    FILE * file = fopen(filename, "rb");  // Use the provided filename

    if (file == NULL) {
        return -1;
    }

    int * buffer = read(file);  // Assuming read is a function you've defined
        
    for (int i = 0; buffer[i] != 0; i++) {  // Change the loop condition
        printf("%c", mt_int_to_char(buffer[i]));
    }

    return 0;
}

char * read_data_from_txt(char *filename) {
    char *buffer = NULL;
    int string_size, read_size;
    FILE *handler = fopen(filename, "r");

    if (handler) {
        fseek(handler, 0, SEEK_END);
        string_size = ftell(handler);
        rewind(handler);
        buffer = (char*) malloc(sizeof(char) * (string_size + 1) );
        read_size = fread(buffer, sizeof(char), string_size, handler);
        buffer[string_size] = '\0';

        if (string_size != read_size) {
            free(buffer);
            buffer = NULL;
        }

        fclose(handler);
    }

    return buffer;
}



int main(int argc, char **argv) {
    if (!strcmp(argv[1], "data")) {
        char * in = argv[2];
        char * out = argv[3];
        if (!in || !out) {
            fprintf(stderr, "Please provide valid arguments. Try `help` for more info.\n");
            return 1;
        }

        char * content = read_data_from_txt(in);
        write_file(in, content);
    } else if (!strcmp(argv[1], "read")) {
        char * in = argv[2];
        if (!in) {
            fprintf(stderr, "Please provide valid arguments. Try `help` for more info.\n");
            return 1;
        }

        read_file(in);
    } else {
        printf("%s","\
        MINTEXT - A TEENY TINY TEXT FORMAT\
        command line usage\
        data <in> <out>\
            reads from utf-8 in, outputs mtx out\
        read <in>\
            reads mtx, outputs to stdout");
    }
    printf("\n");

    return 0;
}
