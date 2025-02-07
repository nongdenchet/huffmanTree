#include <sstream>
#include "huffmanTree.h"

bool NodeCompare::operator()(Node *a, Node *b) {
    if (a->freq != b->freq) {
        return a->freq > b->freq;
    }

    if (a->priority != b->priority) {
        return a->priority < b->priority;
    }

    return a->symbol > b->symbol;
}

void HuffmanTree::createHuffmanTree() {
    // taking two nodes and implement into the tree
    // first two vectors then delete them then implement on tree

    while (orderedList.size() > 1) {
        // First smallest
        Node *left = orderedList.top();
        orderedList.pop();

        // Second smallest
        Node *right = orderedList.top();
        orderedList.pop();

        // New parent node
        int sumFreq = left->freq + right->freq;

        // currentPriority is just an increasing counter
        // it is important to lower the priority of newly created node according to requirement:
        // "To guarantee that your solution generates the expected output from the test cases,
        // every time you generate an internal node of the tree,
        // you must insert this node into the queue of nodes as the lowest node based on its frequency"
        Node *n = new Node('\n', sumFreq, left, right, currentPriority++); // space symbol for smallest ascii
        orderedList.push(n);
    }
    root = orderedList.top();
    orderedList.pop();
}

#pragma clang diagnostic push
#pragma ide diagnostic ignored "misc-no-recursion"

void HuffmanTree::printLeafNodes(Node *node, int arr[], int index) {
    if (node->left != nullptr) {
        arr[index] = 0;
        printLeafNodes(node->left, arr, index + 1);
    }
    if (node->right != nullptr) {
        arr[index] = 1;
        printLeafNodes(node->right, arr, index + 1);
    }

    if (node->left == nullptr && node->right == nullptr) {
        std::cout << "Symbol: " << node->symbol << ", Frequency: " << node->freq << ", Code: ";
        printArray(arr, index);
        return;
    }
}

#pragma clang diagnostic pop

void printArray(int arr[], int size) {
    std::string binCode;
    for (int i = 0; i < size; i++) {
        std::cout << arr[i];
    }
    std::cout << std::endl;
}

void printCharArray(char *arr, int size) {
    for (int i = 0; i < size; i++) {
        std::cout << arr[i];
    }
    std::cout << std::endl;
}

Node *HuffmanTree::getRoot() { return root; }

void HuffmanTree::readCompressedFile(const std::string &compressedFileName) {
    std::string text2;
    std::ifstream inputFile2(compressedFileName);

    while (getline(inputFile2, text2)) {
        std::vector<int> indices;
        std::string binCode = text2.substr(0, text2.find_first_of(' '));
        std::string part = text2.substr(binCode.length());

        std::string word;
        std::istringstream iss(part);
        while (std::getline(iss, word, ' ')) {
            if (!word.empty()) {
                indices.push_back(std::stoi(word));
            }
        }

        char *tmp = strcpy(new char[binCode.length() + 1], binCode.c_str());
        binCodeToPositionsMap[tmp] = indices;
    }
    inputFile2.close();
}

void *decompress(void *Ptr) {
    auto *vp = (VoidPtr *) Ptr;

    // decode binCode -> symbol
    char symbol = vp->tree->decode(vp->binCode);

    // memory access
    pthread_mutex_lock(vp->lock);
    for (int p: vp->positions) {
        vp->finalMessage[p] = symbol;
    }
    pthread_mutex_unlock(vp->lock);

    return nullptr;
}

void HuffmanTree::createThreads() {
    const unsigned long NTHREADS = binCodeToPositionsMap.size();
    char *finalMessage = new char[root->freq];
    for (int i = 0; i < root->freq; i++) {
        finalMessage[i] = '*';
    }

    int thread = 0;
    pthread_mutex_t lock;
    pthread_t tid[NTHREADS];
    for (auto &i: binCodeToPositionsMap) {
        auto *vp = new VoidPtr;
        vp->binCode = i.first;
        vp->tree = this;
        vp->positions = i.second;
        vp->finalMessage = finalMessage;
        vp->lock = &lock;
        if (pthread_create(&tid[thread++], nullptr, decompress, (void *) vp)) {
            std::cout << "Error creating the thread" << std::endl;
            return;
        }
    }

    for (unsigned long i = 0; i < NTHREADS; ++i) {
        pthread_join(tid[i], nullptr);
    }

    std::cout << "Original Message: ";
    printCharArray(finalMessage, root->freq);
    std::cout << std::endl;
}

void HuffmanTree::setVector(std::vector<char> &symbol, std::vector<int> &freq) {
    for (int i = 0; i < symbol.size(); i++) {
        Node *n = new Node(symbol[i], freq[i], nullptr, nullptr, 0);
        orderedList.push(n);
    }
}

char HuffmanTree::decode(char *binCode) {
    Node *node = root;
    size_t length = strlen(binCode);
    for (size_t i = 0; i < length; i++) {
        char c = binCode[i];
        if (c == '0') {
            node = node->left;
        } else if (c == '1') {
            node = node->right;
        } else {
            throw std::runtime_error("Invalid binCode");
        }
    }
    return node->symbol;
}
