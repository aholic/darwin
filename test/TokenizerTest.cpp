#include "gtest/gtest.h"
#include "Tokenizer.hpp"
#include <string>
#include <vector>

TEST(TokenizerTest, SplitBySingleChar) {
    using namespace std;
    Tokenizer tokenizer;
    string sentence = "I have a dream";
    vector<string> expWords = {"I", "have", "a", "dream"};
    vector<string> words = tokenizer.split(sentence, " ");
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
    vector<string> words = tokenizer.split(sentence, " ,.");
    ASSERT_EQ(expWords.size(), words.size());
    for (int i = 0; i < expWords.size(); i++) {
        ASSERT_EQ(expWords[i], words[i]);
    }
}
