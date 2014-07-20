#include "gtest/gtest.h"
#include "darwin.hpp"
#include "Tokenizer.hpp"
#include <string>
#include <vector>

using namespace Darwin;
using namespace std;

class TokenizerValidator;
using Tokenizer4Test = TokenizerT<TokenizerValidator>;
class TokenizerValidator {
    public:
        void validateWordMap(const Tokenizer4Test& tokenizer, const Tokenizer4Test::WordMapType& wordMap) {
            ASSERT_EQ(tokenizer._wordMap.size(), wordMap.size());
            for (const auto& w : tokenizer._wordMap) {
                auto wordInfo = wordMap.find(w.first);
                ASSERT_NE(wordMap.end(), wordInfo);
                ASSERT_EQ(wordInfo->second, w.second); 
            }
        }
};

TEST(TokenizerTest, SplitBySingleChar) {
    Tokenizer4Test tokenizer;
    string sentence = "I have a dream";
    vector<string> expWords = {"I", "have", "a", "dream"};
    auto words = tokenizer.split(sentence);
    ASSERT_EQ(expWords.size(), words.size());
    for (int i = 0; i < expWords.size(); i++) {
        ASSERT_EQ(expWords[i], words[i]);
    }
}

TEST(TokenizerTest, SplitByMutiChar) {
    Tokenizer4Test tokenizer;
    string sentence = "I have a dream, a very big dream.";
    vector<string> expWords = {"I", "have", "a", "dream", "a", "very", "big", "dream"};
    auto words = tokenizer.split(sentence, " ,.");
    ASSERT_EQ(expWords.size(), words.size());
    for (int i = 0; i < expWords.size(); i++) {
        ASSERT_EQ(expWords[i], words[i]);
    }
}

TEST(TokenizerTest, TokenizeBySingleChar) {
    Tokenizer4Test tokenizer;
    string sentence = "I have a dream a very very very big dream Do you have a dream";
    vector<WordIdType> expWordIds = {1, 2, 3, 4, 3, 5, 5, 5, 6, 4, 7, 8, 2, 3, 4};
    auto wordIds = tokenizer.tokenize(sentence);
    for (int i = 0; i < expWordIds.size(); i++) {
        ASSERT_EQ(expWordIds[i], wordIds[i]);
    }

    TokenizerValidator validator;
    Tokenizer4Test::WordMapType wordMap = {
        {"I", 1}, {"have", 2}, {"a", 3}, {"dream", 4}, 
        {"very", 5}, {"big", 6}, {"Do", 7}, {"you", 8}
    };
    validator.validateWordMap(tokenizer, wordMap);
}

TEST(TokenizerTest, TokenizeByMultiChar) {
    Tokenizer4Test tokenizer;
    string sentence = "I have a dream, a very very very big dream. Do you have a dream?";
    vector<WordIdType> expWordIds = {1, 2, 3, 4, 3, 5, 5, 5, 6, 4, 7, 8, 2, 3, 4};
    auto wordIds = tokenizer.tokenize(sentence, " ,.?");
    for (int i = 0; i < expWordIds.size(); i++) {
        ASSERT_EQ(expWordIds[i], wordIds[i]);
    }

    TokenizerValidator validator;
    Tokenizer4Test::WordMapType wordMap = {
        {"I", 1}, {"have", 2}, {"a", 3}, {"dream", 4}, 
        {"very", 5}, {"big", 6}, {"Do", 7}, {"you", 8}
    };
    validator.validateWordMap(tokenizer, wordMap);
}
