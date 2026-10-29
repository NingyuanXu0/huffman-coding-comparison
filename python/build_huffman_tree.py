# build_huffman_tree.py

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

if __name__ == "__main__":
    input_file = 'frequency_dict.json'

    try:
        with open(input_file, 'r', encoding='utf-8') as f:
            frequency_dict = json.load(f)
    except FileNotFoundError:
        print(f"Error: File '{input_file}' not found. Please run 'build_frequency.py' first.")
        exit(1)
    except json.JSONDecodeError:
        print(f"Error: File '{input_file}' is not a valid JSON file.")
        exit(1)
    except Exception as e:
        print(f"An error occurred while reading '{input_file}': {e}")
        exit(1)

    huffman_tree_root = build_huffman_tree(frequency_dict)
    if huffman_tree_root:
        print("Huffman Tree built successfully.")
    else:
        print("Failed to build Huffman Tree.")
