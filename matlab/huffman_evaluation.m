% huffman_evaluation.m

% Clear workspace and command window
clear; clc;

% Total execution time start
total_tic = tic;

% Start memory profiling using Java runtime
runtime = java.lang.Runtime.getRuntime();
mem_before = runtime.totalMemory() - runtime.freeMemory();

%% Step 1: Reading and Preprocessing the Text
tic;
input_file = fullfile(fileparts(mfilename('fullpath')), '..', 'examples', 'sample.txt');
fid = fopen(input_file, 'r', 'n', 'UTF-8');
if fid == -1
    error('Error opening file: %s\n', input_file);
end
raw_text = fread(fid, '*char')';
fclose(fid);

% Retain ASCII printable characters (codes 32 to 126)
allowed_chars = char(32:126);
cleaned_text = regexprep(raw_text, ['[^', regexptranslate('escape', allowed_chars), ']'], '');

% Replace multiple whitespace characters with a single space
cleaned_text = regexprep(cleaned_text, '\s+', ' ');

% Trim leading and trailing spaces
cleaned_text = strtrim(cleaned_text);
if isempty(cleaned_text)
    error('Input contains no printable ASCII characters.');
end

preprocessing_time = toc;
fprintf('Preprocessing Time: %.6f seconds\n', preprocessing_time);

%% Step 2: Building the Frequency Dictionary
tic;
frequency_dict = containers.Map('KeyType', 'char', 'ValueType', 'double');
for i = 1:length(cleaned_text)
    char_key = cleaned_text(i);
    if isKey(frequency_dict, char_key)
        frequency_dict(char_key) = frequency_dict(char_key) + 1;
    else
        frequency_dict(char_key) = 1;
    end
end
frequency_time = toc;
fprintf('Building Frequency Dictionary Time: %.6f seconds\n', frequency_time);

%% Step 3: Building the Huffman Tree
tic;
% Initialize a list of nodes
keys = frequency_dict.keys;
nodes = [];
for i = 1:length(keys)
    char = keys{i};
    freq = frequency_dict(char);
    node = struct('char', char, 'freq', freq, 'left', [], 'right', []);
    nodes = [nodes; node]; %#ok<AGROW>
end

% Build the Huffman Tree
huffman_tree_root = build_huffman_tree(nodes);
tree_time = toc;
fprintf('Building Huffman Tree Time: %.6f seconds\n', tree_time);

%% Step 4: Generating Huffman Codes
tic;
% Initialize Huffman Codes using containers.Map
huffman_codes = containers.Map('KeyType', 'char', 'ValueType', 'char');

% Generate Huffman Codes by traversing the tree
huffman_codes = generate_huffman_codes(huffman_tree_root, '', huffman_codes);
codes_time = toc;
fprintf('Generating Huffman Codes Time: %.6f seconds\n', codes_time);

%% Step 5: Encoding the Text
tic;
% Encode the text
encoded_binary = '';
for i = 1:length(cleaned_text)
    char = cleaned_text(i);
    code = huffman_codes(char);
    encoded_binary = [encoded_binary, code]; %#ok<AGROW>
end
encoding_time = toc;
fprintf('Encoding Text Time: %.6f seconds\n', encoding_time);

%% Step 6: Decoding the Text
tic;
% Create a reverse mapping from codes to characters
reverse_codes = containers.Map('KeyType', 'char', 'ValueType', 'char');
keys = huffman_codes.keys;
for i = 1:length(keys)
    char = keys{i};
    code = huffman_codes(char);
    reverse_codes(code) = char;
end

% Decode the binary string
decoded_text = '';
current_code = '';
for i = 1:length(encoded_binary)
    current_code = [current_code, encoded_binary(i)]; %#ok<AGROW>
    if isKey(reverse_codes, current_code)
        decoded_char = reverse_codes(current_code);
        decoded_text = [decoded_text, decoded_char]; %#ok<AGROW>
        current_code = '';
    end
end
decoding_time = toc;
fprintf('Decoding Text Time: %.6f seconds\n', decoding_time);

% Total execution time end
total_time = toc(total_tic);
fprintf('Total Execution Time: %.6f seconds\n', total_time);

% Measure memory usage after execution using Java runtime
mem_after = runtime.totalMemory() - runtime.freeMemory();
java_heap_change = (mem_after - mem_before) / 1e6; % Java heap delta, not MATLAB peak memory
fprintf('Java Heap Change: %.6f MB\n', java_heap_change);

% Verify if decoded text matches the original cleaned text
if strcmp(decoded_text, cleaned_text)
    fprintf('Decoding Successful: The decoded text matches the original text.\n');
else
    fprintf('Decoding Failed: The decoded text does not match the original text.\n');
end

%% Function Definitions

function root = build_huffman_tree(nodes)
    while length(nodes) > 1
        % Sort nodes by frequency in ascending order
        [~, idx] = sort([nodes.freq]);
        nodes = nodes(idx);

        % Take two nodes with lowest frequency
        node1 = nodes(1);
        node2 = nodes(2);

        % Create a new internal node with these two nodes as children
        merged_node = struct('char', '', 'freq', node1.freq + node2.freq, 'left', node1, 'right', node2);

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
        char_key = node.char;
        if isempty(current_code)
            codes(char_key) = '0'; % Handle single character case
        else
            codes(char_key) = current_code;
        end
        return;
    end

    if ~isempty(node.left)
        codes = generate_huffman_codes(node.left, [current_code '0'], codes);
    end
    if ~isempty(node.right)
        codes = generate_huffman_codes(node.right, [current_code '1'], codes);
    end
end
