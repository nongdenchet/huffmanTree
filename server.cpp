#include <unistd.h>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string>
#include <iostream>
#include <sys/wait.h>
#include "huffmanTree.h"

void fireman(int) {
    while (waitpid(-1, NULL, WNOHANG) > 0)
        std::cout << "A child process ended" << std::endl;
}

void error(const std::string &message) {
    std::cout << message << std::endl;
    exit(1);
}

struct message {
    char binCode[256];
};

struct message2 {
    char symbol;
};

int main(int argc, char *argv[]) {
    int sockfd, newsockfd, portno, clilen, n;
    struct message msg1;
    struct sockaddr_in serv_addr, cli_addr;
    if (argc < 2) {
        fprintf(stderr, "ERROR, no port provided\n");
        exit(1);
    }

    // create socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    // bind socket
    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding");
    }

    // read input file
    std::vector<char> symbol;
    std::vector<int> freq;
    std::string line;
    while (getline(std::cin, line)) {
        char c;
        int num;
        c = line[0];
        num = stoi(line.substr(2));
        symbol.push_back(c);
        freq.push_back(num);
    }

    // create huffmanTree
    auto *ht = new HuffmanTree();
    ht->setVector(symbol, freq);
    ht->createHuffmanTree();
    int arr[symbol.size()];
    ht->printLeafNodes(ht->getRoot(), arr, 0);

    // listen on socket
    listen(sockfd, 1000);
    signal(SIGCHLD, fireman);
    while (true) {
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, (socklen_t *) &clilen);
        if (newsockfd < 0) {
            error("ERROR on accept");
        }

        if (fork() == 0) {
            n = read(newsockfd, &msg1, sizeof(struct message));
            if (n < 0) {
                error("ERROR reading from socket");
            }

            struct message2 m2;
            m2.symbol = ht->decode(msg1.binCode);
            n = write(newsockfd, &m2, sizeof(struct message2));
            if (n < 0) {
                error("ERROR writing to socket");
            }
        }
    }
}
