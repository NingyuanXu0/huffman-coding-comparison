# encode_text.py

import json

def encode_text(text, codes):
    """
    Encodes the text using the provided Huffman codes.

    Args:
        text (str): Cleaned text to encode.
        codes (dict): Dictionary mapping characters to their Huffman codes.

    Returns:
        str: Encoded binary string.
    """
    encoded = ''.join(codes[char] for char in text)
    return encoded

def save_encoded_text(encoded_text, output_path='encoded_text.txt'):
    """
    Saves the encoded binary string to a text file.

    Args:
        encoded_text (str): The binary string to save.
        output_path (str, optional): Path to save the encoded text. Defaults to 'encoded_text.txt'.
    """
    try:
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(encoded_text)
        print(f"Encoded text successfully saved to '{output_path}'.")
    except Exception as e:
        print(f"An error occurred while writing to '{output_path}': {e}")

if __name__ == "__main__":
    cleaned_text_file = 'cleaned_text.txt'
    huffman_codes_file = 'huffman_codes.json'
    encoded_output_file = 'encoded_text.txt'

    # Read cleaned text
    try:
        with open(cleaned_text_file, 'r', encoding='utf-8') as f:
            cleaned_text = f.read()
    except FileNotFoundError:
        print(f"Error: File '{cleaned_text_file}' not found. Please run 'read_preprocess.py' first.")
        exit(1)
    except Exception as e:
        print(f"An error occurred while reading '{cleaned_text_file}': {e}")
        exit(1)

    # Read Huffman codes
    try:
        with open(huffman_codes_file, 'r', encoding='utf-8') as f:
            huffman_codes = json.load(f)
    except FileNotFoundError:
        print(f"Error: File '{huffman_codes_file}' not found. Please run 'generate_codes.py' first.")
        exit(1)
    except json.JSONDecodeError:
        print(f"Error: File '{huffman_codes_file}' is not a valid JSON file.")
        exit(1)
    except Exception as e:
        print(f"An error occurred while reading '{huffman_codes_file}': {e}")
        exit(1)

    # Encode the text
    encoded_binary = encode_text(cleaned_text, huffman_codes)
    print(f"Encoded Text Length: {len(encoded_binary)} bits")

    # Save the encoded text
    save_encoded_text(encoded_binary, encoded_output_file)
