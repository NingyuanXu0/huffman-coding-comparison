# decode_text.py

import json

def decode_text(encoded_binary, codes):
    """
    Decodes the binary string back to the original text using Huffman codes.

    Args:
        encoded_binary (str): The binary string to decode.
        codes (dict): Dictionary mapping characters to their Huffman codes.

    Returns:
        str: Decoded original text.
    """
    # Create a reverse mapping from code to character
    reverse_codes = {code: char for char, code in codes.items()}

    decoded_text = ""
    current_code = ""

    for bit in encoded_binary:
        current_code += bit
        if current_code in reverse_codes:
            decoded_text += reverse_codes[current_code]
            current_code = ""

    return decoded_text

def save_decoded_text(decoded_text, output_path='decoded_text.txt'):
    """
    Saves the decoded text to a file.

    Args:
        decoded_text (str): The decoded text.
        output_path (str, optional): Path to save the decoded text. Defaults to 'decoded_text.txt'.
    """
    try:
        with open(output_path, 'w', encoding='utf-8') as f:
            f.write(decoded_text)
        print(f"Decoded text successfully saved to '{output_path}'.")
    except Exception as e:
        print(f"An error occurred while writing to '{output_path}': {e}")

if __name__ == "__main__":
    encoded_text_file = 'encoded_text.txt'
    huffman_codes_file = 'huffman_codes.json'
    decoded_output_file = 'decoded_text.txt'

    # Read encoded text
    try:
        with open(encoded_text_file, 'r', encoding='utf-8') as f:
            encoded_binary = f.read()
    except FileNotFoundError:
        print(f"Error: File '{encoded_text_file}' not found. Please run 'encode_text.py' first.")
        exit(1)
    except Exception as e:
        print(f"An error occurred while reading '{encoded_text_file}': {e}")
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

    # Decode the text
    decoded_text = decode_text(encoded_binary, huffman_codes)
    print(f"Decoded Text Length: {len(decoded_text)} characters")

    # Save the decoded text
    save_decoded_text(decoded_text, decoded_output_file)

    # Optional: Verify if decoding matches the original cleaned text
    try:
        with open('cleaned_text.txt', 'r', encoding='utf-8') as f:
            original_text = f.read()
        if decoded_text == original_text:
            print("Decoding Successful: The decoded text matches the original text.")
        else:
            print("Decoding Failed: The decoded text does not match the original text.")
    except Exception as e:
        print(f"An error occurred while verifying the decoded text: {e}")
