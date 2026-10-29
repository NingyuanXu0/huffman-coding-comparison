#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <queue>
#include <vector>
#include <cctype>
#include <algorithm>
#include <sstream> // Required for std::ostringstream
#include <set>

// Struct representing a node in the Huffman Tree
struct HuffmanNode {
    char character;               // Character
    int frequency;                // Frequency of the character
    HuffmanNode* left;            // Left child
    HuffmanNode* right;           // Right child

    HuffmanNode(char ch, int freq) : character(ch), frequency(freq), left(nullptr), right(nullptr) {}
    HuffmanNode(int freq) : character('\0'), frequency(freq), left(nullptr), right(nullptr) {}
};

// Comparator for the priority queue (min-heap based on frequency)
struct Compare {
    bool operator()(HuffmanNode* const& a, HuffmanNode* const& b) const {
        return a->frequency > b->frequency;
    }
};

// Function to preprocess the text: retain letters, digits, punctuation, and spaces
std::string preprocess_text(const std::string& input) {
    std::string cleaned;
    cleaned.reserve(input.size());

    // Define allowed characters: ASCII printable characters (codes 32 to 126)
    std::set<char> allowed_chars;
    for (char ch = 32; ch <= 126; ++ch) {
        allowed_chars.insert(ch);
    }

    // Retain allowed characters
    for (char ch : input) {
        if (allowed_chars.count(ch)) {
            cleaned += ch;
        }
        // Ignore all other characters
    }

    // Replace multiple whitespace characters with a single space
    std::string normalized;
    normalized.reserve(cleaned.size());
    bool previous_space = false;
    for (char ch : cleaned) {
        if (std::isspace(static_cast<unsigned char>(ch))) {
            if (!previous_space) {
                normalized += ' ';
                previous_space = true;
            }
        } else {
            normalized += ch;
            previous_space = false;
        }
    }

    // Trim leading and trailing spaces
    size_t start = normalized.find_first_not_of(' ');
    size_t end = normalized.find_last_not_of(' ');
    if (start != std::string::npos && end != std::string::npos)
        normalized = normalized.substr(start, end - start + 1);
    else
        normalized = ""; // String contains only spaces

    return normalized;
}

// Function to build the frequency dictionary
std::map<char, int> build_frequency_dict(const std::string& text) {
    std::map<char, int> frequency;
    for (char ch : text) {
        frequency[ch]++;
    }
    return frequency;
}

// Function to build the Huffman Tree
HuffmanNode* build_huffman_tree(const std::map<char, int>& frequency) {
    std::priority_queue<HuffmanNode*, std::vector<HuffmanNode*>, Compare> min_heap;

    // Create leaf nodes for each character and add to the heap
    for (const auto& pair : frequency) {
        min_heap.push(new HuffmanNode(pair.first, pair.second));
    }

    // Edge case: if there's only one unique character
    if (min_heap.size() == 1) {
        HuffmanNode* only_node = min_heap.top();
        min_heap.pop();
        HuffmanNode* root = new HuffmanNode(only_node->frequency);
        root->left = only_node;
        min_heap.push(root);
    }

    // Iterate until the heap has only one node (the root)
    while (min_heap.size() > 1) {
        HuffmanNode* left = min_heap.top(); min_heap.pop();
        HuffmanNode* right = min_heap.top(); min_heap.pop();

        HuffmanNode* merged = new HuffmanNode(left->frequency + right->frequency);
        merged->left = left;    // Corrected: Use '->'
        merged->right = right;  // Corrected: Use '->'

        min_heap.push(merged);
    }

    return min_heap.empty() ? nullptr : min_heap.top(); // Empty input has no tree
}

// Function to generate Huffman Codes by traversing the tree
void generate_huffman_codes(HuffmanNode* root, const std::string& current_code, std::map<char, std::string>& huffman_codes) {
    if (!root)
        return;

    // If this is a leaf node, assign the code
    if (!root->left && !root->right) {
        huffman_codes[root->character] = current_code.empty() ? "0" : current_code; // Handle single character case
        return;
    }

    // Traverse left
    generate_huffman_codes(root->left, current_code + "0", huffman_codes);

    // Traverse right
    generate_huffman_codes(root->right, current_code + "1", huffman_codes);
}

