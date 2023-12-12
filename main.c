#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#define BITS_PER_VALUE 6
#define MAXCHARS 100000

uint8_t* int_to_u8(const int *input) {
    int numBytes = (BITS_PER_VALUE * sizeof(input) + MAXCHARS) / 8; // if the input is less than MAXCHARS, we won't store the whole thing
    uint8_t *output = (uint8_t *)malloc(numBytes * sizeof(uint8_t));
    uint8_t *final = (uint8_t *)malloc(numBytes * sizeof(uint8_t));

    for (int i = 0; ; ++i) {
        int bitOffset = i * BITS_PER_VALUE;
        int byteIndex = bitOffset / 8;
        int bitIndex = bitOffset % 8;

        if (byteIndex >= numBytes) {
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
            break;
        }

        output[i] = (input[byteIndex] >> bitIndex) & 0x3F;

        if (bitIndex > 2 && byteIndex < numBytes - 1) {
            output[i] |= (input[byteIndex + 1] << (8 - bitIndex)) & 0x3F;
        }
    }

    return output;
}

int get_pos(const char *str, char target) {
    int index = -1; // Initialize to -1 to indicate not found

    for (int i = 0; str[i] != '\0'; i++) {
        if (str[i] == target) {
            index = i;
            break;
        }
    }

    return index;
}

char *chars = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789 ";

int char_mt_int(char c) {
    if (strchr(chars, c)) {
        return get_pos(chars, c) + 1;
    } else {
        return -1; // Invalid character
    }
}

char mt_int_char(int num) {
    int length = strlen(chars);

    if (num >= 1 && num <= length) {
        return chars[num - 1];
    } else {
        return '\0'; // Invalid number
    }
}


int len(int * arr) {
    int i = 0;
    while (arr[i]) i++;
    return i;
}

int lent(uint8_t * arr) {
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
        if (input[i]<64 && input[i]!=0) {
            output[i] = mt_int_char(input[i]);
        } 
    }
    output[strlen(output)]='\0';
    return output;
}

char * read(const char * filename) {
    FILE * file = fopen(filename, "rb");
    if (file != NULL) {
        fseek(file, 0, SEEK_END);
        long file_size = ftell(file);
        fseek(file, 0, SEEK_SET);

        uint8_t *readArray = (uint8_t *)malloc((file_size + 1) * sizeof(uint8_t));
        fread(readArray, sizeof(uint8_t), file_size, file);

        // Ensure null termination
        readArray[file_size] = '\0';

        int *decodedOutput = u8_to_int(readArray);

        fclose(file);

        return mt_to_string(decodedOutput);
    } else {
        printf("Error opening file for reading\n");
        return NULL;
    }
}

char * contents(const char * filename) {
    FILE * ptr;

    ptr = fopen(filename, "r");

    char * input = malloc(1);

    char ch;
    while ((ch = fgetc(ptr)) != EOF) {
        input = realloc(input, (strlen(input) + 2) * sizeof(char));
        input[strlen(input)] = ch;
        input[strlen(input)] = '\0';
    }

    // close the file
    fclose(ptr);

    return input;
}

int write(const char * filename, const char * data) {
    int * input = string_to_mt(data);
    uint8_t *convertedArray = int_to_u8(input);

    // Writing to a binary file
    FILE *file = fopen(filename, "wb");
    if (file != NULL) {
        size_t arraySize = lent(convertedArray); // Use lent to get the size in bytes
        fwrite(convertedArray, sizeof(uint8_t), arraySize, file);
        fclose(file);
        free(input);  // Free dynamically allocated memory
        free(convertedArray);  // Free dynamically allocated memory
    } else {
        printf("Error opening file for writing\n");
        return -1;
    }

    return 0;
}

uint8_t * str_mt(const char * data) {
    return int_to_u8(string_to_mt(data));
}
char * mt_str(uint8_t * data) {
    return mt_to_string(u8_to_int(data));
}
#define error(s) do {\
    fprintf(stderr, "Error - %s", s);\
    exit(EXIT_FAILURE);\
} while (0)

int main(int argc, char ** argv) {
    if (argv[1]) {
        char * command = argv[1];

        if (!strcmp(command, "write")) {
            if (!argv[2] || !argv[3]) {
                error("Wrong arguments");
            }
            exit(
                write(
                    argv[3], // outfile
                    contents(argv[2]) // contents of infile
                    ));
        } else if (!strcmp(command, "read")) {
            if (!argv[2]) {
                error("Wrong arguments");
            }
            printf("%s", read(
                argv[2] // infile
            ));
            exit(EXIT_FAILURE);
        }
    }
    printf("%s", "mintext -- a tiny text format\n\
write <in> <out>\n\
    reads from infile (text), outputs to outfile (mtx)\n\
read <in>\n\
    reads from infile (mtx)\n"); // default help page
    return 0;
}