

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>

int char_mt_int(char c) {
    if (strchr("abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789. ", c)) {
        return c - 'a' + 0; // Adjust the offset based on the range of characters
    } else {
        return -1; // Invalid character
    }
}

int write(FILE *ptr, unsigned short *buffer) {
    // Write the array data until the sentinel (63) is encountered
    size_t i = 0;
    while (buffer[i] && buffer[i] != 63) {
        fwrite(&buffer[i], sizeof(unsigned short), 1, ptr);
        i++;
    }
    // Write the sentinel itself
    fwrite(&buffer[i], sizeof(unsigned short), 1, ptr);
    return 0;
}

int read(FILE *ptr, unsigned short **buffer) {
    // Read the array data until the sentinel (63) is encountered
    size_t size = 0;
    unsigned short element;
    while (fread(&element, sizeof(unsigned short), 1, ptr) && element != 63) {
        *buffer = realloc(*buffer, (size + 1) * sizeof(unsigned short));

        if (*buffer == NULL) {
            perror("Memory allocation error");
            exit(EXIT_FAILURE);
        }

        (*buffer)[size++] = element;
    }
    
    // No need to add the sentinel again; it's already part of the loop condition

    return 0;
}

unsigned short *string_to_mintext(char *string) {
    size_t size = 0;
    unsigned short *arr = malloc((strlen(string) + 1) * sizeof(unsigned short));

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
    arr = realloc(arr, (size + 1) * sizeof(unsigned short));
    if (arr == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }
    arr[size] = 63;

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

int main(int argc, char **argv) {
    FILE *x = fopen("test.bin", "wb");

    if (x == NULL) {
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    unsigned short *z = string_to_mintext("hello");
    write(x, z);

    fclose(x);

    // Reopen the file for reading
    x = fopen("test.bin", "rb");

    if (x == NULL) {
        perror("Error opening file for reading");
        exit(EXIT_FAILURE);
    }

    unsigned short *buffer = malloc(1000);
    if (buffer == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    read(x, &buffer);

    size_t i = 0;
    while (buffer[i] && buffer[i] != 63) {
        printf("%c\n", mt_int_to_char(buffer[i]));  // Use %hu for unsigned short
        i++;
    }

    fclose(x);
    free(z);
    free(buffer);

    return 0;
}
