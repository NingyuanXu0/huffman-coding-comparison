# huffman_evaluation.py

import argparse
from pathlib import Path
import time
import tracemalloc  # For memory usage
import json
from collections import defaultdict
import heapq
import string
import re

# Function to read and preprocess the text
def read_and_preprocess(file_path):
    with open(file_path, 'r', encoding='utf-8') as file:
        raw_text = file.read()
    # Retain all ASCII printable characters (codes 32 to 126)
    allowed_chars = ''.join(chr(i) for i in range(32, 127))
    pattern = f'[^{re.escape(allowed_chars)}]'
    cleaned_text = re.sub(pattern, '', raw_text)
    # Replace multiple whitespace characters with a single space
    cleaned_text = re.sub(r'\s+', ' ', cleaned_text)
    # Trim leading and trailing spaces
    cleaned_text = cleaned_text.strip()
    return cleaned_text

# Function to build the frequency dictionary
def build_frequency_dict(text):
    frequency = defaultdict(int)
    for char in text:
        frequency[char] += 1
    return dict(frequency)

# Class representing a node in the Huffman Tree
class HuffmanNode:
    def __init__(self, char=None, freq=None):
        self.char = char
        self.freq = freq
        self.left = None
        self.right = None

    # Define comparison operators for priority queue
    def __lt__(self, other):
        return self.freq < other.freq

# Function to build the Huffman Tree
def build_huffman_tree(frequency_dict):
    heap = []
    for char, freq in frequency_dict.items():
        node = HuffmanNode(char, freq)
        heapq.heappush(heap, node)

    # Edge case: only one unique character
    if len(heap) == 1:
        only_node = heapq.heappop(heap)
        root = HuffmanNode(freq=only_node.freq)
        root.left = only_node
        heapq.heappush(heap, root)

    while len(heap) > 1:
        node1 = heapq.heappop(heap)
        node2 = heapq.heappop(heap)
        merged = HuffmanNode(freq=node1.freq + node2.freq)
        merged.left = node1
        merged.right = node2
        heapq.heappush(heap, merged)

    if not heap:
        raise ValueError("Input contains no printable ASCII characters.")
    return heap[0]

# Function to generate Huffman Codes by traversing the tree
def generate_huffman_codes(node, current_code="", codes=None):
    if codes is None:
        codes = {}
    if node is None:
        return codes
    if node.char is not None:
        codes[node.char] = current_code if current_code != '' else '0'
        return
    generate_huffman_codes(node.left, current_code + "0", codes)
    generate_huffman_codes(node.right, current_code + "1", codes)

# Function to encode the text using Huffman Codes
def encode_text(text, codes):
    encoded = ''.join(codes[char] for char in text)
    return encoded

# Function to decode the binary string back to text using Huffman Codes
def decode_text(encoded_binary, codes):
    reverse_codes = {code: char for char, code in codes.items()}
    decoded = ''
    current_code = ''
    for bit in encoded_binary:
        current_code += bit
        if current_code in reverse_codes:
            decoded += reverse_codes[current_code]
            current_code = ''
    return decoded

def main():
    parser = argparse.ArgumentParser(description='Measure the printable-ASCII Huffman pipeline.')
    parser.add_argument('--input', type=Path, default=Path(__file__).resolve().parents[1] / 'examples' / 'sample.txt')
    input_file = parser.parse_args().input
    # Performance measurement starts
    total_start_time = time.time()
    tracemalloc.start()

    # Step 1: Read and Preprocess the Text
    start_time = time.time()
    cleaned_text = read_and_preprocess(input_file)
    end_time = time.time()
    print(f"Preprocessing Time: {end_time - start_time:.6f} seconds")

    # Step 2: Build Frequency Dictionary
    start_time = time.time()
    frequency_dict = build_frequency_dict(cleaned_text)
    end_time = time.time()
    print(f"Building Frequency Dictionary Time: {end_time - start_time:.6f} seconds")

    # Step 3: Build Huffman Tree
    start_time = time.time()
    huffman_tree_root = build_huffman_tree(frequency_dict)
    end_time = time.time()
    print(f"Building Huffman Tree Time: {end_time - start_time:.6f} seconds")

    # Step 4: Generate Huffman Codes
    start_time = time.time()
    huffman_codes = {}
    generate_huffman_codes(huffman_tree_root, "", huffman_codes)
    end_time = time.time()
    print(f"Generating Huffman Codes Time: {end_time - start_time:.6f} seconds")

    # Step 5: Encode the Text
    start_time = time.time()
    encoded_binary = encode_text(cleaned_text, huffman_codes)
    end_time = time.time()
    print(f"Encoding Text Time: {end_time - start_time:.6f} seconds")

    # Step 6: Decode the Text
    start_time = time.time()
    decoded_text = decode_text(encoded_binary, huffman_codes)
    end_time = time.time()
    print(f"Decoding Text Time: {end_time - start_time:.6f} seconds")

    total_end_time = time.time()
    total_time = total_end_time - total_start_time
    print(f"Total Execution Time: {total_time:.6f} seconds")

    # Memory usage
    current, peak = tracemalloc.get_traced_memory()
    tracemalloc.stop()
    print(f"Peak Memory Usage: {peak / 10**6:.6f} MB")

    # Verify Decoded Text
    if decoded_text == cleaned_text:
        print("Decoding Successful: The decoded text matches the original text.")
    else:
        print("Decoding Failed: The decoded text does not match the original text.")

if __name__ == "__main__":
    main()
