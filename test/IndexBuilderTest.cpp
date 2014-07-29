#include "gtest/gtest.h"
#include "darwin.hpp"
#include "IndexBuilder.hpp"
#include "CompareOperator.hpp"
#include <string>
#include <vector>
#include <unordered_set>
#include <typeinfo>

using namespace Darwin;
using namespace std;

class IndexBuilderValidator;
using IndexBuilder4Test = IndexBuilderT<IndexBuilderValidator>;
class IndexBuilderValidator {
    public:
        void validateDataDirectory(const IndexBuilder4Test& indexBuilder, const string& dataDirectory) {
            ASSERT_EQ(indexBuilder._dataDirectory, dataDirectory);
        }
        void validateDocList(const IndexBuilder4Test& indexBuilder, const DocumentListType& docList) {
            ASSERT_EQ(indexBuilder._documents, docList);
        }
        void validateInvertedIndex(const IndexBuilder4Test& indexBuilder, const InvertedIndexType& index) {
           ASSERT_EQ(indexBuilder._index.size(), index.size()); 
           for (const auto& i : indexBuilder._index) {
               auto info = index.find(i.first);
               ASSERT_NE(info, index.end());
               ASSERT_EQ(typeid(info->second), typeid(i.second));
               ASSERT_EQ(info->second.size(), i.second.size());
               for (const auto& v : info->second) {
                   ASSERT_NE(i.second.find(v), i.second.end());
               }
           }
        }
        void validateLineContent(const IndexBuilder4Test& indexBuilder, const pair<DocIdType, size_t>& pos, const string& lineContent) {
            ASSERT_EQ(indexBuilder._getLineContent(pos.first, pos.second), lineContent);
        }
        void validateSearchResult(const IndexBuilder4Test& indexBuilder, const string& key, const SearchResultType& result) {
            SearchResultType r = indexBuilder.search(key);
            ASSERT_EQ(r, result);
        }
};

TEST(IndexBuilderTest, SetDataDirectory) {
    IndexBuilder4Test indexBuilder((Tokenizer()));
    IndexBuilderValidator validator;

    vector<string> documents = {"../documents", "documents", "sub/documents", "/documents", "./sub/documents"};
    vector<string> expDataDirectory = {"../", "./", "sub/", "/", "./sub/"};

    for (int i = 0; i < documents.size(); i++) {
        indexBuilder.build(documents[i]);
        validator.validateDataDirectory(indexBuilder, expDataDirectory[i]);
    }
}

TEST(IndexBuilderTest, FillDocList) {
    IndexBuilder4Test indexBuilder((Tokenizer()));
    IndexBuilderValidator validator;

    indexBuilder.build("data/documents");
    DocumentListType expDocList = {"doc1", "doc2", "doc3", "doc4"};
    validator.validateDocList(indexBuilder, expDocList);
}

TEST(IndexBuilderTest, BuildInvertedIndex) {
    IndexBuilder4Test indexBuilder((Tokenizer()));
    IndexBuilderValidator validator;

    indexBuilder.build("data/documents");

    // harry : docId = 0, offset = 11, lineno = 1 | docId = 1, offset = 0, lineno = 0
    InvertedIndexType index = {
        {indexBuilder.getWordId("shell"), {{0, 0, 0}}} ,
        {indexBuilder.getWordId("code"), {{0, 0, 0}}} ,
        {indexBuilder.getWordId("harry"), {{0, 11, 1}, {1, 0, 0}}} ,
        {indexBuilder.getWordId("potter"), {{0, 11, 1}, {1, 0, 0}}} ,
        {indexBuilder.getWordId("i"), {{2, 0, 0}}} ,
        {indexBuilder.getWordId("have"), {{2, 0, 0}, {3, 0, 0}}} ,
        {indexBuilder.getWordId("a"), {{2, 0, 0}, {3, 0, 0}}} ,
        {indexBuilder.getWordId("dream"), {{2, 0, 0}, {3, 0, 0}}} ,
        {indexBuilder.getWordId("do"), {{3, 0, 0}}} ,
        {indexBuilder.getWordId("you"), {{3, 0, 0}}} 
    };

    validator.validateInvertedIndex(indexBuilder, index);
}

TEST(IndexBuilderTest, GetLineContent) {
    IndexBuilder4Test indexBuilder((Tokenizer()));
    IndexBuilderValidator validator;

    indexBuilder.build("data/documents");

    vector<pair<DocIdType, size_t>> pos = {{0, 0}, {0, 11}, {1, 0}};
    vector<string> expLineContent = {"shell code", "harry potter", "harry potter"};

    for (int i = 0; i < pos.size(); i++) {
        validator.validateLineContent(indexBuilder, pos[i], expLineContent[i]);
    }
}

TEST(IndexBuilderTest, Search) {
    IndexBuilder4Test indexBuilder((Tokenizer()));
    IndexBuilderValidator validator;

    indexBuilder.build("data/documents");
    vector<string> keys = {"shell", "harry", "dream", "ruochen"};
    vector<SearchResultType> expResults = {
        {{0, "doc1", 0, "shell code"}},
        {{1, "doc2", 0, "harry potter"}, {0, "doc1", 1, "harry potter"}},
        {{3, "doc4", 0, "do you have a dream"}, {2, "doc3", 0, "i have a dream"}},
        {{}}
    };

    for (int i = 0; i < keys.size(); i++) {
        validator.validateSearchResult(indexBuilder, keys[i], expResults[i]);
    }
}

TEST(IndexBuilderTest, Serialization) {
    const string fname = "dump/index_builder";
    IndexBuilder4Test indexBuilder((Tokenizer()));
    indexBuilder.build("data/documents");
    Serializer serializer;

    serializer.serialize(fname, indexBuilder);
    IndexBuilder4Test backupIndexBuilder((Tokenizer()));
    serializer.deserialize(fname, backupIndexBuilder);
    ASSERT_EQ(indexBuilder, backupIndexBuilder);
}
