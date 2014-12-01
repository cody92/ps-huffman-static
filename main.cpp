#include <iostream>
#include <string.h>
#include <stdio.h>
#include <vector>
#include <map>
#include <bits/stl_algo.h>
#include <bitset>
#include <fstream>
#include <chrono>

using namespace std;


typedef std::pair<unsigned char, unsigned long int> Symbol;
typedef std::map<unsigned char, std::pair<std::string, unsigned char>> EncodedSymbol;
typedef vector<Symbol> AlphabetList;
typedef std::chrono::duration<int, std::milli> miliseconds_type;

typedef struct Alphabet {
    AlphabetList data;
    unsigned long long int dataSize;
    unsigned long int alphabetSize;
};
typedef struct GraphTree {
    Symbol data;
    GraphTree *next;
    GraphTree *previous;
    GraphTree *left;
    GraphTree *right;
} ShannonTree;

int ENCODE_BITS = 8;
EncodedSymbol globalSymbolMapped;


auto cmp = [](std::pair<unsigned char, unsigned long int> const &a, std::pair<unsigned char, unsigned long int> const &b) {
    return a.second != b.second ? a.second > b.second : a.first < b.first;
};

vector<long int> computeAlphabet(std::string fileName) {
    vector<long int> alphabet(256);
    unsigned char temp;
    ifstream inputFile;

    inputFile.open(fileName.c_str(), ios::binary);

    if (!inputFile.is_open()) {
        std::cout << "Unable to open input file '" << fileName << "'!";
        exit(-1);
        return alphabet;
    }
    inputFile.unsetf(ios_base::skipws);

    while (!inputFile.eof()) {
        inputFile >> temp;
        alphabet[temp]++;
    }
    alphabet[temp]--;
    inputFile.close();
    return alphabet;
}

Alphabet extractAlphabet(vector<long int> inputAlphabet) {
    unsigned long long int sizeOfData = 0;
    vector<long int>::size_type inputLength, index;
    AlphabetList alphabetList;
    Alphabet alphabet;
    Symbol temp;
    inputLength = inputAlphabet.size();
    for (index = 0; index < inputLength; index++) {
        if (inputAlphabet[index] > 0) {
            temp.second = inputAlphabet[index];
            temp.first = (char) index;
            alphabetList.push_back(temp);
            sizeOfData++;
        }
    }

    alphabet.data = alphabetList;
    alphabet.dataSize = sizeOfData;
    return alphabet;
}

void displayAlphabet(AlphabetList alphabet) {
    printf("Other alphabet\n");
    for (Symbol &x: alphabet) {
        printf("%d,%d\n", x.first, x.second);

    }
}

void buildAlphabetMap(GraphTree *tree, std::string tempChar, char type, unsigned char deepth) {
    if (type == 1) {
        tempChar.push_back('1');
    } else if (type == 0) {
        tempChar.push_back('0');
    }
    if (tree->left == NULL && tree->right == NULL) {
        std::pair<std::string, unsigned char> tempSymbol = {tempChar, deepth};
        //std::cout << tempChar << " " << ((tree->data).first) << "\n";

        globalSymbolMapped.insert(
                globalSymbolMapped.begin(),
                std::pair<unsigned char, std::pair<std::string, unsigned char>>((tree->data).first, tempSymbol)
        );
    } else {
        buildAlphabetMap(tree->left, tempChar, 0, deepth + 1);
        buildAlphabetMap(tree->right, tempChar, 1, deepth + 1);
    }

}

GraphTree *getLastItem(GraphTree *graph) {
    while (graph->next != NULL) {
        graph = graph->next;
    }
    return graph;
}

void displayInitialGraph(GraphTree *alphabet) {

    std::cout << "\n\n";
    while (alphabet != NULL) {
        std::cout << (alphabet->data).first << " " << (alphabet->data).second << "\n";
        alphabet = alphabet->next;

    }
}

