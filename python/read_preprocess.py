# read_preprocess.py

import os
from pathlib import Path

def read_and_preprocess(file_path, output_path='cleaned_text.txt'):
    """
    Reads a text file, converts it to lowercase, retains only a-z and space characters,
    normalizes whitespace, and saves the cleaned text to an output file.

    Args:
        file_path (str): Path to the input text file.
        output_path (str, optional): Path to save the cleaned text. Defaults to 'cleaned_text.txt'.

    Returns:
        str: Cleaned text.
    """
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            text = file.read()
    except FileNotFoundError:
        print(f"Error: File '{file_path}' not found.")
        return ""
    except Exception as e:
        print(f"An error occurred while reading the file: {e}")
        return ""

    # Convert to lowercase
    text = text.lower()

    # Retain only a-z and space
    allowed_chars = set('abcdefghijklmnopqrstuvwxyz ')
    cleaned_text = ''.join(char for char in text if char in allowed_chars)

    # Replace multiple spaces with a single space
    cleaned_text = ' '.join(cleaned_text.split())

    # Save the cleaned text to the output file
    try:
        with open(output_path, 'w', encoding='utf-8') as out_file:
            out_file.write(cleaned_text)
        print(f"Cleaned text successfully saved to '{output_path}'.")
    except Exception as e:
        print(f"An error occurred while writing to '{output_path}': {e}")

    return cleaned_text

if __name__ == "__main__":
    input_file = os.environ.get('HUFFMAN_INPUT', str(Path(__file__).resolve().parents[1] / 'examples' / 'sample.txt'))
    output_file = 'cleaned_text.txt'

    cleaned_text = read_and_preprocess(input_file, output_file)
    if not cleaned_text:
        raise SystemExit("Input contains no supported characters after preprocessing.")
    if cleaned_text:
        print("Preprocessing Complete.")
        print(f"Cleaned Text Length: {len(cleaned_text)} characters")
        print(f"Cleaned Text Sample (first 500 characters):\n{cleaned_text[:500]}...\n")
