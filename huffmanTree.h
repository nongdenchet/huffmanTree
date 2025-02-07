#include <chrono>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <pthread.h>
#include <queue>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>
#include <map>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"

struct Node {
    char symbol;
    int freq;
    Node *left;
    Node *right;
    int priority;

    Node(char s, int f, Node *l = nullptr, Node *r = nullptr, int priority = 0) {
        this->symbol = s;
        this->freq = f;
        this->left = l;
        this->right = r;
        this->priority = priority;
    }
};

void printArray(int arr[], int size);

class NodeCompare {
public:
    bool operator()(Node *a, Node *b);
};

class HuffmanTree {
private:
    std::priority_queue<Node *, std::vector<Node *>, NodeCompare> orderedList;
    Node *root;
    std::map<char *, std::vector<int>> binCodeToPositionsMap;
    int currentPriority;
public:
    HuffmanTree() {
        root = nullptr;
        currentPriority = 0;
    }

    ~HuffmanTree() { delete root; }

    void setVector(std::vector<char> &, std::vector<int> &);

    void createHuffmanTree();

    void printLeafNodes(Node *node, int arr[], int size);

    Node *getRoot();

    void readCompressedFile(const std::string &);

    void createThreads();

    char decode(char *binCode);
};

struct VoidPtr {
    std::vector<int> positions;
    char *binCode;
    HuffmanTree *tree;
    char *finalMessage;
    pthread_mutex_t *lock;
};

#pragma clang diagnostic pop
