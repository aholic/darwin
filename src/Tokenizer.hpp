#ifndef __TOKENIZER_HPP__
#define __TOKENIZER_HPP__

#include <vector>
#include <string>
#include <algorithm>

class Tokenizer {
    public:
        std::vector<std::string> split(const std::string& sentence, const std::string& delims) {
            using namespace std;
            vector<string> words;
            string::size_type length = sentence.length();
            words.reserve(length / avgWordLength);

            string::size_type pos = 0, lastPos = 0;
            while (pos != length) {
                pos = sentence.find_first_of(delims, lastPos);
                if (pos == string::npos) pos = length;
                if (pos != lastPos) words.push_back(string(sentence.data()+lastPos, pos-lastPos));
                lastPos = pos + 1;
            }

            return words;
        }

        Tokenizer(int avgWordLength = 5) : avgWordLength(avgWordLength) {}

    private:
        int avgWordLength = 5;
};

#endif
