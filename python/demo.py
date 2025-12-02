import wrapper # Our wrapper.py file
import os
import filecmp

def run_demo():
    print("--- Running Python CTypes Wrapper Demo ---")
    
    # Define file paths
    # We use os.path.join to be cross-platform
    base_dir = os.path.dirname(__file__)
    test_files_dir = os.path.join(base_dir, '..', 'test_files')
    
    input_file = os.path.join(test_files_dir, 'sample.txt')
    compressed_file = os.path.join(test_files_dir, 'sample_py.huff')
    restored_file = os.path.join(test_files_dir, 'restored_py.txt')
    
    # Check if sample.txt exists
    if not os.path.exists(input_file):
        print(f"Error: 'test_files/sample.txt' not found.")
        print("Please create it before running the demo.")
        return

    # --- 1. Compress ---
    success_compress = wrapper.compress(input_file, compressed_file)
    
    if not success_compress:
        print("Demo failed during compression.")
        return

    # --- 2. Decompress ---
    success_decompress = wrapper.decompress(compressed_file, restored_file)
    
    if not success_decompress:
        print("Demo failed during decompression.")
        return

    # --- 3. Verify ---
    print(f"[Python] Verifying integrity of '{restored_file}'...")
    try:
        if filecmp.cmp(input_file, restored_file, shallow=False):
            print("SUCCESS: Restored file is identical to the original.")
        else:
            print("FAILURE: Restored file is different from the original.")
    except FileNotFoundError:
        print("FAILURE: Output file was not created.")

    # --- 4. Cleanup ---
    print("[Python] Cleaning up demo files...")
    if os.path.exists(compressed_file):
        os.remove(compressed_file)
    if os.path.exists(restored_file):
        os.remove(restored_file)

if __name__ == "__main__":
    # Ensure the C library is built first
    # We are in the /python directory, so we look in ../bin/
    lib_path = os.path.join(os.path.dirname(__file__), '..', 'bin', 'libhuffman.so')
    if not os.path.exists(lib_path):
        print("Error: 'bin/libhuffman.so' not found.")
        print("Please run 'make lib' or 'make all' in the root directory first.")
    else:
        run_demo()