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

// Write compressed data to a file with .huff extension
void writeCompressedFile(const string& compressedData, const string& originalFileName) {
    string compressedFileName = originalFileName + ".huff";
    ofstream outFile(compressedFileName, ios::binary);

    if (outFile.is_open()) {
        // Convert the binary string to bytes and write to the file
        for (size_t i = 0; i < compressedData.size(); i += 8) {
            bitset<8> byte(compressedData.substr(i, 8));
            outFile.put(static_cast<char>(byte.to_ulong()));
        }

        outFile.close();
        cout << "File compressed successfully. Compressed file: " << compressedFileName << endl;
    } else {
        cerr << "Error writing compressed file." << endl;
    }
}

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

int main() {
    string fileName;
    cout << "Enter the name of the file to compress: ";
    cin >> fileName;

    // Read file content
    string message = readFile(fileName);

    // Calculate character frequencies
    map<char, int> frequencyMap;
    for (char c : message) {
        frequencyMap[c]++;
    }

    // Build Huffman tree
    HuffmanNode* root = buildHuffmanTree(frequencyMap);

    // Build code table
    map<char, string> codeTable;
    buildCodeTable(root, "", codeTable);

    // Encode message
    string encodedMessage = encodeMessage(message, codeTable);

    // Write compressed data to file
    writeCompressedFile(encodedMessage, fileName);

    // Clean up allocated memory
    delete root;

    return 0;
}