bool compareNodes(GraphTree *nodeA, GraphTree *nodeB) {
    if ((nodeA->data).second > (nodeB->data).second) {
        return true;
    } else if ((nodeA->data).second == (nodeB->data).second) {
        if ((nodeA->data).first >= (nodeB->data).first) {
            return true;
        } else {
            return false;
        }
    } else {
        return false;
    }
}


GraphTree *buildGraphTree(GraphTree *graph, unsigned char size) {

    GraphTree *itemGraph, *lastItem, *iteration, *parent;
    for (unsigned char index = 0; index < size - 1; index++) {

        parent = graph;
        //std::cout << "Pas: " << index << "\n";
        lastItem = getLastItem(parent);
        itemGraph = new GraphTree;
        itemGraph->left = lastItem->previous;
        itemGraph->right = lastItem;
        lastItem = lastItem->previous->previous;
        itemGraph->data = Symbol(0, (itemGraph->left->data).second + (itemGraph->right->data).second);
        if (itemGraph->left->previous->next != NULL) {
            itemGraph->left->previous->next = NULL;
        }

        itemGraph->left->next = NULL;
        itemGraph->left->previous = NULL;
        itemGraph->right->next = NULL;
        itemGraph->right->previous = NULL;


        iteration = lastItem;
        while (iteration->previous != NULL) {
            if (compareNodes(itemGraph, iteration) == true) {
                iteration = iteration->previous;
            } else {
                break;
            }
        }

        if (iteration->previous == NULL && compareNodes(itemGraph, iteration) == true) {
            itemGraph->next = graph;
            graph->previous = itemGraph;
            graph = itemGraph;
            itemGraph->previous = NULL;
        } else {
            itemGraph->next = iteration->next;
            iteration->next->previous = itemGraph;
            itemGraph->previous = iteration;
            iteration->next = itemGraph;
        }

        //displayInitialGraph(graph);

    }

    itemGraph = new GraphTree;
    itemGraph->left = graph;
    itemGraph->previous = NULL;
    itemGraph->next = NULL;
    itemGraph->right = graph->next;
    graph->right->previous = NULL;
    graph->next = NULL;
    itemGraph->data = Symbol(0, (itemGraph->left->data).second + (itemGraph->right->data).second);
    graph = itemGraph;

    //displayInitialGraph(graph);
    return graph;

}

GraphTree *initGraphTree(AlphabetList structuredAlphabet) {
    GraphTree *parent, *currentItem, *newItem;
    Symbol tempData;

    parent = NULL;
    for (Symbol &x: structuredAlphabet) {
//        std::cout << x.first << " " << x.second << "\n";
        if (parent == NULL) {
            parent = new GraphTree;
            parent->data = x;
            parent->left = NULL;
            parent->right = NULL;
            parent->next = NULL;
            parent->previous = NULL;
            currentItem = parent;
        } else {
            newItem = new GraphTree;
            newItem->data = x;
            newItem->left = NULL;
            newItem->right = NULL;
            newItem->next = NULL;
            newItem->previous = currentItem;
            currentItem->next = newItem;
            currentItem = newItem;
        }

    }

    return parent;
}

void writeAuxiliaryData(std::string fileName, AlphabetList alphabet) {
    ofstream outputFile;
    unsigned char dataSize, symbol;

    outputFile.open(fileName.c_str(), ios::binary);
    if (!outputFile.is_open()) {
        std::cout << "Could not open output file! " << fileName;
        exit(-1);
    }

    dataSize = alphabet.size() - 1;


    outputFile.unsetf(ios_base::skipws);
    outputFile << dataSize;

    for (Symbol &x:alphabet) {

        std::bitset<32> bits(x.second);
        std::bitset<8> tempBit;
        std::string byte1, byte2, byte3, byte4, byte;
        byte = bits.to_string();
        //std::cout << bits << " -> ";

        byte1 = byte.substr(0, 8);
        byte2 = byte.substr(8, 8);
        byte3 = byte.substr(16, 8);
        byte4 = byte.substr(24, 8);


        std::bitset<8> tempBit1(byte1), tempBit2(byte2), tempBit3(byte3), tempBit4(byte4);
        //std::cout << (int) tempBit1.to_ulong() << " " << (int) tempBit2.to_ulong() << " " << (int) tempBit3.to_ulong() << " " << (int) tempBit4.to_ulong() << "\n";

        symbol = x.first;
        outputFile << symbol << (unsigned char) tempBit1.to_ulong() << (unsigned char) tempBit2.to_ulong() << (unsigned char) tempBit3.to_ulong() << (unsigned char) tempBit4.to_ulong();
    }

    //std::cout << outputFile.tellp() << " uu\n";
    outputFile.close();

}

