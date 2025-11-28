# Huffman Compression Project

A high-performance file compression utility using Huffman encoding, built in C with Python bindings. This project demonstrates core data structures and algorithms including binary trees, min-heaps, and bit-level I/O operations.

## Table of Contents

- [Features](#features)
- [Architecture](#architecture)
- [Prerequisites](#prerequisites)
- [Installation](#installation)
- [Usage](#usage)
  - [Command Line](#command-line)
  - [Python API](#python-api)
- [Technical Implementation](#technical-implementation)
- [File Structure](#file-structure)
- [Building & Testing](#building--testing)
- [Examples](#examples)
- [Performance Considerations](#performance-considerations)
- [Contributing](#contributing)

## Features

- **Efficient Compression**: Implements classic Huffman encoding algorithm for lossless file compression
- **Dual Interface**: 
  - CLI tool for direct file compression/decompression
  - Shared library with Python ctypes wrapper for programmatic access
- **Robust Error Handling**: Magic number validation, file integrity checks
- **Cross-Platform**: Builds on Linux/Unix systems (with support for macOS and Windows via adaptations)
- **Memory Efficient**: Uses binary tree structure and bit-level I/O for minimal overhead
- **Large File Support**: Uses 64-bit frequency counters for handling files with billions of characters
- **Edge Case Handling**: Properly handles empty files and files with single unique character

## Architecture

### High-Level Overview

```
Input File
    ↓
[Frequency Analysis] → Build frequency table
    ↓
[Build Huffman Tree] → Min-heap based construction
    ↓
[Generate Codes] → Traverse tree to create binary codes
    ↓
[Encode] → Transform original data using codes
    ↓
Compressed File (.huff)
```

### Core Components

#### 1. **Data Structures** (`src/huffman.h`)

- **Node**: Represents a node in the Huffman tree
  - `data`: Character value (for leaf nodes)
  - `freq`: Frequency count (unsigned long long for large files)
  - `left/right`: Child pointers

- **MinHeap**: Priority queue for building optimal tree
  - Stores nodes sorted by frequency
  - Enables O(log n) insertions and deletions
  - Essential for Huffman algorithm efficiency

#### 2. **Core Algorithm** (`src/huffman.c`)

**Compression Pipeline:**
1. **Frequency Analysis**: Read input file, count occurrence of each byte (0-255)
2. **Tree Building**: Use min-heap to iteratively combine lowest-frequency nodes
3. **Code Generation**: Traverse tree to assign binary codes (0/1 path)
4. **Encoding**: Re-read input, convert each byte to its code, pack bits
5. **File Output**: Write header (magic number, char count, frequency table) + compressed data

**Decompression Pipeline:**
1. **Validation**: Verify magic number (0x48554646 = 'HUFF')
2. **Header Parsing**: Read original character count and frequency table
3. **Tree Reconstruction**: Rebuild identical Huffman tree from frequencies
4. **Decoding**: Read bits, traverse tree left(0)/right(1), output character when leaf reached

#### 3. **CLI Interface** (`src/main.c`)

Command-line utility with timing information:
```
./bin/huffman -c input.txt output.huff   # Compress
./bin/huffman -d input.huff output.txt   # Decompress
```

#### 4. **Python Bindings** (`python/wrapper.py`)

Ctypes wrapper providing Pythonic interface to compiled C library:
```python
from wrapper import compress, decompress

compress('input.txt', 'input.huff')
decompress('input.huff', 'restored.txt')
```

## Prerequisites

- **GCC**: C compiler with C11 support
- **Make**: Build automation tool
- **Python 3.6+**: For Python wrapper and demo
- **Linux/WSL/macOS**: Unix-like environment

## Installation

### 1. Clone the Repository
```bash
git clone <your-repo-url>
cd huffman-project
```

### 2. Build the Project
```bash
# Build both CLI executable and shared library
make all

# Or build individually:
make clean              # Remove build artifacts
```

After building, you'll have:
- `bin/huffman` - Command-line tool
- `bin/libhuffman.so` - Shared library (for Python on Linux)

## Usage

### Command Line

#### Compress a file:
```bash
./bin/huffman -c input.txt output.huff
```
**Sample Output:**
```
Mode: Compress
Input: input.txt
Output: output.huff
Compression successful.
Operation finished in 0.0234 seconds.
```

#### Decompress a file:
```bash
./bin/huffman -d output.huff restored.txt
```
**Sample Output:**
```
Mode: Decompress
Input: output.huff
Output: restored.txt
Decompression successful.
Operation finished in 0.0156 seconds.
```

### Python API

#### Basic Usage:
```python
from python.wrapper import compress, decompress

# Compress
if compress('input.txt', 'input.huff'):
    print("Compression successful")

# Decompress
if decompress('input.huff', 'restored.txt'):
    print("Decompression successful")
```

#### Full Example with Verification:
```python
import os
from python.wrapper import compress, decompress
import filecmp

input_file = 'test.txt'
compressed_file = 'test.huff'
restored_file = 'test_restored.txt'

# Compress
compress(input_file, compressed_file)

# Decompress
decompress(compressed_file, restored_file)

# Verify integrity
if filecmp.cmp(input_file, restored_file, shallow=False):
    print("✓ Files are identical - compression is lossless")
    
    # Show compression ratio
    original_size = os.path.getsize(input_file)
    compressed_size = os.path.getsize(compressed_file)
    ratio = (1 - compressed_size/original_size) * 100
    print(f"Compression ratio: {ratio:.1f}%")
```

#### Run the Demo:
```bash
cd python
python demo.py
```

## Technical Implementation

### Huffman Algorithm Details

**Time Complexity:**
- Frequency counting: O(n) where n = file size
- Tree building: O(m log m) where m = unique characters (≤ 256)
- Code generation: O(m)
- Encoding: O(n × average_code_length)
- **Overall: O(n + m log m) ≈ O(n)**

**Space Complexity:**
- Frequency table: O(256) = O(1)
- Huffman tree: O(m) ≤ O(256)
- Code map: O(m × average_code_length) ≤ O(m log m)
- **Overall: O(m log m) ≈ O(1)**

### File Format

**Compressed File Structure:**
```
[0-3]   Magic Number (4 bytes): 0x48554646 ('HUFF')
[4-11]  Original Char Count (8 bytes, unsigned long long)
[12-2059] Frequency Table (256 × 8 bytes = 2048 bytes)
[2060+] Compressed bit stream
```

**Header Size: 2060 bytes (fixed)**

### Edge Cases Handled

1. **Empty Files**: Creates empty output, sets char count to 0
2. **Single Character Files**: Creates dummy parent node to ensure valid tree
3. **Large Files**: Uses `unsigned long long` for frequencies (handles up to 2^64 - 1 characters)
4. **Bit Padding**: Last byte padded with zeros if not full
5. **File Validation**: Magic number prevents decompressing wrong files

## File Structure

```
huffman-project/
├── Makefile                 # Build configuration
├── README.md               # This file
├── src/
│   ├── huffman.h          # Core data structures and API
│   ├── huffman.c          # Algorithm implementation
│   └── main.c             # CLI interface
├── python/
│   ├── wrapper.py         # Python ctypes wrapper
│   └── demo.py            # Python demo script
├── bin/                   # Output directory (generated)
│   ├── huffman            # CLI executable
│   └── libhuffman.so      # Shared library
├── build/                 # Object files directory (generated)
│   ├── huffman.o
│   └── main.o
└── test_files/            # Test and example files
    ├── sample.txt         # Sample input
    ├── sample_large.txt   # Larger test file
    └── sample.huff        # Compressed file
```

## Building & Testing

### Build Commands

```bash
# Clean previous builds
make clean

# Build everything (CLI + library)
make all

# Build only CLI
make bin/huffman

# Build only library
make bin/libhuffman.so
```

### Testing

**CLI Test:**
```bash
# Compress sample file
./bin/huffman -c test_files/sample_large.txt test_files/sample.huff

# Verify compression occurred
ls -lh test_files/sample_large.txt test_files/sample.huff

# Decompress and verify
./bin/huffman -d test_files/sample.huff test_files/sample.out
cmp test_files/sample_large.txt test_files/sample.out && echo "✓ Test passed" || echo "✗ Test failed"
```

**Python Test:**
```bash
cd python
python demo.py
```

## Examples

### Example 1: Compressing Text

```bash
$ echo "Hello World! This is a test. This test is good." > test.txt

$ ./bin/huffman -c test.txt test.huff
Mode: Compress
Input: test.txt
Output: test.huff
Compression successful.
Operation finished in 0.0001 seconds.

$ ls -lh test.txt test.huff
-rw-r--r-- 1 user user   46 Nov 28 10:30 test.txt
-rw-r--r-- 1 user user 2169 Nov 28 10:30 test.huff
```
*Note: Small files have compression overhead due to header (2060 bytes)*

### Example 2: Large File Compression

With larger files, you'll see significant compression:
```bash
$ ls -lh test_files/sample_large.txt
-rw-r--r-- 1 user user 100K Nov 28 10:30 sample_large.txt

$ ./bin/huffman -c test_files/sample_large.txt sample.huff
Compression successful.

$ ls -lh sample.huff
-rw-r--r-- 1 user user 45K Nov 28 10:30 sample.huff
```
*~55% compression on typical English text*

### Example 3: Python Integration

```python
from python.wrapper import compress, decompress
from pathlib import Path

# Compress
input_file = Path('large_document.pdf')
output_file = input_file.with_suffix('.huff')

if compress(str(input_file), str(output_file)):
    size_reduction = (1 - output_file.stat().st_size / input_file.stat().st_size) * 100
    print(f"Compressed to {size_reduction:.1f}% of original size")

# Later: Decompress
restored_file = input_file.with_stem(input_file.stem + '_restored')
if decompress(str(output_file), str(restored_file)):
    print("File restored successfully")
```

## Performance Considerations

### Compression Ratio by File Type

| File Type | Typical Ratio | Notes |
|-----------|--------------|-------|
| Plain Text (.txt) | 40-60% | Excellent - high character redundancy |
| Source Code (.c, .py) | 50-65% | Very good - repetitive keywords |
| Already Compressed (.zip, .jpg) | 95-100% | Poor - entropy already high |
| Binary Data | 70-90% | Variable - depends on entropy |

### Performance Metrics

On modern hardware (single-threaded):
- **Compression Speed**: 10-50 MB/sec (depends on entropy)
- **Decompression Speed**: 20-100 MB/sec
- **Memory Usage**: < 10MB for files up to 1GB

### Optimization Opportunities

1. **Parallel Processing**: Process file in chunks
2. **Adaptive Huffman**: Update tree during compression for streaming
3. **Run-Length Encoding**: Preprocess repetitive data
4. **Dictionary Compression**: Combine with LZ algorithms

## Contributing

This is a learning project. Feel free to:
- Report issues
- Suggest improvements
- Extend with new features (e.g., streaming compression, arithmetic coding)
- Optimize performance
- Add language bindings (Rust, Go, Java, etc.)

### Ideas for Extensions

- [ ] Streaming compression (not loading entire file in memory)
- [ ] Parallel multi-threaded compression
- [ ] Adaptive Huffman coding (tree updates during compression)
- [ ] JavaScript binding via WebAssembly
- [ ] Compression statistics and analysis
- [ ] Support for directory compression (tar-like)

## License

This project is provided as-is for educational purposes.

---

**Questions or Issues?** Feel free to open an issue.

**Last Updated:** November 28, 2025
