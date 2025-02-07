// Please note this is a C program
// It compiles without warnings with gcc
#include <unistd.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <string>
#include <iostream>
#include <vector>
#include <sstream>

std::mutex mainThreadLock;

void error(const std::string &message) {
    std::cout << message << std::endl;
    exit(0);
}

struct message {
    char binCode[256];
};

struct message2 {
    char symbol;
};

struct VoidPtr {
    std::vector<int> positions;
    char *binCode;
    char *finalMessage;
    char *serverName;
    char *portNumber;
};

// send message to server on new thread
void *makeCall(void *Ptr) {
    VoidPtr *vp = (VoidPtr *) Ptr;
    struct sockaddr_in serv_addr;
    int sockfd, portno, n;
    struct message msg1;
    struct hostent *server;

    // port number
    portno = atoi(vp->portNumber);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        error("ERROR opening socket");
    }

    // server connection
    server = gethostbyname(vp->serverName);
    if (server == nullptr) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    // establish connection
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr, (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR connecting");
    }

    // send message
    strcpy(msg1.binCode, vp->binCode);
    n = write(sockfd, &msg1, sizeof(struct message));
    if (n < 0) {
        error("ERROR writing to socket");
    }

    // receive message
    struct message2 msg2;
    n = read(sockfd, &msg2, sizeof(struct message2));
    if (n < 0) {
        error("ERROR reading from socket: " + std::to_string(n));
    }

    // memory access
    mainThreadLock.lock();
    for (int i = 0; i < vp->positions.size(); i++) {
        int pos = vp->positions[i];
        vp->finalMessage[pos] = msg2.symbol;
    }
    mainThreadLock.unlock();
    return nullptr;
}

void printCharArray(char *arr, int size) {
    for (int i = 0; i < size; i++) {
        std::cout << arr[i];
    }
    std::cout << std::endl;
}

int main(int argc, char *argv[]) {
    int messageSize = 0;
    if (argc < 3) {
        fprintf(stderr, "usage %s hostname port\n", argv[0]);
        exit(0);
    }

    // read compressed file
    std::string text2;
    std::vector<VoidPtr *> inputs;
    while (getline(std::cin, text2)) {
        std::vector<int> indices;
        std::string binCode = text2.substr(0, text2.find_first_of(' '));
        std::string part = text2.substr(binCode.length());

        std::string word;
        std::istringstream iss(part);
        while (std::getline(iss, word, ' ')) {
            if (!word.empty()) {
                int pos = std::stoi(word);
                messageSize = messageSize > pos ? messageSize : pos;
                indices.push_back(pos);
            }
        }

        VoidPtr *vp = new VoidPtr;
        char *tmp = strcpy(new char[binCode.length() + 1], binCode.c_str());
        vp->binCode = tmp;
        vp->serverName = argv[1];
        vp->portNumber = argv[2];
        vp->positions = indices;
        inputs.push_back(vp);
    }

    // Create threads
    pthread_t tid[inputs.size()];
    char *finalMessage = new char[messageSize + 1];
    for (int i = 0; i < inputs.size(); i++) {
        inputs[i]->finalMessage = finalMessage;
        if (pthread_create(&tid[i], nullptr, makeCall, (void *) inputs[i])) {
            std::cout << "Error creating the thread" << std::endl;
            exit(0);
        }
    }

    // join thread
    for (unsigned long i = 0; i < inputs.size(); ++i) {
        pthread_join(tid[i], nullptr);
    }

    std::cout << "Original Message: ";
    printCharArray(finalMessage, messageSize + 1);
    return 0;
}
