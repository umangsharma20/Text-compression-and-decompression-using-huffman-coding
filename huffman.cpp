#include <iostream>
#include <vector>
#include <queue>
#include <map>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <algorithm>

/*This implementation involves reading a file, compressing its contents using Huffman coding,
 writing the compressed data to a file, and then decompressing it back to its original form.*/

using namespace std;
//Node in the Huffman tree,containing the frequency of a character, the character itself, and pointers to left and right children.
struct Tree {
    int frequency;
    unsigned char character;
    Tree *left = nullptr;
    Tree *right = nullptr;
};
// Custom comparator for the priority queue to prioritize nodes with lower frequency.
class TreeComparator {
public:
    bool operator()(Tree *a, Tree *b) {
        return a->frequency > b->frequency;
    }
};
/*The function buildHuffmanTree takes a frequency table and constructs the Huffman tree using a priority queue.
    Nodes with lower frequencies have higher priority.*/
Tree *buildHuffmanTree(vector<pair<unsigned char, int>> freqtable) {
    priority_queue<Tree *, vector<Tree *>, TreeComparator> huffqueue;
    for (int i = 0; i < freqtable.size(); i++) {
        Tree *node = new Tree();
        node->frequency = freqtable[i].second;
        node->character = freqtable[i].first;
        huffqueue.push(node);
    }
    while (huffqueue.size() > 1) {
        Tree *a, *b;
        a = huffqueue.top();
        huffqueue.pop();
        b = huffqueue.top();
        huffqueue.pop();
        Tree *c = new Tree();
        c->frequency = a->frequency + b->frequency;
        c->left = a;
        c->right = b;
        huffqueue.push(c);
    }
    return huffqueue.top();
}
/*The toBinary function converts an unsigned char (a byte) to its (8-bit) binary string representation*/
string toBinary(unsigned char a) {
    string output = "";
    while (a != 0) {
        string bit = a % 2 == 0 ? "0" : "1";
        output += bit;
        a /= 2;
    }
    if (output.size() < 8) {
        int deficit = 8 - output.size();
        for (int i = 0; i < deficit; i++) {
            output += "0";
        }
    }
    reverse(output.begin(), output.end());
    return output;
}

/*The traverseHuffmanTree function recursively traverses the Huffman tree, appending "0" or "1" to the binary code at each step,
 and stores the resulting binary codes for each character in the provided map when a leaf node is reached.*/

/*  root: A pointer to the current node in the Huffman tree.
    prev: A string representing the binary code generated so far during the traversal.
    toAppend: A string representing the current bit ("0" or "1") to append to prev
    codemap: A map that stores the binary codes for each character.
    */
void traverseHuffmanTree(Tree *root, string prev, string toAppend, map<unsigned char, string> &codemap) {
    prev += toAppend;
    if (root->right == nullptr && root->left == nullptr) {
        codemap[root->character] = prev; // if leaf node,store with the character as key
    }
    if (root->right != nullptr) {
        traverseHuffmanTree(root->right, prev, "1", codemap);
    }
    if (root->left != nullptr) {
        traverseHuffmanTree(root->left, prev, "0", codemap);
    }
}
/*readFileIntoBuffer function reads the contents of a file into a dynamically allocated buffer and returns a pointer to this buffer.*/
//also sets the size of the file in bytes through the reference parameter sz.
unsigned char *readFileIntoBuffer(const char *path, int &sz) {
    FILE *fp = fopen(path, "rb");
    if (fp == nullptr) {
        cerr << "Error opening file" << endl;
        exit(1);
    }
    /*file size by using the fseek and ftell functions
    (fp, 0, SEEK_END) moves the pointer to the end.
    ftell(fp) gives the position at the end, which is the size of the file.
    fseek(fp, 0, SEEK_SET) resets the pointer for subsequent operations.*/

    fseek(fp, 0, SEEK_END);  
    sz = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    unsigned char *buffer = (unsigned char *) malloc(sz);
    fread(buffer, 1, sz, fp);
    fclose(fp);
    return buffer;
}

