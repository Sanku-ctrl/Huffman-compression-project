#include "huffman.h"
#include <stdio.h>
#include <string.h>
#include <time.h> // For timing

void printUsage() {
    fprintf(stderr, "Usage: ./bin/huffman [mode] [input_file] [output_file]\n");
    fprintf(stderr, "Modes:\n");
    fprintf(stderr, "  -c : Compress\n");
    fprintf(stderr, "  -d : Decompress\n");
}

int main(int argc, char* argv[]) {
    // Basic argument parsing
    if (argc != 4) {
        printUsage();
        return 1;
    }

    const char* mode = argv[1];
    const char* inputPath = argv[2];
    const char* outputPath = argv[3];

    // Start timer
    clock_t start = clock();

    if (strcmp(mode, "-c") == 0) {
        // --- Compress Mode ---
        printf("Mode: Compress\n");
        printf("Input: %s\n", inputPath);
        printf("Output: %s\n", outputPath);
        
        if (api_compress_file(inputPath, outputPath) != 0) {
            fprintf(stderr, "Compression failed.\n");
            return 1;
        }

    } else if (strcmp(mode, "-d") == 0) {
        // --- Decompress Mode ---
        printf("Mode: Decompress\n");
        printf("Input: %s\n", inputPath);
        printf("Output: %s\n", outputPath);

        if (api_decompress_file(inputPath, outputPath) != 0) {
            fprintf(stderr, "Decompression failed.\n");
            return 1;
        }

    } else {
        // --- Invalid Mode ---
        fprintf(stderr, "Error: Invalid mode '%s'\n", mode);
        printUsage();
        return 1;
    }

    // Stop timer
    clock_t end = clock();
    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;

    printf("Operation finished in %.4f seconds.\n", time_spent);

    return 0;
}
