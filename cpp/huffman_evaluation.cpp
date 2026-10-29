// huffman_evaluation.cpp

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
#include <chrono>  // For timing

// Include for memory usage measurement on macOS
#ifdef __APPLE__
#include <mach/mach.h>
#else
#include <sys/resource.h> // For getrusage on Linux
#endif

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
        merged->left = left;    // Use '->' to access members of pointers
        merged->right = right;

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

// Function to delete the Huffman Tree and free memory
void delete_huffman_tree(HuffmanNode* node) {
    if (node == nullptr)
        return;
    delete_huffman_tree(node->left);
    delete_huffman_tree(node->right);
    delete node;
}

// Function to get memory usage
size_t getMemoryUsage() {
    #ifdef __APPLE__
    // Use Mach APIs on macOS
    task_basic_info_data_t info;
    mach_msg_type_number_t infoCount = TASK_BASIC_INFO_COUNT;
    kern_return_t kr = task_info(mach_task_self(), TASK_BASIC_INFO, (task_info_t)&info, &infoCount);
    if (kr != KERN_SUCCESS) {
        std::cerr << "Error: task_info failed with error code " << kr << "\n";
        return 0;
    }
    // Debugging: Print info.resident_size
    // std::cout << "resident_size: " << info.resident_size << " bytes\n";
    return info.resident_size; // Memory usage in bytes
    #else
    // Use getrusage on Linux
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss * 1024L; // ru_maxrss is in kilobytes, convert to bytes
    #endif
}

int main(int argc, char** argv) {
    // Total execution time start
    auto total_start = std::chrono::high_resolution_clock::now();

    // Memory usage before execution
    size_t mem_before = getMemoryUsage();

    // Debugging: Print memory before execution
    // std::cout << "Memory Before Execution: " << mem_before << " bytes\n";

    // Step 1: Read and Preprocess the Text
    auto start = std::chrono::high_resolution_clock::now();
    std::string input_file = argc > 1 ? argv[1] : "examples/sample.txt";
    std::ifstream infile(input_file, std::ios::in | std::ios::binary);
    if (!infile.is_open()) {
        std::cerr << "Error: Unable to open input file '" << input_file << "'.\n";
        return 1;
    }

    std::ostringstream ss;
    ss << infile.rdbuf();
    std::string raw_text = ss.str();
    infile.close();

    std::string cleaned_text = preprocess_text(raw_text);
    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> preprocessing_time = end - start;
    std::cout << "Preprocessing Time: " << preprocessing_time.count() << " seconds\n";

    // Step 2: Build Frequency Dictionary
    start = std::chrono::high_resolution_clock::now();
    std::map<char, int> frequency = build_frequency_dict(cleaned_text);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> frequency_time = end - start;
    std::cout << "Building Frequency Dictionary Time: " << frequency_time.count() << " seconds\n";

    // Step 3: Build Huffman Tree
    start = std::chrono::high_resolution_clock::now();
    HuffmanNode* root = build_huffman_tree(frequency);
    if (!root) {
        std::cerr << "Input contains no printable ASCII characters.\n";
        return 1;
    }
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tree_time = end - start;
    std::cout << "Building Huffman Tree Time: " << tree_time.count() << " seconds\n";

    // Step 4: Generate Huffman Codes
    start = std::chrono::high_resolution_clock::now();
    std::map<char, std::string> huffman_codes;
    generate_huffman_codes(root, "", huffman_codes);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> codes_time = end - start;
    std::cout << "Generating Huffman Codes Time: " << codes_time.count() << " seconds\n";

    // Step 5: Encode the Text
    start = std::chrono::high_resolution_clock::now();
    std::string encoded_binary = encode_text(cleaned_text, huffman_codes);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> encoding_time = end - start;
    std::cout << "Encoding Text Time: " << encoding_time.count() << " seconds\n";

    // Step 6: Decode the Text
    start = std::chrono::high_resolution_clock::now();
    std::string decoded_text = decode_text(encoded_binary, root);
    end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> decoding_time = end - start;
    std::cout << "Decoding Text Time: " << decoding_time.count() << " seconds\n";

    // Memory usage after execution
    size_t mem_after = getMemoryUsage();

    // Debugging: Print memory after execution
    // std::cout << "Memory After Execution: " << mem_after << " bytes\n";

    // macOS returns current resident memory; Linux returns the process high-water mark.
    size_t peak_memory_usage = mem_after;

    std::cout << "Process Memory Usage: " << (peak_memory_usage / (1024.0 * 1024.0)) << " MB\n";

    // Total execution time end
    auto total_end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> total_time = total_end - total_start;
    std::cout << "Total Execution Time: " << total_time.count() << " seconds\n";

    // Verify if decoded text matches the original cleaned text
    if (decoded_text == cleaned_text) {
        std::cout << "Decoding Successful: The decoded text matches the original text.\n";
    } else {
        std::cout << "Decoding Failed: The decoded text does not match the original text.\n";
    }

    // Clean up Huffman Tree to free memory
    delete_huffman_tree(root);

    return 0;
}
