% huffman_encoding.m

clear; % Clear workspace variables

%% Step 1: Reading and Preprocessing the Text

% Define input and output file names
input_file = fullfile(fileparts(mfilename('fullpath')), '..', 'examples', 'sample.txt');
cleaned_output_file = 'cleaned_text.txt';

% Read the input file
try
    fid = fopen(input_file, 'r', 'n', 'UTF-8');
    raw_text = fread(fid, '*char')';
    fclose(fid);
    fprintf('Successfully read the input file: %s\n', input_file);
catch ME
    fprintf('Error reading file %s: %s\n', input_file, ME.message);
    return;
end

% Retain letters, digits, punctuation, and spaces
allowed_chars = char(32:126); % ASCII printable characters
mask = ismember(raw_text, allowed_chars);
cleaned_text = raw_text(mask);

% Replace multiple whitespace characters with a single space
cleaned_text = regexprep(cleaned_text, '\s+', ' ');

% Trim leading and trailing spaces
cleaned_text = strtrim(cleaned_text);
if isempty(cleaned_text)
    error('Input contains no printable ASCII characters.');
end

% Save the cleaned text to a file
try
    fid = fopen(cleaned_output_file, 'w', 'n', 'UTF-8');
    fwrite(fid, cleaned_text);
    fclose(fid);
    fprintf('Cleaned text successfully saved to: %s\n', cleaned_output_file);
catch ME
    fprintf('Error writing to file %s: %s\n', cleaned_output_file, ME.message);
    return;
end

% Display sample of cleaned text
fprintf('Preprocessing Complete.\n');
fprintf('Cleaned Text Length: %d characters\n', length(cleaned_text));
sample_length = min(500, length(cleaned_text));
fprintf('Cleaned Text Sample (first %d characters):\n%s...\n\n', sample_length, cleaned_text(1:sample_length));

%% Step 2: Building the Frequency Dictionary

% Initialize frequency dictionary using containers.Map
frequency_dict = containers.Map('KeyType', 'char', 'ValueType', 'double');

% Count frequencies
for i = 1:length(cleaned_text)
    ch = cleaned_text(i); % Use 'ch' as variable name
    if isKey(frequency_dict, ch)
        frequency_dict(ch) = frequency_dict(ch) + 1;
    else
        frequency_dict(ch) = 1;
    end
end

% Display character frequencies
fprintf('Character Frequencies:\n');
keys_list = keys(frequency_dict);
for i = 1:length(keys_list)
    ch = keys_list{i};
    if ch == ' '
        display_label = 'Space';
    elseif ch == newline
        display_label = 'Newline';
    elseif ch == '\t'
        display_label = 'Tab';
    else
        display_label = sprintf('''%s''', ch);
    end
    fprintf('%-8s : %d\n', display_label, frequency_dict(ch));
end

% Save frequency dictionary to a .mat file
frequency_output_file = 'frequency_dict.mat';
try
    save(frequency_output_file, 'frequency_dict');
    fprintf('Frequency dictionary successfully saved to: %s\n', frequency_output_file);
catch ME
    fprintf('Error saving frequency dictionary to %s: %s\n', frequency_output_file, ME.message);
    return;
end

%% Step 3: Building the Huffman Tree

% Initialize a list of nodes
nodes = [];
for i = 1:length(keys_list)
    ch = keys_list{i};
    freq = frequency_dict(ch);
    node = struct('character', ch, 'freq', freq, 'left', [], 'right', []);
    nodes = [nodes; node]; %#ok<AGROW>
end

% Build the Huffman Tree
huffman_tree_root = build_huffman_tree(nodes);
fprintf('Huffman Tree built successfully.\n');

%% Step 4: Generating Huffman Codes

% Initialize Huffman Codes using containers.Map
huffman_codes = containers.Map('KeyType', 'char', 'ValueType', 'char');

% Generate Huffman Codes by traversing the tree
huffman_codes = generate_huffman_codes(huffman_tree_root, '', huffman_codes);

% Display Huffman Codes
fprintf('\nHuffman Codes:\n');
keys_list = keys(huffman_codes);
for i = 1:length(keys_list)
    ch = keys_list{i};
    code = huffman_codes(ch);
    if ch == ' '
        display_char = 'Space';
    elseif ch == newline
        display_char = 'Newline';
    elseif ch == '\t'
        display_char = 'Tab';
    else
        display_char = sprintf('''%s''', ch);
    end
    fprintf('%-8s : %s\n', display_char, code);
end

% Save Huffman Codes to a .mat file
codes_output_file = 'huffman_codes.mat';
try
    save(codes_output_file, 'huffman_codes');
    fprintf('Huffman codes successfully saved to: %s\n', codes_output_file);
catch ME
    fprintf('Error saving Huffman codes to %s: %s\n', codes_output_file, ME.message);
    return;
end

%% Step 5: Encoding the Text

% Encode the text
encoded_binary = '';
for i = 1:length(cleaned_text)
    ch = cleaned_text(i);
    code = huffman_codes(ch);
    encoded_binary = [encoded_binary, code]; %#ok<AGROW>
end

% Calculate the total number of bits
total_bits = length(encoded_binary);
fprintf('\nEncoded Text Length: %d bits\n', total_bits);

% Save the encoded binary string to a text file
encoded_output_file = 'encoded_text.txt';
try
    fid = fopen(encoded_output_file, 'w', 'n', 'UTF-8');
    fwrite(fid, encoded_binary);
    fclose(fid);
    fprintf('Encoded text successfully saved to: %s\n', encoded_output_file);
