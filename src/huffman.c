#include "huffman.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_TREE_HT 256 // Max height of tree (for code buffers)
#define NUM_CHARS 256   // Number of possible ASCII/byte values

// A magic number to identify our compressed file format
// (Helps prevent decompressing the wrong file)
const unsigned int MAGIC_NUMBER = 0x48554646; // 'HUFF'

// --- Node Utility ---

Node* createNode(unsigned char data, unsigned long long freq) {
    Node* temp = (Node*)malloc(sizeof(Node));
    if (!temp) {
        perror("malloc error (createNode)");
        exit(EXIT_FAILURE);
    }
    temp->left = temp->right = NULL;
    temp->data = data;
    temp->freq = freq;
    return temp;
}

// --- Min Heap Utilities ---

MinHeap* createMinHeap(unsigned capacity) {
    MinHeap* minHeap = (MinHeap*)malloc(sizeof(MinHeap));
    if (!minHeap) {
        perror("malloc error (createMinHeap)");
        exit(EXIT_FAILURE);
    }
    minHeap->size = 0;
    minHeap->capacity = capacity;
    minHeap->array = (MinHeapNode**)malloc(capacity * sizeof(MinHeapNode*));
    if (!minHeap->array) {
        perror("malloc error (minHeap->array)");
        exit(EXIT_FAILURE);
    }
    return minHeap;
}

void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b) {
    MinHeapNode* t = *a;
    *a = *b;
    *b = t;
}

void minHeapify(MinHeap* minHeap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < minHeap->size && minHeap->array[left]->huffmanNode->freq < minHeap->array[smallest]->huffmanNode->freq)
        smallest = left;

    if (right < minHeap->size && minHeap->array[right]->huffmanNode->freq < minHeap->array[smallest]->huffmanNode->freq)
        smallest = right;

    if (smallest != idx) {
        swapMinHeapNode(&minHeap->array[smallest], &minHeap->array[idx]);
        minHeapify(minHeap, smallest);
    }
}

int isMinHeapSizeOne(MinHeap* minHeap) {
    return (minHeap->size == 1);
}

Node* extractMin(MinHeap* minHeap) {
    if (minHeap->size == 0) return NULL;

    MinHeapNode* temp = minHeap->array[0];
    minHeap->array[0] = minHeap->array[minHeap->size - 1];
    --minHeap->size;
    minHeapify(minHeap, 0);

    Node* huffmanNode = temp->huffmanNode;
    free(temp); // Free the heap node wrapper, not the huffman node
    return huffmanNode;
}