/*writeFileFromBuffer writes data from a buffer to a file

If flag is 0, the file is opened in "wb" mode (write binary).
"wb" mode creates a new file for writing. If the file already exists, it will be truncated to zero length.

If flag is not 0, the file is opened in "ab" mode (append binary).
"ab" mode opens the file for writing, appending new data to the end of the file. If the file does not exist, it will be created.
*/
void writeFileFromBuffer(const char *path, unsigned char *buffer, int sz, int flag) {
    FILE *fp;
    if (flag == 0) {
        fp = fopen(path, "wb");
    } else {
        fp = fopen(path, "ab");
    }
    // Check if the file was successfully opened
    if (fp == nullptr) {
        cerr << "Error opening file" << endl;
        exit(1);
    }
     // Write the buffer content to the file
    fwrite(buffer, 1, sz, fp);
    fclose(fp);
}

/*convertToVector, takes a map of character frequencies and converts it into a vector of pairs.
*/
vector<pair<unsigned char, int>> convertToVector(map<unsigned char, int> codes) {
    // Declare a vector to hold pairs of characters and their frequencies
    vector<pair<unsigned char, int>> codesV;
    for (map<unsigned char, int>::iterator i = codes.begin(); i != codes.end(); i++) {
         //while iterating through the map, Push each pair (character, frequency) into the vector
        codesV.push_back(make_pair(i->first, i->second));
    }
    return codesV;
}

string getHuffmanBitstring(unsigned char *buffer, map<unsigned char, string> codes, int sz, int &paddedBits) {
    string outputBuffer = "";
    for (int i = 0; i < sz; i++) {
        // Generate the Huffman bitstring
        outputBuffer = outputBuffer + codes[buffer[i]];
    }
    if (outputBuffer.size() % 8 != 0) {
        int deficit = 8 * ((outputBuffer.size() / 8) + 1) - outputBuffer.size();
        paddedBits = deficit;
        for (int i = 0; i < deficit; i++) {
            outputBuffer += "0";
        }
    }
    return outputBuffer; //Return the Huffman-encoded bitstring
}
/*getBufferFromString function converts a string of binary digits into a vector of bytes,grouping every 8 bits */
unsigned char *getBufferFromString(string bitstring, vector<unsigned char> &outputBuffer, int &sz) {
    int interval = 0;
    unsigned char bit = 0;
    for (int i = 0; i < sz; i++) {
        bit = (bit << 1) | (bitstring[i] - '0');
        interval++;
        if (interval == 8) {
            interval = 0;
            outputBuffer.push_back(bit);
            bit = 0;
        }
    }
    sz = outputBuffer.size();
    return outputBuffer.data();
}
/*getStringFromBuffer function converts a buffer of bytes into a string of binary digits.*/
string getStringFromBuffer(unsigned char *buffer, int sz) {
    string bitstring = "";
    for (int i = 0; i < sz; i++) {
        bitstring += toBinary(buffer[i]);
    }
    return bitstring;
}

/*getDecodedBuffer function takes a Huffman-encoded bitstring and decodes it back into the original data. 
It does this by reversing the Huffman codes to quickly look up characters and accumulating bits until a complete Huffman code is matched. 
Once a code is matched, it adds the corresponding character to the output buffer and continues until all bits are processed.
*/
unsigned char *getDecodedBuffer(string bitstring, vector<unsigned char> &buffer, map<unsigned char, string> codes,
                                int &sz, int paddedBits) {
    string bit = "";
    map<string, unsigned char> reversecodes; // keys are Huffman codes and values are the corresponding characters.
    for (map<unsigned char, string>::iterator i = codes.begin(); i != codes.end(); i++) {
        reversecodes[i->second] = i->first;
    }
    for (int i = 0; i < bitstring.size() - paddedBits; i++) {
        bit += string(1, bitstring[i]);
        if (reversecodes.find(bit) != reversecodes.end()) {
            buffer.push_back(reversecodes[bit]);
            bit = "";
        }
    }
    sz = buffer.size();
    return buffer.data();
}
/*writeHeader function writes crucial metadata to the file before the actual compressed data. 
metadata--> includes the number of padded bits, the size of the Huffman codes map(table), and each character(key) along with its Huffman code(value).
 This information is necessary for correctly decoding the compressed file later.
*/
void writeHeader(const char *path, map<unsigned char, string> codes, int paddedBits) {
    int size = codes.size();
    writeFileFromBuffer(path, (unsigned char *) &paddedBits, sizeof(int), 0);
    writeFileFromBuffer(path, (unsigned char *) &size, sizeof(int), 1);
    for (map<unsigned char, string>::iterator i = codes.begin(); i != codes.end(); i++) {
        unsigned char key = i->first;
        int len = i->second.size();
        writeFileFromBuffer(path, (unsigned char *) &key, 1, 1);
        writeFileFromBuffer(path, (unsigned char *) &len, sizeof(int), 1);
        writeFileFromBuffer(path, (unsigned char *) i->second.c_str(), len, 1);
    }
}
/*readHeader function reads the header from a compressed file, extracting metadata*/
unsigned char *readHeader(unsigned char *buffer, map<unsigned char, string> &codes, int &paddedBits, int &sz) {
    /* unsigned char *buffer: A pointer to the buffer containing the file data.
    */
    paddedBits = *((int *) buffer);
    buffer = buffer + 4;
    sz -= 4;
    int size = *((int *) buffer);
    buffer = buffer + 4;
    sz -= 4;
    for (int i = 0; i < size; i++) {
        unsigned char key = buffer[0];
        buffer++;
        sz--;
        int len = *((int *) buffer);
        buffer += 4;
        sz -= 4;
        char *value = (char *) malloc(len + 1);
        for (int j = 0; j < len; j++) {
            value[j] = buffer[j];
        }
        buffer += len;
        sz -= len;
        value[len] = '\0';
        codes[key] = value;
    }
    return buffer;
}

