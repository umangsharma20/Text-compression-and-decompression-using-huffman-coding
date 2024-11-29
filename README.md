# Text File Compression and Decompression using Huffman Coding

## Overview

This program implements **Huffman coding**, a popular method for lossless data compression. The program includes functions to compress a file using Huffman encoding and then decompress the file back to its original state. Huffman coding is efficient for encoding data with varying character frequencies, achieving better compression ratios than fixed-length encoding schemes.

## Requirements

- C++ with MinGW as the compiler

## Features

### Compression:
- Reads a file and constructs a frequency table for its characters.
- Builds a **Huffman tree** based on character frequencies.
- Generates Huffman codes for each character.
- Compresses the file and writes the compressed data to an output file along with a header containing the Huffman codes.

### Decompression:
- Reads the compressed file and extracts the Huffman codes from the header.
- Reconstructs the original file from the compressed bitstream.

## Components

### Data Structures:
- **Tree**: Represents a node in the Huffman tree.
  - `int frequency`: The frequency of the character.
  - `unsigned char character`: The character the node represents.
  - `Tree *left, *right`: Pointers to the left and right child nodes.
  
- **TreeComparator**: A comparator class used to order nodes in the priority queue based on their frequencies.

### Functions:
- **buildHuffmanTree**: Constructs the Huffman tree from a frequency table using a priority queue to combine the least frequent nodes.
- **toBinary**: Converts a character to its binary string representation.
- **traverseHuffmanTree**: Recursively traverses the Huffman tree to generate the Huffman codes for each character.
- **readFileIntoBuffer**: Reads a file into a buffer and returns a pointer to the buffer and its size.
- **writeFileFromBuffer**: Writes data from a buffer to a file.
- **convertToVector**: Converts a frequency map to a vector of pairs.
- **getHuffmanBitstring**: Generates a bitstring for the entire file using Huffman codes. Pads the bitstring to make its length a multiple of 8.
- **getBufferFromString**: Converts a bitstring to a buffer of bytes.
- **getStringFromBuffer**: Converts a buffer of bytes to a bitstring.
- **getDecodedBuffer**: Decodes a bitstring using the Huffman codes to reconstruct the original data.
- **writeHeader**: Writes the Huffman codes and padding information to the output file.
- **readHeader**: Reads the Huffman codes and padding information from the input file.
- **compressFile**: Handles the entire process of reading a file, generating Huffman codes, compressing the file, and writing the compressed data to an output file.
- **decompressFile**: Handles the entire process of reading a compressed file, extracting Huffman codes, and reconstructing the original file.

### Main Function:
The main function provides an interface for compressing and decompressing files via command-line arguments.

## Usage

### Compilation
To compile the program, use a C++ compiler such as g++:

```bash
g++ -o huffman huffman.cpp
