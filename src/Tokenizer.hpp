#ifndef __TOKENIZER_HPP__
#define __TOKENIZER_HPP__

#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <algorithm>
#include "darwin.hpp"
#include "Serializer.hpp"

namespace Darwin {

    template <typename Validator>
    class TokenizerT;

    template <typename Validator>
    bool operator == (const TokenizerT<Validator>& t1, const TokenizerT<Validator>& t2);

    template <typename Validator>
    bool operator != (const TokenizerT<Validator>& t1, const TokenizerT<Validator>& t2);

    template <typename Validator>
    class TokenizerT {
        friend Validator;
        friend bool operator == <> (const TokenizerT& t1, const TokenizerT& t2);
        friend bool operator != <> (const TokenizerT& t1, const TokenizerT& t2);

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
            TokenizerT& operator = (TokenizerT&& rhs) {
                _wordMap = move(rhs._wordMap);
                _avgWordLength = move(rhs._avgWordLength);
                return *this;
            }
            TokenizerT& operator = (const TokenizerT& rhs) {
                _wordMap = rhs._wordMap;
                _avgWordLength = rhs._avgWordLength;
                return *this;
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
                Serializer serializer;
                serializer.serialize(fout, _avgWordLength);
                serializer.serialize(fout, _wordMap);
            }
        private:
            void _deserialize(ifstream& fin) {
                Serializer serializer;
                serializer.deserialize(fin, _avgWordLength);
                serializer.deserialize(fin, _wordMap);
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

    template<typename Validator>
    inline bool operator == (const TokenizerT<Validator>& lhs, const TokenizerT<Validator>& rhs) {
        return (lhs._avgWordLength == rhs._avgWordLength && lhs._wordMap == rhs._wordMap);
    }

    template<typename Validator>
    inline bool operator != (const TokenizerT<Validator>& lhs, const TokenizerT<Validator>& rhs) {
        return (lhs._avgWordLength != rhs._avgWordLength || lhs._wordMap != rhs._wordMap);
    }

    using Tokenizer = TokenizerT<int>;
}
#endif
