#ifndef __INDEXBUILDER_HPP__
#define __INDEXBUILDER_HPP__

#include <string>
#include <fstream>
#include "darwin.hpp"
#include "Tokenizer.hpp"
#include "Serializer.hpp"
#include <unordered_set>

namespace Darwin {
    template <typename Validator>
    class IndexBuilderT;

    template <typename Validator>
    inline bool operator == (const IndexBuilderT<Validator>& i1, const IndexBuilderT<Validator>& i2);

    template <typename Validator>
    inline bool operator != (const IndexBuilderT<Validator>& i1, const IndexBuilderT<Validator>& i2);

    template <typename Validator>
    class IndexBuilderT {
        friend Validator;
        friend bool operator == <> (const IndexBuilderT& i1, const IndexBuilderT& i2);
        friend bool operator != <> (const IndexBuilderT& i1, const IndexBuilderT& i2);

        public:
            using InvertedIndexType = unordered_map<WordIdType, unordered_set<InvertedIndexValueType, HashFunc>>;
            using DocumentListType = vector<string>;

        private:
            DocumentListType _documents; 
            Tokenizer _tokenizer;
            InvertedIndexType _index;
            string _dataDirectory;

        public:
            explicit IndexBuilderT(const Tokenizer& tokenizer) : _tokenizer(tokenizer) {}
            explicit IndexBuilderT(Tokenizer&& tokenizer) : _tokenizer(tokenizer) {}
            IndexBuilderT& operator = (IndexBuilderT&& rhs) {
                _documents = move(rhs._documents);
                _tokenizer = move(rhs._tokenizer);
                _index = move(rhs._index);
                _dataDirectory = move(rhs._dataDirectory);
            }
            IndexBuilderT& operator = (const IndexBuilderT& rhs) {
                _documents = rhs._documents;
                _tokenizer = rhs._tokenizer;
                _index = rhs._index;
                _dataDirectory = rhs._dataDirectory;
            }

            WordIdType getWordId(const string& word) const {
                return _tokenizer.getWordId(word);
            }

            void build(const string& documents) {
                auto pos = documents.find_last_of("/");
                _dataDirectory = (pos == string::npos ? "." : documents.substr(0, documents.find_last_of("/")));
                _dataDirectory += "/";
                _documents = _fillDocList(documents);
                _index = _buildInvertedIndex(_documents); 
            }

            SearchResultType search(const string& word) const {
                string lineContent;
                SearchResultType result;
                WordIdType wordId = _tokenizer.getWordId(word);
                auto docs = _index.find(wordId);
                if (docs == _index.end()) return result;

                for (const auto& doc : docs->second) {
                    lineContent = _getLineContent(doc.docId, doc.offset);
                    result.push_back(Result(doc.docId, _documents[doc.docId], doc.lineno, lineContent));
                }

                return result;
            }
        private:
            string _getLineContent(DocIdType docId, size_t offset) const {
                auto docName = _dataDirectory+_documents[docId];
                ifstream doc(docName);
                doc.seekg(offset);
                
                string line;
                getline(doc, line);
                doc.close();
                return line;
            }

            InvertedIndexType _buildInvertedIndex(const DocumentListType& documents) {
                InvertedIndexType index;
                int documentNum = documents.size();
                for (size_t i = 0; i< documentNum; i++) {
                    _buildInvertedIndexAux(i, _dataDirectory+documents[i], index);
                }
                return index;
            }

            void _buildInvertedIndexAux(DocIdType docId, const string& docName, InvertedIndexType& index) {
                ifstream doc(docName);
                string line;
                size_t lineno = 0;
                size_t offset = 0;

                while (getline(doc, line)) {
                    auto buf = _tokenizer.tokenize(line);
                    for (const auto& wordId : buf) {
                        index[wordId].insert(InvertedIndexValueType(docId, offset, lineno));
                    }
                    lineno += 1;
                    offset = doc.tellg();
                }
                doc.close();
            }

            DocumentListType _fillDocList(const string& documents) const {
                DocumentListType docList; 
                ifstream content(documents);
                string line;
                while (getline(content, line)) {
                    auto buf = _tokenizer.split(line);
                    if (2 != buf.size()) continue;
                    docList.push_back(buf[1]);
                }
                content.close();
                return docList;
            }
    };

    template<typename Validator>
    inline bool operator == (const IndexBuilderT<Validator>& lhs, const IndexBuilderT<Validator>& rhs) {
        if (lhs._tokenizer != rhs._tokenizer) return false;
        if (lhs._documents != rhs._documents) return false;
        if (lhs._index != rhs._index) return false;
        if (lhs._dataDirectory != rhs._dataDirectory) return false;
        return true;
    }

    template<typename Validator>
    inline bool operator != (const IndexBuilderT<Validator>& lhs, const IndexBuilderT<Validator>& rhs) {
        if (lhs._tokenizer != rhs._tokenizer) return true;
        if (lhs._documents != rhs._documents) return true;
        if (lhs._index != rhs._index) return true;
        if (lhs._dataDirectory != rhs._dataDirectory) return true;
        return false;
    }

    using IndexBuilder = IndexBuilderT<int>;
}

#endif
