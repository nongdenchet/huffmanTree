#include "huffmanTree.h"
#include <fstream>
#include <string>
#include <vector>

int main() {
    std::vector<std::string> inputFileNames;
    inputFileNames.emplace_back("./InputFile.txt");
    inputFileNames.emplace_back("./InputFile2.txt");
    inputFileNames.emplace_back("./InputFile3.txt");

    std::vector<std::string> compressedFileNames;
    compressedFileNames.emplace_back("./CompressedFile.txt");
    compressedFileNames.emplace_back("./CompressedFile2.txt");
    compressedFileNames.emplace_back("./CompressedFile3.txt");

    for (int i = 0; i < inputFileNames.size(); i++) {
        std::string text;
        std::vector<char> symbol;
        std::vector<int> freq;

        const std::string& inputFileName = inputFileNames[i];
        std::ifstream inputFile(inputFileName);
        while (getline(inputFile, text)) {
            char c;
            int num;
            c = text[0];
            num = stoi(text.substr(2));
            symbol.push_back(c);
            freq.push_back(num);
        }
        inputFile.close();

        HuffmanTree ht;
        ht.setVector(symbol, freq);
        ht.createHuffmanTree();
        int arr[symbol.size()];
        ht.printLeafNodes(ht.getRoot(), arr, 0);
        ht.readCompressedFile(compressedFileNames[i]);
        ht.createThreads();
    }

    return 0;
}
