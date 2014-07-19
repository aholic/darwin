#include "gtest/gtest.h"
#include "darwin.hpp"
#include "Tokenizer.hpp"
#include <string>
#include <vector>

using namespace Darwin;

TEST(TokenizerTest, SplitBySingleChar) {
    using namespace std;
    Tokenizer tokenizer;
    string sentence = "I have a dream";
    vector<string> expWords = {"I", "have", "a", "dream"};
    auto words = tokenizer.split(sentence);
    ASSERT_EQ(expWords.size(), words.size());
    for (int i = 0; i < expWords.size(); i++) {
        ASSERT_EQ(expWords[i], words[i]);
    }
}

TEST(TokenizerTest, SplitByMutiChar) {
    using namespace std;
    Tokenizer tokenizer;
    string sentence = "I have a dream, a very big dream.";
    vector<string> expWords = {"I", "have", "a", "dream", "a", "very", "big", "dream"};
    auto words = tokenizer.split(sentence, " ,.");
    ASSERT_EQ(expWords.size(), words.size());
    for (int i = 0; i < expWords.size(); i++) {
        ASSERT_EQ(expWords[i], words[i]);
    }
}

TEST(TokenizerTest, TokenizeBySingleChar) {
    using namespace std;
    Tokenizer tokenizer;
    string sentence = "I have a dream a very very very big dream Do you have a dream";
    vector<WordIdType> expWordIds = {0, 1, 2, 3, 2, 4, 4, 4, 5, 3, 6, 7, 1, 2, 3};
    auto wordIds = tokenizer.tokenize(sentence);
    for (int i = 0; i < expWordIds.size(); i++) {
        ASSERT_EQ(expWordIds[i], wordIds[i]);
    }
}

TEST(TokenizerTest, TokenizeByMultiChar) {
    using namespace std;
    Tokenizer tokenizer;
    string sentence = "I have a dream, a very very very big dream. Do you have a dream?";
    vector<WordIdType> expWordIds = {0, 1, 2, 3, 2, 4, 4, 4, 5, 3, 6, 7, 1, 2, 3};
    auto wordIds = tokenizer.tokenize(sentence, " ,.?");
    for (int i = 0; i < expWordIds.size(); i++) {
        ASSERT_EQ(expWordIds[i], wordIds[i]);
    }
}