// Function to encode the text using Huffman Codes
std::string encode_text(const std::string& text, const std::map<char, std::string>& huffman_codes) {
    std::string encoded;
    encoded.reserve(text.size() * 2); // Estimate initial size

    for (char ch : text) {
        encoded += huffman_codes.at(ch);
    }

    return encoded;
}

// Function to decode the binary string back to text using the Huffman Tree
std::string decode_text(const std::string& encoded_binary, HuffmanNode* root) {
    std::string decoded;
    decoded.reserve(encoded_binary.size()); // Estimate initial size

    HuffmanNode* current = root;
    for (char bit : encoded_binary) {
        if (bit == '0')
            current = current->left;
        else if (bit == '1')
            current = current->right;
        else
            continue; // Ignore invalid bits

        // If leaf node, append the character and reset to root
        if (!current->left && !current->right) {
            decoded += current->character;
            current = root;
        }
    }

    return decoded;
}

// Function to verify if two texts are identical
bool verify_decoding(const std::string& original, const std::string& decoded) {
    return original == decoded;
}

// Function to save text to a file
bool save_to_file(const std::string& filename, const std::string& data) {
    std::ofstream outfile(filename, std::ios::out | std::ios::binary);
    if (!outfile.is_open()) {
        std::cerr << "Error: Unable to open file '" << filename << "' for writing.\n";
        return false;
    }
    outfile << data;
    outfile.close();
    return true;
}

// Function to load text from a file
bool load_from_file(const std::string& filename, std::string& data) {
    std::ifstream infile(filename, std::ios::in | std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "Error: Unable to open file '" << filename << "' for reading.\n";
        return false;
    }
    std::ostringstream ss;
    ss << infile.rdbuf();
    data = ss.str();
    infile.close();
    return true;
}

// Function to save frequency dictionary to a text file
bool save_frequency_dict(const std::map<char, int>& frequency, const std::string& filename) {
    std::ofstream outfile(filename, std::ios::out);
    if (!outfile.is_open()) {
        std::cerr << "Error: Unable to open file '" << filename << "' for writing frequency dictionary.\n";
        return false;
    }

    for (const auto& pair : frequency) {
        if (pair.first == ' ')
            outfile << "Space " << pair.second << "\n";
        else if (pair.first == '\n')
            outfile << "Newline " << pair.second << "\n";
        else if (pair.first == '\t')
            outfile << "Tab " << pair.second << "\n";
        else
            outfile << pair.first << " " << pair.second << "\n";
    }

    outfile.close();
    return true;
}

// Function to save Huffman Codes to a text file
bool save_huffman_codes(const std::map<char, std::string>& huffman_codes, const std::string& filename) {
    std::ofstream outfile(filename, std::ios::out);
    if (!outfile.is_open()) {
        std::cerr << "Error: Unable to open file '" << filename << "' for writing Huffman codes.\n";
        return false;
    }

    for (const auto& pair : huffman_codes) {
        if (pair.first == ' ')
            outfile << "Space " << pair.second << "\n";
        else if (pair.first == '\n')
            outfile << "Newline " << pair.second << "\n";
        else if (pair.first == '\t')
            outfile << "Tab " << pair.second << "\n";
        else
            outfile << pair.first << " " << pair.second << "\n";
    }

    outfile.close();
    return true;
}

// Function to display a sample of the text
void display_sample(const std::string& text, size_t sample_size = 500) {
    std::cout << "Cleaned Text Sample (first " << sample_size << " characters):\n";
    if (text.size() < sample_size)
        std::cout << text << "\n\n";
    else
        std::cout << text.substr(0, sample_size) << "...\n\n";
}

// Function to delete the Huffman Tree and free memory
void delete_huffman_tree(HuffmanNode* node) {
    if (node == nullptr)
        return;
    delete_huffman_tree(node->left);
    delete_huffman_tree(node->right);
    delete node;
}

