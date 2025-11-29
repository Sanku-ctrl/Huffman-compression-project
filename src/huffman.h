#ifndef HUFFMAN_H
#define HUFFMAN_H

// Include standard libraries needed for types (size_t) and (NULL)
#include <stddef.h> 

// --- Data Structures ---

// A node in the Huffman Tree
// Note: 'unsigned char' is used for 'data' to handle all 256 possible byte values.
typedef struct Node {
    unsigned char data;        // Character (for leaf nodes)
    unsigned long long freq;   // Frequency of the character (use long long for large files)
    struct Node *left, *right; // Left and right children
} Node;

// A node for the Min Heap
// This wraps a Huffman Tree node to be used in the heap
typedef struct MinHeapNode {
    Node* huffmanNode;
} MinHeapNode;

// The Min Heap
typedef struct MinHeap {
    unsigned size;     // Current size of heap
    unsigned capacity; // Capacity of heap
    MinHeapNode** array; // Array of MinHeapNode pointers
} MinHeap;


// Core Logic Prototypes (Internal to huffman.c) ---

// (These are the helper functions you will implement in huffman.c)

// Node utility
Node* createNode(unsigned char data, unsigned long long freq);

// MinHeap utilities
MinHeap* createMinHeap(unsigned capacity);
void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b);
void minHeapify(MinHeap* minHeap, int idx);
Node* extractMin(MinHeap* minHeap);
void insertMinHeap(MinHeap* minHeap, Node* huffmanNode);
void buildMinHeap(MinHeap* minHeap);
int isMinHeapSizeOne(MinHeap* minHeap);

// Huffman Tree utilities
Node* buildHuffmanTree(unsigned long long freqTable[]);
int isLeaf(Node* root);
void freeTree(Node* root);

// Code generation utilities
void generateCodes(Node* root, char* codeMap[256], char buffer[], int top);

// Main File I/O Functions
void compressFile(const char* inputPath, const char* outputPath);
void decompressFile(const char* inputPath, const char* outputPath);


// --- Public API Functions (for Python ctypes) ---
// These are the "clean" functions our Python wrapper will call.
// They return 0 on success and -1 on failure.
#ifdef __cplusplus
extern "C" {
#endif

// Compresses a file. Returns 0 on success, -1 on error.
int api_compress_file(const char* inputPath, const char* outputPath);

// Decompresses a file. Returns 0 on success, -1 on error.
int api_decompress_file(const char* inputPath, const char* outputPath);

#ifdef __cplusplus
}
#endif

#endif // HUFFMAN_H
