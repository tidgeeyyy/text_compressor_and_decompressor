#include <iostream>
#include <fstream>
#include <queue>
#include <bitset>
#include <map>

using namespace std;

// Node structure for Huffman tree
struct HuffmanNode {
    char data;
    int frequency;
    HuffmanNode* left;
    HuffmanNode* right;

    HuffmanNode(char data, int frequency) : data(data), frequency(frequency), left(nullptr), right(nullptr) {}
};

// Comparison function for priority queue
struct Compare {
    bool operator()(HuffmanNode* a, HuffmanNode* b) {
        return a->frequency > b->frequency;
    }
};

// Read a file and return its content as a string
string readFile(const string& fileName) {
    ifstream inFile(fileName, ios::binary | ios::ate);
    string content = "";

    if (inFile.is_open()) {
        // Obtain the size of the file
        streampos fileSize = inFile.tellg();

        // Set the position indicator to the beginning
        inFile.seekg(0, ios::beg);

        // Resize the string to fit the entire file content
        content.resize(fileSize);

        // Read the file content into the string
        inFile.read(&content[0], fileSize);

        inFile.close();
    } else {
        cerr << "Error reading file." << endl;
    }

    return content;
}

// Build Huffman tree
HuffmanNode* buildHuffmanTree(const map<char, int>& frequencyMap) {
    priority_queue<HuffmanNode*, vector<HuffmanNode*>, Compare> pq;

    for (const auto& pair : frequencyMap) {
        pq.push(new HuffmanNode(pair.first, pair.second));
    }

    while (pq.size() > 1) {
        HuffmanNode* left = pq.top();
        pq.pop();
        HuffmanNode* right = pq.top();
        pq.pop();

        HuffmanNode* internalNode = new HuffmanNode('$', left->frequency + right->frequency);
        internalNode->left = left;
        internalNode->right = right;

        pq.push(internalNode);
    }

    return pq.top();
}

// Traverse Huffman tree and build code table
void buildCodeTable(HuffmanNode* root, string code, map<char, string>& codeTable) {
    if (root) {
        if (root->data != '$') {
            codeTable[root->data] = code;
        }

        buildCodeTable(root->left, code + "0", codeTable);
        buildCodeTable(root->right, code + "1", codeTable);
    }
}

// Encode a message using Huffman coding
string encodeMessage(const string& message, const map<char, string>& codeTable) {
    string encodedMessage = "";

    for (char c : message) {
        encodedMessage += codeTable.at(c);
    }

    return encodedMessage;
}

// Decode a message using Huffman coding
string decodeMessage(const string& encodedMessage, HuffmanNode* root) {
    string decodedMessage = "";
    HuffmanNode* currentNode = root;

    for (char bit : encodedMessage) {
        if (bit == '0') {
            currentNode = currentNode->left;
        } else {
            currentNode = currentNode->right;
        }

        if (currentNode->data != '$') {
            decodedMessage += currentNode->data;
            currentNode = root;
        }
    }

    return decodedMessage;
}

// Write decompressed data to a file
void writeDecompressedFile(const string& decompressedData, const string& originalFileName) {
    string decompressedFileName = originalFileName.substr(0, originalFileName.find_last_of('.')) + "_decompressed.txt";
    ofstream outFile(decompressedFileName);

    if (outFile.is_open()) {
        outFile << decompressedData;
        outFile.close();
        cout << "File decompressed successfully. Decompressed file: " << decompressedFileName << endl;
    } else {
        cerr << "Error writing decompressed file." << endl;
    }
}

int main() {
    string fileName;
    cout << "Enter the name of the file to decompress: ";
    cin >> fileName;

    // Read compressed file content
    string compressedMessage = readFile(fileName);

    // Calculate character frequencies from the compressed message
    map<char, int> frequencyMap;
    for (char c : compressedMessage) {
        frequencyMap[c]++;
    }

    // Build Huffman tree
    HuffmanNode* root = buildHuffmanTree(frequencyMap);

    // Build code table
    map<char, string> codeTable;
    buildCodeTable(root, "", codeTable);

    // Encode message
    string encodedMessage = encodeMessage(compressedMessage, codeTable);

    // Decode message
    string decodedMessage = decodeMessage(encodedMessage, root);

    // Write decompressed data to a separate file
    writeDecompressedFile(decodedMessage, fileName);

    // Clean up allocated memory
    delete root;

    // Compare original and decoded messages
    if (compressedMessage == decodedMessage) {
        cout << "Original message matches decoded message." << endl;
    } else {
        cerr << "Error: Original message does not match decoded message." << endl;
    }

    return 0;
}