int main(int argc, char** argv) {
    // Step 1: Read and Preprocess the Text
    std::string input_file = argc > 1 ? argv[1] : "examples/sample.txt";
    std::string cleaned_output_file = "cleaned_text.txt";

    std::ifstream infile(input_file, std::ios::in | std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "Error: Unable to open input file '" << input_file << "'.\n";
        return 1;
    }

    std::ostringstream ss;
    ss << infile.rdbuf();
    std::string raw_text = ss.str();
    infile.close();
    std::cout << "Successfully read the input file: " << input_file << "\n";

    std::string cleaned_text = preprocess_text(raw_text);
    if (!save_to_file(cleaned_output_file, cleaned_text)) {
        return 1;
    }
    std::cout << "Cleaned text successfully saved to: " << cleaned_output_file << "\n";
    std::cout << "Preprocessing Complete.\n";
    std::cout << "Cleaned Text Length: " << cleaned_text.size() << " characters\n";
    display_sample(cleaned_text, 500);

    // Step 2: Build Frequency Dictionary
    std::map<char, int> frequency = build_frequency_dict(cleaned_text);
    std::string frequency_output_file = "frequency_dict.txt";
    if (!save_frequency_dict(frequency, frequency_output_file)) {
        return 1;
    }
    std::cout << "Character Frequencies:\n";
    for (const auto& pair : frequency) {
        std::string display_char;
        if (pair.first == ' ')
            display_char = "Space";
        else if (pair.first == '\n')
            display_char = "Newline";
        else if (pair.first == '\t')
            display_char = "Tab";
        else
            display_char = std::string(1, pair.first);
        std::cout << "'" << display_char << "'    : " << pair.second << "\n";
    }
    std::cout << "Frequency dictionary successfully saved to: " << frequency_output_file << "\n";

    // Step 3: Build Huffman Tree
    HuffmanNode* root = build_huffman_tree(frequency);
    if (!root) {
        std::cerr << "Error: Failed to build Huffman Tree.\n";
        return 1;
    }
    std::cout << "Huffman Tree built successfully.\n";

    // Step 4: Generate Huffman Codes
    std::map<char, std::string> huffman_codes;
    generate_huffman_codes(root, "", huffman_codes);
    std::string huffman_codes_file = "huffman_codes.txt";
    if (!save_huffman_codes(huffman_codes, huffman_codes_file)) {
        return 1;
    }
    std::cout << "\nHuffman Codes:\n";
    for (const auto& pair : huffman_codes) {
        std::string display_char;
        if (pair.first == ' ')
            display_char = "Space";
        else if (pair.first == '\n')
            display_char = "Newline";
        else if (pair.first == '\t')
            display_char = "Tab";
        else
            display_char = std::string(1, pair.first);
        std::cout << "'" << display_char << "'    : " << pair.second << "\n";
    }
    std::cout << "Huffman codes successfully saved to: " << huffman_codes_file << "\n";

    // Step 5: Encode the Text
    std::string encoded_binary = encode_text(cleaned_text, huffman_codes);
    std::cout << "\nEncoded Text Length: " << encoded_binary.size() << " bits\n";
    std::string encoded_output_file = "encoded_text.txt";
    if (!save_to_file(encoded_output_file, encoded_binary)) {
        return 1;
    }
    std::cout << "Encoded text successfully saved to: " << encoded_output_file << "\n";

    // Step 6: Decode the Text
    std::string loaded_encoded_binary;
    if (!load_from_file(encoded_output_file, loaded_encoded_binary)) {
        return 1;
    }
    std::cout << "\nSuccessfully loaded encoded binary from: " << encoded_output_file << "\n";

    std::string loaded_decoded_text;
    loaded_decoded_text = decode_text(loaded_encoded_binary, root);

    std::string decoded_output_file = "decoded_text.txt";
    if (!save_to_file(decoded_output_file, loaded_decoded_text)) {
        return 1;
    }
    std::cout << "Decoded text successfully saved to: " << decoded_output_file << "\n";

    // Verification
    if (verify_decoding(cleaned_text, loaded_decoded_text)) {
        std::cout << "Decoding Successful: The decoded text matches the original text.\n";
    } else {
        std::cout << "Decoding Failed: The decoded text does not match the original text.\n";
    }

    // Clean up Huffman Tree to free memory
    delete_huffman_tree(root);

    std::cout << "\nAll steps executed successfully.\n";

    return 0;
}