Alphabet readAuxiliaryData(std::string fileName) {
    ifstream inputFile;
    AlphabetList tempAlphabet;
    Alphabet alphabet;
    AlphabetList::iterator it;
    unsigned char noOfApp1, noOfApp2, noOfApp3, noOfApp4, character, dataSize;
    Symbol *tempSymbol;
    int index, dataSizeI;
    unsigned long long int totalCharacters = 0, alphabetSize = 0;

    inputFile.open(fileName.c_str(), ios::binary);

    if (!inputFile.is_open()) {
        std::cout << "Unable to open file " << fileName << "\n";
        exit(-1);
    }

    inputFile.unsetf(ios_base::skipws);
    it = tempAlphabet.begin();
    inputFile >> dataSize;
    dataSizeI = dataSize;
    dataSizeI++;
    for (index = 0; index < dataSizeI; index++) {

        inputFile >> character >> noOfApp1 >> noOfApp2 >> noOfApp3 >> noOfApp4;

        tempSymbol = new Symbol;
        tempSymbol->first = character;
        tempSymbol->second = noOfApp1 * pow(2, 24) + noOfApp2 * pow(2, 16) + noOfApp3 * pow(2, 8) + noOfApp4;
        tempAlphabet.push_back(*tempSymbol);
        it++;
        totalCharacters += noOfApp1 * pow(2, 24) + noOfApp2 * pow(2, 16) + noOfApp3 * pow(2, 8) + noOfApp4;
        alphabetSize++;
    }

    inputFile.close();
    alphabet.data = tempAlphabet;
    alphabet.alphabetSize = alphabetSize;
    alphabet.dataSize = totalCharacters;

    return alphabet;

}

void encodeFile(std::string fileName, std::string outputFileName, unsigned char alphabetSize) {

    unsigned int remainLength, newLength, length;
    unsigned char tempChar;
    ifstream inputFile;
    ofstream outputFile;
    std::string tempOutput, newString;


    inputFile.open(fileName.c_str(), ios::binary);
    if (inputFile == NULL) {
        std::cout << "Unable to open file '" << fileName << "'";
        exit(-1);
    }

    outputFile.open(outputFileName, ios::binary | ios_base::app);
    if (!outputFile.is_open()) {
        std::cout << "Unable to open file '" << outputFileName << "'";
        exit(-1);
    }

    inputFile.unsetf(ios_base::skipws);
    outputFile.unsetf(ios_base::skipws);

    //outputFile.seekp(5 * alphabetSize + 1, inputFile.beg);

    while (!inputFile.eof()) {
        inputFile >> tempChar;
        newString = ((globalSymbolMapped[tempChar]).first);
        newLength = newString.length();
        length = tempOutput.length() + newLength;

        while (newString.length()) {

            if (tempOutput.length() + newString.length() <= ENCODE_BITS) {
                tempOutput += newString;
                newString = "";
                //std::cout << "<- " << tempOutput << "  \n";
            } else {
                remainLength = ENCODE_BITS - tempOutput.length();
                tempOutput += newString.substr(0, remainLength);
                //std::cout << "<- " << tempOutput << "";
                newString = newString.substr(remainLength);
                //std::cout << " -- " << newString << " ->\n";

            }

            if (tempOutput.length() == ENCODE_BITS) {
                std::bitset<8> tempBit(tempOutput);
                outputFile << (char) tempBit.to_ulong();
                //std::cout << "bit ->" << tempBit << "\n";
                tempOutput = "";
            }
        }


    }

    while (tempOutput.length() < ENCODE_BITS) {
        tempOutput.push_back('0');
    }
    std::bitset<8> tempBit(tempOutput);
    outputFile << (char) tempBit.to_ulong();
    //std::cout << "\n\n";

    inputFile.close();
    outputFile.close();

}


