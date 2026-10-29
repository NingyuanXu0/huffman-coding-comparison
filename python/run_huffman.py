"""Run the original six-stage Huffman pipeline with one Python interpreter."""

import argparse
import os
from pathlib import Path
import subprocess
import sys


def main():
    source_dir = Path(__file__).resolve().parent
    root = source_dir.parent
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--input', type=Path, default=root / 'examples' / 'sample.txt')
    parser.add_argument('--output-dir', type=Path, default=root / 'outputs' / 'python')
    args = parser.parse_args()
    input_file = args.input.expanduser().resolve()
    output_dir = args.output_dir.expanduser().resolve()
    output_dir.mkdir(parents=True, exist_ok=True)
    environment = os.environ.copy()
    environment['HUFFMAN_INPUT'] = str(input_file)
    scripts = [
        'read_preprocess.py',
        'build_frequency.py',
        'build_huffman_tree.py',
        'generate_codes.py',
        'encode_text.py',
        'decode_text.py',
    ]
    for script in scripts:
        print(f'Running {script}...', flush=True)
        subprocess.run(
            [sys.executable, str(source_dir / script)],
            cwd=output_dir, env=environment, check=True,
        )
    print(f'Pipeline outputs: {output_dir}')


if __name__ == '__main__':
    main()
