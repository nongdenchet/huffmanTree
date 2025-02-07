# OS Assignment
## Assignment 1

Instruction is in pa1-instructions.docx. To run solution, execute following comman

```
g++ -std=c++11 -o ./main ./main.cpp ./huffmanTree.cpp && ./main
```

## Assignment 2

Instruction is in pa2-instructions.docx. To run solution, execute following commands

### Server

```
g++ -std=c++11 -o ./server ./huffmanTree.cpp ./server.cpp && ./server 3000 < InputFile2.txt
```

### Client

```
g++ -std=c++11 -o ./client ./client.cpp && ./client localhost 3000 < CompressedFile2.txt
```
