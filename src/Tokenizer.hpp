#ifndef __TOKENIZER_HPP__
#define __TOKENIZER_HPP__

#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <algorithm>
#include "darwin.hpp"

namespace Darwin {

    template <typename Validator>
    class TokenizerT {
        friend Validator;

        public:
            using WordMapType = unordered_map<string, WordIdType>;
            size_t _avgWordLength = 5;
            WordMapType _wordMap;
        public:
            explicit TokenizerT(size_t avgWordLength = 5) :
                _avgWordLength(avgWordLength) {}
            TokenizerT(const TokenizerT& tokenizer) :
                _avgWordLength(tokenizer._avgWordLength),
                _wordMap(tokenizer._wordMap) {}
            TokenizerT(TokenizerT&& tokenizer) : 
                _avgWordLength(tokenizer._avgWordLength), 
                _wordMap(move(tokenizer._wordMap)) {}
            explicit TokenizerT(const string& backupFileName) {
                _deserialize(backupFileName);
            }
            explicit TokenizerT(ifstream& fin) {
                _deserialize(fin);
            }
            
            WordIdType getWordId(const string& word) const {
                auto wordInfo = _wordMap.find(word);
                if (wordInfo == _wordMap.end()) return 0;
                return wordInfo->second;
            }

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

            vector<WordIdType> tokenize(const string& sentence, const string& delims = " ") {
                auto words = split(sentence, delims);
                vector<WordIdType> ret;
                for (const auto& word : words) {
                    ret.push_back(_update(word));
                }
                return ret;
            }

            void serialize(const string& dumpFileName) const {
                ofstream fout(dumpFileName, ios_base::out | ios_base::binary);
                serialize(fout);
                fout.close();
            }

            void serialize(ofstream& fout) const {
                fout.write((reinterpret_cast<const char*>(&_avgWordLength)), sizeof(_avgWordLength));
                for (const auto& w : _wordMap) {
                    size_t wordLen = w.first.length();
                    size_t wordId = w.second;
                    fout.write((reinterpret_cast<const char*>(&wordLen)), sizeof(wordLen));
                    fout.write(w.first.c_str(), wordLen);
                    fout.write((reinterpret_cast<const char*>(&wordId)), sizeof(wordId));
                }
            }
        private:
            void _deserialize(ifstream& fin) {
                fin.read((reinterpret_cast<char*>(&_avgWordLength)), sizeof(_avgWordLength));
                _wordMap.clear();

                char buff[256];
                size_t wordLen;
                size_t wordId;
                while (fin) {
                    fin.read((reinterpret_cast<char*>(&wordLen)), sizeof(wordLen));
                    fin.read(buff, wordLen);
                    buff[wordLen] = '\0';
                    fin.read((reinterpret_cast<char*>(&wordId)), sizeof(wordId));
                    _wordMap.insert(make_pair(string(buff), wordId));
                }
            }
            void _deserialize(const string& backupFileName) {
                ifstream fin(backupFileName, ios_base::in | ios_base::binary);
                _deserialize(fin);
                fin.close();
            }

            WordIdType _update(const string& word) {
                auto wordInfo = _wordMap.find(word);
                if (wordInfo != _wordMap.end()) return wordInfo->second; 

                auto wordId = _wordMap.size();
                _wordMap.insert(make_pair(word, wordId+1));
                return (wordId+1);
            }
    };

    using Tokenizer = TokenizerT<int>;
}
#endif
