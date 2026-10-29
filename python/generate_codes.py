# generate_codes.py

import json
import heapq
from collections import defaultdict

class HuffmanNode:
    def __init__(self, char=None, freq=0):
        """
        Initializes a Huffman node.

        Args:
            char (str, optional): The character. Defaults to None for internal nodes.
            freq (int): Frequency of the character or combined frequency for internal nodes.
        """
        self.char = char
        self.freq = freq
        self.left = None
        self.right = None

    # Define comparison operators for priority queue
    def __lt__(self, other):
        return self.freq < other.freq

def build_huffman_tree(frequency_dict):
    """
    Builds the Huffman Tree based on the frequency dictionary.

    Args:
        frequency_dict (dict): Dictionary with characters and their frequencies.

    Returns:
        HuffmanNode: Root node of the Huffman Tree.
    """
    heap = []

    # Create a leaf node for each character and add it to the heap
    for char, freq in frequency_dict.items():
        node = HuffmanNode(char, freq)
        heapq.heappush(heap, node)

    # Iterate until the heap has only one node (the root)
    while len(heap) > 1:
        # Remove the two nodes with the lowest frequency
        node1 = heapq.heappop(heap)
        node2 = heapq.heappop(heap)

        # Create a new internal node with these two nodes as children
        merged = HuffmanNode()
        merged.freq = node1.freq + node2.freq
        merged.left = node1
        merged.right = node2

        # Add the new node to the heap
        heapq.heappush(heap, merged)

    # The remaining node is the root of the Huffman Tree
    root = heap[0] if heap else None
    return root

def generate_huffman_codes(node, current_code="", codes=None):
    """
    Recursively traverses the Huffman Tree to generate codes for each character.

    Args:
        node (HuffmanNode): Current node in the Huffman Tree.
        current_code (str, optional): The binary code accumulated so far. Defaults to "".
        codes (dict, optional): Dictionary to store the codes. A new dictionary is created by default.

    Returns:
        dict: Dictionary mapping characters to their Huffman codes.
    """
    if codes is None:
        codes = {}
    if node is None:
        return codes

    # If this is a leaf node, assign the current code
    if node.char is not None:
        codes[node.char] = current_code if current_code else "0"  # Handle single character edge case
        return codes

    # Traverse left
    generate_huffman_codes(node.left, current_code + "0", codes)

    # Traverse right
    generate_huffman_codes(node.right, current_code + "1", codes)

    return codes

def save_huffman_codes(codes, output_path='huffman_codes.json'):
    """
    Saves the Huffman codes to a JSON file.

    Args:
        codes (dict): Dictionary mapping characters to their Huffman codes.
        output_path (str, optional): Path to save the JSON file. Defaults to 'huffman_codes.json'.
    """
    try:
        with open(output_path, 'w', encoding='utf-8') as json_file:
            json.dump(codes, json_file, indent=4)
        print(f"Huffman codes successfully saved to '{output_path}'.")
    except Exception as e:
        print(f"An error occurred while writing to '{output_path}': {e}")

if __name__ == "__main__":
    frequency_file = 'frequency_dict.json'
    output_codes_file = 'huffman_codes.json'

    try:
        with open(frequency_file, 'r', encoding='utf-8') as f:
            frequency_dict = json.load(f)
    except FileNotFoundError:
        print(f"Error: File '{frequency_file}' not found. Please run 'build_frequency.py' first.")
        exit(1)
    except json.JSONDecodeError:
        print(f"Error: File '{frequency_file}' is not a valid JSON file.")
        exit(1)
    except Exception as e:
        print(f"An error occurred while reading '{frequency_file}': {e}")
        exit(1)

    # Build Huffman Tree
    huffman_tree_root = build_huffman_tree(frequency_dict)
    if huffman_tree_root is None:
        print("Failed to build Huffman Tree.")
        exit(1)
    else:
        print("Huffman Tree built successfully.")

    # Generate Huffman Codes
    huffman_codes = {}
    generate_huffman_codes(huffman_tree_root, "", huffman_codes)
    print("\nHuffman Codes:")
    for char, code in sorted(huffman_codes.items()):
        display_char = 'Space' if char == ' ' else f"'{char}'"
        print(f"{display_char}: {code}")

    # Save Huffman Codes
    save_huffman_codes(huffman_codes, output_codes_file)
