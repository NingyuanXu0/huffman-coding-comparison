# build_frequency.py

import json
from collections import defaultdict

def build_frequency_dict(text):
    """
    Builds a frequency dictionary for each character in the text.

    Args:
        text (str): Cleaned text.

    Returns:
        dict: Dictionary with characters as keys and their frequencies as values.
    """
    frequency = defaultdict(int)
    for char in text:
        frequency[char] += 1
    return frequency

def save_frequency_dict(frequency_dict, output_path='frequency_dict.json'):
    """
    Saves the frequency dictionary to a JSON file.

    Args:
        frequency_dict (dict): Dictionary with character frequencies.
        output_path (str, optional): Path to save the JSON file. Defaults to 'frequency_dict.json'.
    """
    try:
        with open(output_path, 'w', encoding='utf-8') as json_file:
            json.dump(frequency_dict, json_file, indent=4)
        print(f"Frequency dictionary successfully saved to '{output_path}'.")
    except Exception as e:
        print(f"An error occurred while writing to '{output_path}': {e}")

if __name__ == "__main__":
    input_file = 'cleaned_text.txt'
    output_file = 'frequency_dict.json'

    try:
        with open(input_file, 'r', encoding='utf-8') as f:
            cleaned_text = f.read()
    except FileNotFoundError:
        print(f"Error: File '{input_file}' not found. Please run 'read_preprocess.py' first.")
        exit(1)
    except Exception as e:
        print(f"An error occurred while reading '{input_file}': {e}")
        exit(1)

    frequency_dict = build_frequency_dict(cleaned_text)
    print("Character Frequencies:")
    for char, freq in sorted(frequency_dict.items()):
        display_char = 'Space' if char == ' ' else f"'{char}'"
        print(f"{display_char}: {freq}")

    save_frequency_dict(frequency_dict, output_file)
