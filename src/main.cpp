#include <iostream>
#include <unordered_map>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include "Tokenizer.hpp"

using namespace std;

int main() {
    const int BUFF_SIZE = 2048;
    FILE *dataFile = fopen("../data/harry.txt", "r");
    char buff[BUFF_SIZE];
    Tokenizer tokenizer;
    vector<string> words;
    unordered_map<string, int> wordCnt;

    while (fscanf(dataFile, "%s", buff) != EOF) {
        words = tokenizer.split(buff, " .,\"':?-");
        for (const auto word : words) {
            wordCnt[word] += 1;
        }
    }

    return 0;
}
