#ifndef __INDEXBUILDER_HPP__
#define __INDEXBUILDER_HPP__

#include <string>
#include <fstream>
#include "Tokenizer.hpp"
#include "darwin.hpp"

namespace Darwin {
    class IndexBuilder {
        public:
            struct InvertedIndexValueType {
                DocIdType docId;
                size_t offset;
                size_t lineno;
                InvertedIndexValueType(DocIdType docId, size_t offset, size_t lineno) :
                    docId(docId), offset(offset), lineno(lineno) {}
                InvertedIndexValueType(const InvertedIndexValueType& obj) : 
                    docId(obj.docId), offset(obj.offset), lineno(obj.lineno) {}
            };
            using InvertedIndexType = unordered_map<WordIdType, vector<InvertedIndexValueType>>;
            using DocumentListType = vector<string>;

        public:
            IndexBuilder(const Tokenizer& tokenizer) : _tokenizer(tokenizer) {}
            IndexBuilder(Tokenizer&& tokenizer) : _tokenizer(tokenizer) {}

            void build(const string& documents) {
                _documents = _fillDocList(documents);
                _index = _buildInvertedIndex(_documents); 
            }

            SearchResultType search(const string& word) {
                string lineContent;
                WordIdType wordId = _tokenizer.getWordId(word);
                auto docs = _index[wordId];

                SearchResultType result;
                for (const auto& doc : docs) {
                    lineContent = _getLineContent(doc.docId, doc.offset);
                    result.push_back(Result(doc.docId, _documents[doc.docId], doc.lineno, lineContent));
                }

                return result;
            }

        private:
            DocumentListType _documents; 
            Tokenizer _tokenizer;
            InvertedIndexType _index;
            string _dataDirectory = "../data/";
        private:
            string _getLineContent(DocIdType docId, size_t offset) {
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
                        index[wordId].push_back(InvertedIndexValueType(docId, offset, lineno));
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
}

#endif
