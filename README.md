# Huffman Coding in Python, C++ and MATLAB

Build a Huffman tree, generate prefix codes, encode text, and recover the preprocessed message in three languages. The repository preserves a step-by-step Python pipeline and standalone C++ and MATLAB implementations from an information theory project.

## Implementations

| Location | Entry point | Text policy |
| --- | --- | --- |
| `python/` | `run_huffman.py` | Lowercase English letters and spaces; repeated spaces collapse |
| `python/` | `huffman_evaluation.py` | Printable ASCII, codes 32–126; repeated spaces collapse |
| `cpp/` | `huffman_encoding.cpp`, `huffman_evaluation.cpp` | Printable ASCII, codes 32–126; repeated spaces collapse |
| `matlab/` | `huffman_encoding.m`, `huffman_evaluation.m` | Printable ASCII, codes 32–126; repeated spaces collapse |

Preprocessing removes line breaks and unsupported characters. Decoding recovers the cleaned text. Encoded output stores one text character per bit (`0` or `1`); its bit count describes the code length, while the file itself uses text storage. Equal-frequency symbols can receive different valid codes across implementations.

## Python

Use Python 3.9 or later. The Python programs use the standard library.

From the repository root:

```sh
python3 python/run_huffman.py
python3 python/run_huffman.py --input /path/to/message.txt --output-dir outputs/custom
python3 python/huffman_evaluation.py --input examples/sample.txt
```

The pipeline launches each stage with the active interpreter and writes cleaned text, frequencies, codes, encoded text, and decoded text to `outputs/python/` by default. The individual stage scripts use their current working directory for intermediate files.

## C++

Use a compiler with C++11 support. The evaluation program uses Mach memory APIs on macOS and `getrusage` on Linux.

```sh
mkdir -p build outputs/cpp
c++ -std=c++11 -O2 cpp/huffman_encoding.cpp -o build/huffman_encoding
c++ -std=c++11 -O2 cpp/huffman_evaluation.cpp -o build/huffman_evaluation
(cd outputs/cpp && ../../build/huffman_encoding ../../examples/sample.txt)
./build/huffman_evaluation examples/sample.txt
```

Both executables accept an optional input path. Their default, `examples/sample.txt`, is relative to the current directory. The encoding program writes intermediate files to the current directory.

## MATLAB

Use MATLAB R2016b or later for local functions in scripts. From the repository root:

```matlab
run('matlab/huffman_encoding.m')
run('matlab/huffman_evaluation.m')
```

The scripts locate the included sample relative to their source files. Change `input_file` near the start of a script to use another text. The encoding script also displays the tree and writes its intermediate files in the execution directory. The evaluator requires a JVM for its Java heap measurement.

## Measurements

Evaluation programs report per-stage elapsed time. Python reports traced Python allocation peak; C++ reports current resident memory on macOS or the process high-water mark on Linux; MATLAB reports the change in Java heap usage. These measurements have different scopes. Use the printable-ASCII entries with the same input when comparing languages.

## Source and publication

The code comes from the author's information theory coursework. The Python stage pipeline is selected from `Project1`; the evaluation, C++ and MATLAB entries are selected from the consolidated `Information Theory Project 1` snapshot. Publication changes cover portable entry points, empty-input handling, single-symbol coding and memory labels. `examples/sample.txt` is a new text written for this repository.

Existing notices govern the files that carry them; repository-wide reuse terms remain unspecified.

Publication preparation used source review. Local execution, builds and tests were not run.
