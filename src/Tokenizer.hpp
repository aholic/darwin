#ifndef __TOKENIZER_HPP__
#define __TOKENIZER_HPP__

#include <vector>
#include <string>
#include <algorithm>
#include "darwin.hpp"

namespace Darwin {
    using WordMapType = unordered_map<string, WordIdType>;
    class Tokenizer {
        public:
            vector<string> split(const string& sentence, const string& delims = " ") const {
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

            Tokenizer(size_t avgWordLength = 5) : avgWordLength(avgWordLength) {}
            
            vector<WordIdType> tokenize(const string& sentence) {
                auto words = split(sentence);
                for_each(words.begin(), words.end(), []);
            }

        private:
            size_t avgWordLength = 5;
            WordMapType _wordMap;
    };
}
#endif