AlphabetList getAlphabet(std::string fileName) {
    vector<long int> alphabet;
    Alphabet structuredAlphabet;
    AlphabetList generatedAlphabet;

    alphabet = computeAlphabet(fileName);
    structuredAlphabet = extractAlphabet(alphabet);
    //displayAlphabet(structuredAlphabet.data);
    std::sort(structuredAlphabet.data.begin(), structuredAlphabet.data.end(), cmp);
    generatedAlphabet.insert(
            generatedAlphabet.begin(),
            structuredAlphabet.data.begin(),
            structuredAlphabet.data.begin() + structuredAlphabet.dataSize
    );

    return generatedAlphabet;
}

void compressFile(std::string fileName) {
    std::string compressedFileName;
    AlphabetList alphabet;
    GraphTree *alphabetGraph;
    alphabetGraph = new GraphTree;

    compressedFileName = fileName + ".psh";

    alphabet = getAlphabet(fileName);
//    displayAlphabet(alphabet);
    alphabetGraph = initGraphTree(alphabet);
    //displayInitialGraph(alphabetGraph);
    alphabetGraph = buildGraphTree(alphabetGraph, alphabet.size() - 1);
    buildAlphabetMap(alphabetGraph, std::string(""), 2, 0);
    writeAuxiliaryData(compressedFileName, alphabet);
    encodeFile(fileName, compressedFileName, alphabet.size());
}

void uncompressFile(std::string fileName) {
    std::string fileCompressed, fileUncompressed;
    Alphabet alphabet;
    GraphTree *alphabetGraph, *tempGraph;
    ifstream inputFile;
    ofstream outputFile;
    unsigned char tempValue;
    unsigned long long int numberOfCharacters = 0;


    fileCompressed = fileName + ".psh";
    fileUncompressed = fileName + ".pshu";


    inputFile.open(fileCompressed.c_str(), ios::binary);
    outputFile.open(fileUncompressed, ios::binary);

    if (!inputFile.is_open()) {
        std::cout << "Unable to open file " << fileCompressed << "\n";
        exit(-1);
    }

    if (!outputFile.is_open()) {
        std::cout << "Unable to open file " << fileUncompressed << "\n";
        exit(-1);
    }

    inputFile.unsetf(ios_base::skipws);
    alphabet = readAuxiliaryData(fileCompressed);
//    displayAlphabet(alphabet.data);
    alphabetGraph = initGraphTree(alphabet.data);
//    displayInitialGraph(alphabetGraph);
    alphabetGraph = buildGraphTree(alphabetGraph, alphabet.alphabetSize - 1);
    tempGraph = alphabetGraph;

    inputFile.unsetf(ios_base::skipws);
    outputFile.unsetf(ios_base::skipws);
    inputFile.seekg(5 * alphabet.alphabetSize + 1, inputFile.beg);


    while (!inputFile.eof() && numberOfCharacters < alphabet.dataSize) {
        inputFile >> tempValue;
        //std::cout << tempValue << " ";
        std::bitset<8> bitsValues(tempValue);
        //std::cout << bitsValues << " -> ";

        for (int i = bitsValues.size() - 1; i >= 0 && numberOfCharacters < alphabet.dataSize; --i) {
            //std::cout << bitsValues.test(i);
            if (bitsValues.test(i)) {
                //std::cout << '1';
                tempGraph = tempGraph->right;
                if (tempGraph->left == NULL && tempGraph->right == NULL) {
                    outputFile << (tempGraph->data).first;
                    //std::cout << " " << (tempGraph->data).first << " ";
                    tempGraph = alphabetGraph;
                    numberOfCharacters++;
                }
            } else {
                //std::cout << '0';
                tempGraph = tempGraph->left;
                if (tempGraph->left == NULL && tempGraph->right == NULL) {
                    outputFile << (tempGraph->data).first;
                    //std::cout << " " << (tempGraph->data).first << " ";
                    tempGraph = alphabetGraph;
                    numberOfCharacters++;
                }
            }
        }
        //std::cout << "\n";
    }
    inputFile.close();
    outputFile.close();
}

