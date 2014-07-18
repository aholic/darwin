#ifndef __TOKENIZER_HPP__
#define __TOKENIZER_HPP__

#include <vector>
#include <string>
#include <algorithm>
#include "darwin.hpp"

namespace Darwin {
    class Tokenizer {
        using WordMapType = unordered_map<string, WordIdType>;
        public:
            Tokenizer(size_t avgWordLength = 5) : _avgWordLength(avgWordLength) {}
            Tokenizer(Tokenizer&& tokenizer) : 
                _avgWordLength(tokenizer._avgWordLength) , 
                _wordMap(move(tokenizer._wordMap)) {}
            
            vector<string> split(const string& sentence, const string& delims = " ") const {
                vector<string> words;
                auto length = sentence.length();
                words.reserve(length / _avgWordLength);

                decltype(length) pos = 0, lastPos = 0;
                while (pos != length) {
                    pos = sentence.find_first_of(delims, lastPos);
                    if (pos == string::npos) pos = length;
                    if (pos != lastPos) words.push_back(string(sentence.data()+lastPos, pos-lastPos));
                    lastPos = pos + 1;
                }

                return words;
            }

            vector<WordIdType> tokenize(const string& sentence) {
                auto words = split(sentence);
                vector<WordIdType> ret;
                for (const auto word : words) {
                    ret.push_back(_update(word));
                }
                return ret;
            }

        private:
            size_t _avgWordLength = 5;
            WordMapType _wordMap;

            WordIdType _update(const string& word) {
                auto wordInfo = _wordMap.find(word);
                if (wordInfo != _wordMap.end()) return wordInfo->second; 

                auto wordId = _wordMap.size();
                _wordMap[word] = wordId;
                return wordId;
            }
    };
}
#endif