void insertMinHeap(MinHeap* minHeap, Node* huffmanNode) {
    if (minHeap->size == minHeap->capacity) {
        // This shouldn't happen with our logic, but it's good practice
        fprintf(stderr, "Heap is full. Cannot insert.\n");
        return;
    }

    MinHeapNode* minHeapNode = (MinHeapNode*)malloc(sizeof(MinHeapNode));
    minHeapNode->huffmanNode = huffmanNode;

    ++minHeap->size;
    int i = minHeap->size - 1;
    minHeap->array[i] = minHeapNode;

    // Fix the min heap property
    while (i && minHeap->array[i]->huffmanNode->freq < minHeap->array[(i - 1) / 2]->huffmanNode->freq) {
        swapMinHeapNode(&minHeap->array[i], &minHeap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

void buildMinHeap(MinHeap* minHeap) {
    int n = minHeap->size - 1;
    for (int i = (n - 1) / 2; i >= 0; --i)
        minHeapify(minHeap, i);
}

// --- Huffman Tree Utilities ---

int isLeaf(Node* root) {
    return !(root->left) && !(root->right);
}

Node* buildHuffmanTree(unsigned long long freqTable[]) {
    MinHeap* minHeap = createMinHeap(NUM_CHARS);
    int charCount = 0;

    // Create a leaf node for each character with non-zero frequency
    // and add it to the min heap.
    for (int i = 0; i < NUM_CHARS; ++i) {
        if (freqTable[i] > 0) {
            Node* node = createNode((unsigned char)i, freqTable[i]);
            MinHeapNode* minHeapNode = (MinHeapNode*)malloc(sizeof(MinHeapNode));
            minHeapNode->huffmanNode = node;
            minHeap->array[charCount] = minHeapNode;
            charCount++;
        }
    }
    minHeap->size = charCount;
    
    // Handle edge case: empty file
    if (minHeap->size == 0) {
        free(minHeap->array);
        free(minHeap);
        return NULL; // Or a special node
    }
    
    // Handle edge case: file with only one unique character
    if (minHeap->size == 1) {
        Node* root = extractMin(minHeap);
        // Create a dummy parent node
        Node* parent = createNode('$', root->freq); 
        parent->left = root;
        insertMinHeap(minHeap, parent);
    } else {
        buildMinHeap(minHeap);
    }


    // Iterate while size of heap doesn't become 1
    while (!isMinHeapSizeOne(minHeap)) {
        // Extract the two minimum freq items from min heap
        Node* left = extractMin(minHeap);
        Node* right = extractMin(minHeap);

        // Create a new internal node with freq = sum of two nodes' freq.
        // Use a non-character value ($) for internal nodes.
        Node* top = createNode('$', left->freq + right->freq);
        top->left = left;
        top->right = right;

        insertMinHeap(minHeap, top);
    }

    // The remaining node is the root node
    Node* root = extractMin(minHeap);
    
    // Free the heap structure
    free(minHeap->array);
    free(minHeap);

    return root;
}

void freeTree(Node* root) {
    if (root == NULL) return;
    freeTree(root->left);
    freeTree(root->right);
    free(root);
}

// --- Code Generation ---

// Fills codeMap with '0' and '1' strings representing the codes
void generateCodes(Node* root, char* codeMap[NUM_CHARS], char buffer[], int top) {
    if (root == NULL) return;

    // If this is a left child, add '0' to buffer
    if (root->left) {
        buffer[top] = '0';
        generateCodes(root->left, codeMap, buffer, top + 1);
    }

    // If this is a right child, add '1' to buffer
    if (root->right) {
        buffer[top] = '1';
        generateCodes(root->right, codeMap, buffer, top + 1);
    }

    // If this is a leaf node, it contains a character
    if (isLeaf(root)) {
        buffer[top] = '\0'; // Null-terminate the string
        codeMap[root->data] = (char*)malloc(strlen(buffer) + 1);
        if(!codeMap[root->data]) {
            perror("malloc error (codeMap)");
            exit(EXIT_FAILURE);
        }
        strcpy(codeMap[root->data], buffer);
    }
}

// --- Main File I/O Functions ---

void compressFile(const char* inputPath, const char* outputPath) {
    FILE *in = fopen(inputPath, "rb"); // Read in binary mode
    if (!in) {
        perror("Failed to open input file");
        return;
    }

    // 1. Count frequencies
    unsigned long long freqTable[NUM_CHARS] = {0};
    unsigned long long originalCharCount = 0;
    int c;
    while ((c = fgetc(in)) != EOF) {
        freqTable[c]++;
        originalCharCount++;
    }
    
    // Handle empty file
    if (originalCharCount == 0) {
        fclose(in);
        // Create an empty output file
        FILE *out = fopen(outputPath, "wb");
        if(out) fclose(out);
        printf("Input file is empty. Created empty output file.\n");
        return;
    }

    // 2. Build the Huffman Tree
    Node* root = buildHuffmanTree(freqTable);

    // 3. Generate codes
    char* codeMap[NUM_CHARS] = {0}; // Array of string pointers
    char buffer[MAX_TREE_HT];
    generateCodes(root, codeMap, buffer, 0);

    // 4. Open output file for writing (binary mode)
    FILE *out = fopen(outputPath, "wb");
    if (!out) {
        perror("Failed to open output file");
        fclose(in);
        return;
    }

    // 5. Write the "header"
    //    a. Magic number
    fwrite(&MAGIC_NUMBER, sizeof(unsigned int), 1, out);
    //    b. Original character count (for decompression)
    fwrite(&originalCharCount, sizeof(unsigned long long), 1, out);
    //    c. The frequency table (this is how we rebuild the tree)
    fwrite(freqTable, sizeof(unsigned long long), NUM_CHARS, out);

    // 6. Re-read input file and write compressed bits
    fseek(in, 0, SEEK_SET); // Go back to start of input file

    unsigned char bitBuffer = 0;
    int bitCount = 0;

    while ((c = fgetc(in)) != EOF) {
        char* code = codeMap[c];
        for (int i = 0; code[i] != '\0'; ++i) {
            // Add the bit to our buffer
            if (code[i] == '1') {
                bitBuffer |= (1 << (7 - bitCount));
            }
            bitCount++;

            // If buffer is full (8 bits), write it to file
            if (bitCount == 8) {
                fwrite(&bitBuffer, sizeof(unsigned char), 1, out);
                bitBuffer = 0;
                bitCount = 0;
            }
        }
    }

    // Write any remaining bits (padding)
    if (bitCount > 0) {
        fwrite(&bitBuffer, sizeof(unsigned char), 1, out);
    }

    // 7. Clean up
    fclose(in);
    fclose(out);
    freeTree(root);
    for (int i = 0; i < NUM_CHARS; ++i) {
        if (codeMap[i]) free(codeMap[i]);
    }

    printf("Compression successful.\n");
}

void decompressFile(const char* inputPath, const char* outputPath) {
    FILE *in = fopen(inputPath, "rb");
    if (!in) {
        perror("Failed to open input file");
        return;
    }

    // 1. Read and verify magic number
    unsigned int magic;
    if (fread(&magic, sizeof(unsigned int), 1, in) != 1 || magic != MAGIC_NUMBER) {
        fprintf(stderr, "Error: Not a valid .huff file or file is corrupted.\n");
        fclose(in);
        return;
    }

    // 2. Read original char count and frequency table
    unsigned long long originalCharCount;
    unsigned long long freqTable[NUM_CHARS];
    
    if (fread(&originalCharCount, sizeof(unsigned long long), 1, in) != 1) {
         fprintf(stderr, "Error: Failed to read header.\n");
         fclose(in);
         return;
    }
    
    // Handle empty file
    if (originalCharCount == 0) {
        fclose(in);
        FILE *out = fopen(outputPath, "wb"); // Create empty file
        if (out) fclose(out);
        printf("Decompression successful (empty file).\n");
        return;
    }

    if (fread(freqTable, sizeof(unsigned long long), NUM_CHARS, in) != NUM_CHARS) {
        fprintf(stderr, "Error: Failed to read frequency table.\n");
        fclose(in);
        return;
    }

    // 3. Rebuild the Huffman Tree
    Node* root = buildHuffmanTree(freqTable);
    if (!root) {
        fprintf(stderr, "Error: Failed to rebuild Huffman tree.\n");
        fclose(in);
        return;
    }

    // 4. Open output file
    FILE *out = fopen(outputPath, "wb");
    if (!out) {
        perror("Failed to open output file");
        fclose(in);
        freeTree(root);
        return;
    }

    // 5. Read compressed data bit by bit and write to output
    Node* currentNode = root;
    unsigned char bitBuffer;
    int bitCount = 0;
    unsigned long long charsWritten = 0;

    while (charsWritten < originalCharCount && fread(&bitBuffer, sizeof(unsigned char), 1, in) == 1) {
        bitCount = 0;
        while (charsWritten < originalCharCount && bitCount < 8) {
            // Check the current bit
            if ((bitBuffer >> (7 - bitCount)) & 1) {
                currentNode = currentNode->right;
            } else {
                currentNode = currentNode->left;
            }
            bitCount++;

            if (isLeaf(currentNode)) {
                fputc(currentNode->data, out);
                charsWritten++;
                currentNode = root; // Go back to the root for the next char
            }
        }
    }

    // 6. Clean up
    fclose(in);
    fclose(out);
    freeTree(root);

    printf("Decompression successful.\n");
}


// --- Public API Functions (for Python ctypes) ---

int api_compress_file(const char* inputPath, const char* outputPath) {
    // A real implementation would have try/catch or better error checking.
    // For C, we check file pointers and return -1 on failure.
    
    // We can't really "catch" a malloc fail, as we exit(), but we can check
    // file I/O. For this project, we'll keep it simple.
    
    FILE *in = fopen(inputPath, "rb");
    if (!in) {
        perror("API: Failed to open input file");
        return -1;
    }
    fclose(in); // We only checked if it exists, compressFile will reopen

    compressFile(inputPath, outputPath);
    
    FILE *out = fopen(outputPath, "rb");
    if (!out) {
        // compressFile must have failed
        return -1;
    }
    fclose(out);
    
    return 0; // Return 0 on success
}

int api_decompress_file(const char* inputPath, const char* outputPath) {
    FILE *in = fopen(inputPath, "rb");
    if (!in) {
        perror("API: Failed to open input file");
        return -1;
    }
    fclose(in);

    decompressFile(inputPath, outputPath);

    // We should check if the output file was successfully created
    // and is not empty (unless original was empty)
    FILE *out = fopen(outputPath, "rb");
    if (!out) {
        // decompressFile must have failed
        return -1;
    }
    fclose(out);
    
    return 0; // Return 0 on success
}