/*
compressFile function reads a file, calculates the frequency of each character, builds a Huffman tree, generates Huffman codes, encodes the file content using these codes,
and writes both the header and the compressed data to an output file. 
The header contains metadata necessary for decompression, such as the Huffman codes and the number of padded bits.
*/
void compressFile(const char *path, const char *output_path, map<unsigned char, string> &codes) {
    int sz = 0;
    int paddedBits = 0;
    map<unsigned char, int> freqtable;
    // Read the file into a buffer and get the size
    unsigned char *buffer = readFileIntoBuffer(path, sz);

    // Build frequency table
    for (int i = 0; i < sz; i++) {
        freqtable[buffer[i]]++;
    }
     // Convert frequency table to a vector of pairs and build Huffman tree
    Tree *root = buildHuffmanTree(convertToVector(freqtable));

    // Traverse the Huffman tree to get codes for each character
    traverseHuffmanTree(root, "", "", codes);

    // Get the Huffman encoded bitstring for the file content
    string outputString = getHuffmanBitstring(buffer, codes, sz, paddedBits);
    // Update size to the size of the output bitstring
    sz = outputString.size();

     // Convert the bitstring to a buffer of bytes
    vector<unsigned char> outputBufferV;
    getBufferFromString(outputString, outputBufferV, sz);
    unsigned char *outputBuffer = outputBufferV.data();

    // Write the header (codes and padded bits) to the output file
    writeHeader(output_path, codes, paddedBits);

    // Write the compressed data to the output file
    writeFileFromBuffer(output_path, outputBuffer, sz, 1);
}
/*decompressFile function reads a compressed file, extracts the Huffman codes and padded bits from the header,
 converts the compressed data to a bitstring, decodes the bitstring using the Huffman codes, and writes the decompressed data to an output file. 
*/
void decompressFile(const char *inputPath, const char *outputPath) {
    int sz = 0;
    map<unsigned char, string> codes;
    int paddedBits = 0;
    unsigned char *fileBuffer = readFileIntoBuffer(inputPath, sz);
    fileBuffer = readHeader(fileBuffer, codes, paddedBits, sz);
     //Convert the remaining buffer to a bitstring
    string fileBitString = getStringFromBuffer(fileBuffer, sz);

      // Decode the bitstring using the Huffman codes
    vector<unsigned char> outputBufferV;
    unsigned char *outputBuffer;
    getDecodedBuffer(fileBitString, outputBufferV, codes, sz, paddedBits);
    // Get the raw data pointer from the vector
    outputBuffer = outputBufferV.data();

    // Write the decompressed data to the output file
    writeFileFromBuffer(outputPath, outputBuffer, sz, 0);
}

int main() {
    const char *inputFile = "test.txt";
    const char *compressedFile = "test1.txt";
    const char *decompressedFile = "decoded.txt";
    
    map<unsigned char, string> codes;
    compressFile(inputFile, compressedFile, codes);
    decompressFile(compressedFile, decompressedFile);
    
    cout << "Compression and decompression completed successfully!" << endl;
    return 0;
}
