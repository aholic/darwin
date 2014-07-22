#ifndef __INDEXBUILDER_HPP__
#define __INDEXBUILDER_HPP__

#include <string>
#include <fstream>
#include "darwin.hpp"
#include "Tokenizer.hpp"
#include <unordered_set>

namespace Darwin {
    template <typename Validator>
    class IndexBuilderT {
        friend Validator;

        public:
            struct InvertedIndexValueType {
                DocIdType docId;
                size_t offset;
                size_t lineno;
                InvertedIndexValueType(DocIdType docId, size_t offset, size_t lineno) :
                    docId(docId), offset(offset), lineno(lineno) {}
                InvertedIndexValueType(const InvertedIndexValueType& obj) : 
                    docId(obj.docId), offset(obj.offset), lineno(obj.lineno) {}
                bool operator == (const InvertedIndexValueType& rhs) const {
                    if (rhs.docId != docId) return false;
                    if (rhs.offset != offset) return false;
                    if (rhs.lineno != lineno) return false;
                    return true;
                }
            };

            struct hashFunc {
                size_t operator() (const InvertedIndexValueType &val) const {
                    std::hash<size_t> hasher;
                    size_t ret = val.docId;
                    ret ^= hasher(val.offset) + 0x9e3779b9 + (ret << 6) + (ret >> 2);
                    ret ^= hasher(val.lineno) + 0x9e3779b9 + (ret << 6) + (ret >> 2);
                    return ret;
                }
            };
            using InvertedIndexType = unordered_map<WordIdType, unordered_set<InvertedIndexValueType, hashFunc>>;
            using DocumentListType = vector<string>;

        private:
            DocumentListType _documents; 
            Tokenizer _tokenizer;
            InvertedIndexType _index;
            string _dataDirectory;

        public:
            explicit IndexBuilderT(const Tokenizer& tokenizer) : _tokenizer(tokenizer) {}
            explicit IndexBuilderT(Tokenizer&& tokenizer) : _tokenizer(tokenizer) {}

            void serialize(ofstream& fout) {
                _tokenizer.serialize(fout);
                size_t dataDirectoryLen = _dataDirectory.length();
                fout.write((reinterpret_cast<const char*>(&dataDirectoryLen)), sizeof(dataDirectoryLen));
                fout.write(_dataDirectory.c_str(), dataDirectoryLen);
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
    using IndexBuilder = IndexBuilderT<int>;
}

#endif
