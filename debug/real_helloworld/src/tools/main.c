#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include <sys/mman.h>
#include <sys/stat.h>

#define BUFFER_SIZE 512

// Function to read raw data from a file
void read_raw_data(const char *input_file) {
    FILE *file = fopen(input_file, "rb");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    unsigned char buffer[BUFFER_SIZE];
    size_t bytes_read;

    printf("Reading from file: %s\n", input_file);
    while ((bytes_read = fread(buffer, 1, BUFFER_SIZE, file)) > 0) {
        // Process the data (in this example, just print to console)
        printf("Read %zu bytes:\n", bytes_read);
        for (size_t i = 0; i < bytes_read; ++i) {
            printf("%02x ", buffer[i]);
            if ((i + 1) % 16 == 0) printf("\n");
        }
        printf("\n");
    }

    if (ferror(file)) {
        perror("fread");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    fclose(file);
}

// Function to write raw data to a file
void write_raw_data(const char *output_file, const unsigned char *data, size_t data_size) {
    FILE *file = fopen(output_file, "wb");
    if (file == NULL) {
        perror("fopen");
        exit(EXIT_FAILURE);
    }

    size_t bytes_written = fwrite(data, 1, data_size, file);
    if (bytes_written != data_size) {
        perror("fwrite");
        fclose(file);
        exit(EXIT_FAILURE);
    }

    printf("Written %zu bytes to file: %s\n", bytes_written, output_file);

    fclose(file);
}

int main(int argc, char *argv[]) {

    if (argc != 3) {
        fprintf(stderr, "Usage: %s <input_file> <output_file>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Read raw data from input file
    read_raw_data(argv[1]);

    // Example data to write to the output file
    unsigned char data[] = {
        0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20, 0x46, 0x72, 0x65, 0x65,
        0x42, 0x53, 0x44, 0x21, 0x0a
    };
    size_t data_size = sizeof(data);

    // Write raw data to output file
    write_raw_data(argv[2], data, data_size);

    return EXIT_SUCCESS;
}