catch ME
    fprintf('Error writing to file %s: %s\n', encoded_output_file, ME.message);
    return;
end

%% Step 6: Decoding the Text

% Load the encoded binary string
encoded_text_file = 'encoded_text.txt';
try
    fid = fopen(encoded_text_file, 'r', 'n', 'UTF-8');
    encoded_binary_loaded = fread(fid, '*char')';
    fclose(fid);
    fprintf('\nSuccessfully loaded encoded binary from: %s\n', encoded_text_file);
catch ME
    fprintf('Error reading file %s: %s\n', encoded_text_file, ME.message);
    return;
end

% Load the Huffman Codes
codes_file = 'huffman_codes.mat';
try
    load(codes_file, 'huffman_codes');
    fprintf('Successfully loaded Huffman codes from: %s\n', codes_file);
catch ME
    fprintf('Error loading Huffman codes from %s: %s\n', codes_file, ME.message);
    return;
end

% Create a reverse mapping from codes to characters
reverse_codes = containers.Map('KeyType', 'char', 'ValueType', 'char');
keys_list = keys(huffman_codes);
for i = 1:length(keys_list)
    ch = keys_list{i};
    code = huffman_codes(ch);
    reverse_codes(code) = ch;
end

% Decode the binary string
decoded_text = '';
current_code = '';
for i = 1:length(encoded_binary_loaded)
    current_code = [current_code, encoded_binary_loaded(i)]; %#ok<AGROW>
    if isKey(reverse_codes, current_code)
        decoded_char = reverse_codes(current_code);
        decoded_text = [decoded_text, decoded_char]; %#ok<AGROW>
        current_code = '';
    end
end

% Save the decoded text to a file
decoded_output_file = 'decoded_text.txt';
try
    fid = fopen(decoded_output_file, 'w', 'n', 'UTF-8');
    fwrite(fid, decoded_text);
    fclose(fid);
    fprintf('Decoded text successfully saved to: %s\n', decoded_output_file);
catch ME
    fprintf('Error writing to file %s: %s\n', decoded_output_file, ME.message);
    return;
end

% Verify if decoded text matches the original cleaned text
try
    fid = fopen('cleaned_text.txt', 'r', 'n', 'UTF-8');
    original_text = fread(fid, '*char')';
    fclose(fid);

    if strcmp(decoded_text, original_text)
        fprintf('Decoding Successful: The decoded text matches the original text.\n');
    else
        fprintf('Decoding Failed: The decoded text does not match the original text.\n');
    end
catch ME
    fprintf('Error verifying decoded text: %s\n', ME.message);
end

%% Step 7: Visualizing the Huffman Tree

% Visualize the Huffman Tree
visualize_huffman_tree(huffman_tree_root);
fprintf('Huffman Tree visualization complete. Check the generated figure.\n');

%% Function Definitions (Must Be Placed at the End of the Script)

function root = build_huffman_tree(nodes)
    while length(nodes) > 1
        % Sort nodes by frequency in ascending order
        [~, idx] = sort([nodes.freq]);
        nodes = nodes(idx);

        % Take two nodes with lowest frequency
        node1 = nodes(1);
        node2 = nodes(2);

        % Create a new internal node with these two nodes as children
        merged_node = struct('character', '', 'freq', node1.freq + node2.freq, 'left', node1, 'right', node2);

        % Remove the first two nodes and add the merged node
        nodes = nodes(3:end);
        nodes = [nodes; merged_node]; %#ok<AGROW>
    end
    % The remaining node is the root of the Huffman Tree
    root = nodes(1);
end

function codes = generate_huffman_codes(node, current_code, codes)
    if isempty(node.left) && isempty(node.right)
        % Leaf node
        character_key = node.character; % Updated field name
        if isempty(current_code)
            current_code = '0';
        end
        codes(character_key) = current_code;
        return;
    end

    if ~isempty(node.left)
        codes = generate_huffman_codes(node.left, [current_code '0'], codes);
    end
    if ~isempty(node.right)
        codes = generate_huffman_codes(node.right, [current_code '1'], codes);
    end
end

function visualize_huffman_tree(root)
    figure('Name', 'Huffman Tree Visualization', 'NumberTitle', 'off');
    hold on;
    axis off;
    % Starting coordinates
    x = 0;
    y = 0;
    dx = 10; % Horizontal spacing
    dy = -2; % Vertical spacing
    % Recursive plotting
    plot_node(root, x, y, dx, dy);
    hold off;
end

function [next_x] = plot_node(node, x, y, dx, dy)
    if isempty(node)
        next_x = x;
        return;
    end

    if isempty(node.left) && isempty(node.right)
        % Leaf node
        text(x, y, ['\bf{' node.character '}'], 'HorizontalAlignment', 'center');
        next_x = x;
    else
        % Internal node
        text(x, y, '*', 'HorizontalAlignment', 'center');
        % Plot left child
        [left_x] = plot_node(node.left, x - dx, y + dy, dx / 1.5, dy);
        % Draw line to left child
        line([x, left_x], [y, y + dy], 'Color', 'k');
        % Plot right child
        [right_x] = plot_node(node.right, x + dx, y + dy, dx / 1.5, dy);
        % Draw line to right child
        line([x, right_x], [y, y + dy], 'Color', 'k');
        % Update current x position
        next_x = x;
    end
end
