#include "gtest/gtest.h"
#include "darwin.hpp"
#include "CompareOperator.hpp"
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
            ASSERT_EQ(tokenizer._wordMap, wordMap);
        }

        void validateAvgWordLength(const Tokenizer4Test& tokenizer, size_t avgWordLength) {
            ASSERT_EQ(tokenizer._avgWordLength, avgWordLength);
        }

        void validateEqual(const Tokenizer4Test& t1, const Tokenizer4Test& t2) {
            validateAvgWordLength(t1, t2._avgWordLength);
            validateWordMap(t1, t2._wordMap);
        }
};

TEST(TokenizerTest, SplitBySingleChar) {
    Tokenizer4Test tokenizer;
    string sentence = "I have a dream";
    vector<string> expWords = {"I", "have", "a", "dream"};
    auto words = tokenizer.split(sentence);
    ASSERT_EQ(expWords, words);
}

TEST(TokenizerTest, SplitByMutiChar) {
    Tokenizer4Test tokenizer;
    string sentence = "I have a dream, a very big dream.";
    vector<string> expWords = {"I", "have", "a", "dream", "a", "very", "big", "dream"};
    auto words = tokenizer.split(sentence, " ,.");
    ASSERT_EQ(expWords, words);
}

TEST(TokenizerTest, TokenizeBySingleChar) {
    Tokenizer4Test tokenizer;
    string sentence = "I have a dream a very very very big dream Do you have a dream";
    vector<WordIdType> expWordIds = {1, 2, 3, 4, 3, 5, 5, 5, 6, 4, 7, 8, 2, 3, 4};
    auto wordIds = tokenizer.tokenize(sentence);
    ASSERT_EQ(expWordIds, wordIds);

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
    ASSERT_EQ(expWordIds, wordIds);

    TokenizerValidator validator;
    Tokenizer4Test::WordMapType wordMap = {
        {"I", 1}, {"have", 2}, {"a", 3}, {"dream", 4}, 
        {"very", 5}, {"big", 6}, {"Do", 7}, {"you", 8}
    };
    validator.validateWordMap(tokenizer, wordMap);
}

TEST(TokenizerTest, Serialization) {
    vector<string> sentences = {
        "I have a dream, a very very very big dream. Do you have a dream?",
        "hello word, i love all the world!",
        "my name is xu ruochen, what is your name?"
    };

    for (int i = 0; i < sentences.size(); i++ ) {
        Tokenizer4Test tokenizer;
        tokenizer.tokenize(sentences[i], " ,.!?");

        tokenizer.serialize("dump/tokenizer_dump");
        Tokenizer4Test backupTokenizer("dump/tokenizer_dump");

        TokenizerValidator validator;
        validator.validateEqual(tokenizer, backupTokenizer);
    }
}