void displayOptions() {
    std::cout << "Alege fisierul!" << "\n\n Audio:\n";
    std::cout << "\t1.)instr_01.wav\n";
    std::cout << "\t2.)sound_01.wav\n";
    std::cout << "\t3.)speech_01.wav\n";
    std::cout << "\nDocuments:\n";
    std::cout << "\t4.)Documentatie_UMAPID.doc\n";
    std::cout << "\t5.)Documentatie_UMAPID.pdf\n";
    std::cout << "\t6.)Prefata_Undine.txt\n";
    std::cout << "\t7.)show_audio.m\n";
    std::cout << "\t8.)Y04.M\n";
    std::cout << "\nExecutables:\n";
    std::cout << "\t9.)KARMA_DATA482#1_5_V7.mat\n";
    std::cout << "\t10.)quartz.dll\n";
    std::cout << "\t11.)WinRar.exe\n";
    std::cout << "\t12.)WINZIP32.EXE\n";


}

std::ifstream::pos_type filesize(std::string fileName) {
    std::ifstream in(fileName.c_str(), std::ifstream::ate | std::ifstream::binary);
    return in.tellg();
}

void displayInformations(std::string fileName) {
    std::ifstream::pos_type compressedFileSize, fileSize, uncompressedFileSize;
    std::string compressedFileName, uncompressedFileName;

    compressedFileName = fileName + ".psh";
    uncompressedFileName = fileName + ".pshu";

    compressedFileSize = filesize(compressedFileName);
    fileSize = filesize(fileName);
    uncompressedFileSize = filesize(uncompressedFileName);

    std::cout << "Norma: " << (fileSize - uncompressedFileSize) * (fileSize - uncompressedFileSize) << "\n";
    std::cout << "Rata compresie: " << (1 - ((float) compressedFileSize) / ((float) uncompressedFileSize)) * 100 << "%\n";
    std::cout << "Factor compresie:  " << (((float) compressedFileSize) / ((float) fileSize)) * 100 << "%\n";


}


int main() {

    std::vector<std::string> files = {
            ".\\audio\\instr_01.wav",
            ".\\audio\\sound_01.wav",
            ".\\audio\\speech_01.wav",
            ".\\documents\\Documentatie_UMAPID.doc",
            ".\\documents\\Documentatie_UMAPID.pdf",
            ".\\documents\\Prefata_Undine.txt",
            ".\\documents\\show_audio.m",
            ".\\documents\\Y04.M",
            ".\\executables\\KARMA_DATA482#1_5_V7.mat",
            ".\\executables\\quartz.dll",
            ".\\executables\\WinRar.exe",
            ".\\executables\\WINZIP32.EXE",
            ".\\documents\\input.txt",
            "D:\\input.txt",
    };

    int option;
    displayOptions();
    std::cout << "Select file!..\n";
    std::cin >> option;
    if (option < 1 && option > 14) {
        std::cout << "Invalid option!";
        return 0;
    }
    auto startC = std::chrono::high_resolution_clock::now();


    compressFile(files[option - 1]);
    auto endC = std::chrono::high_resolution_clock::now();
    auto timeC = endC - startC;
    std::cout << "Compressed time: " << std::chrono::duration_cast<miliseconds_type>(timeC).count() << " miliseconds.\n";

    auto startU = std::chrono::high_resolution_clock::now();
    uncompressFile(files[option - 1]);

    auto endU = std::chrono::high_resolution_clock::now();
    auto timeU = endU - startU;
    std::cout << "Uncompressed time: " << std::chrono::duration_cast<miliseconds_type>(timeU).count() << " miliseconds.\n";
    displayInformations(files[option - 1]);

    std::cin >> option;

    return 0;
}