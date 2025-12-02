import ctypes
import os
import sys

# --- Load the Shared Library ---

# Define the path to the shared library
lib_name = 'libhuffman.so'
if sys.platform.startswith('win'):
    # Note: For this to work on native Windows (not WSL),
    # you'd need to compile a .dll, which is a bit different.
    # But since we're in WSL, .so is correct.
    lib_name = 'libhuffman.dll'
elif sys.platform.startswith('darwin'):
    lib_name = 'libhuffman.dylib'

# Get the absolute path to the library in the /bin directory
# This goes up one level from /python, then into /bin
lib_path = os.path.join(os.path.dirname(__file__), '..', 'bin', lib_name)

# Check if the file exists before loading
if not os.path.exists(lib_path):
    print(f"Error: Shared library not found at {lib_path}")
    print("Please compile the C code first by running 'make lib' or 'make all' in the root directory.")
    sys.exit(1)

# Load the C library
try:
    libhuffman = ctypes.CDLL(lib_path)
except OSError as e:
    print(f"Error loading C library: {e}")
    print("This can happen if you are trying to run this on Windows *without* WSL.")
    sys.exit(1)


# --- Define Function Prototypes (ArgTypes and ResType) ---

# This is crucial! It tells ctypes how to handle the function arguments and return values.

# int api_compress_file(const char* inputPath, const char* outputPath);
libhuffman.api_compress_file.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
libhuffman.api_compress_file.restype = ctypes.c_int

# int api_decompress_file(const char* inputPath, const char* outputPath);
libhuffman.api_decompress_file.argtypes = [ctypes.c_char_p, ctypes.c_char_p]
libhuffman.api_decompress_file.restype = ctypes.c_int


# --- Create Friendly Python Wrapper Functions ---

def compress(input_path: str, output_path: str) -> bool:
    """
    Compresses a file using the C Huffman library.
    
    Args:
        input_path (str): Path to the input file.
        output_path (str): Path to the output compressed file.
    
    Returns:
        bool: True on success, False on failure.
    """
    print(f"[Python] Calling C function to compress '{input_path}'...")
    
    # Convert Python strings to C-style byte strings
    c_input = input_path.encode('utf-8')
    c_output = output_path.encode('utf-8')
    
    # Call the C function
    ret = libhuffman.api_compress_file(c_input, c_output)
    
    if ret == 0:
        print("[Python] C compression successful.")
        return True
    else:
        print("[Python] C compression failed.")
        return False

def decompress(input_path: str, output_path: str) -> bool:
    """
    Decompresses a file using the C Huffman library.
    
    Args:
        input_path (str): Path to the compressed file.
        output_path (str): Path to the restored file.
    
    Returns:
        bool: True on success, False on failure.
    """
    print(f"[Python] Calling C function to decompress '{input_path}'...")
    
    # Convert Python strings to C-style byte strings
    c_input = input_path.encode('utf-8')
    c_output = output_path.encode('utf-8')
    
    # Call the C function
    ret = libhuffman.api_decompress_file(c_input, c_output)
    
    if ret == 0:
        print("[Python] C decompression successful.")
        return True
    else:
        print("[Python] C decompression failed.")
        return False

# This allows other Python scripts to import these functions
if __name__ == '__main__':
    print("This is a wrapper module. Run demo.py to see it in action.")