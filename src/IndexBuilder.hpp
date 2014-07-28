#ifndef __INDEXBUILDER_HPP__
#define __INDEXBUILDER_HPP__

#include <string>
#include <fstream>
#include "darwin.hpp"
#include "Tokenizer.hpp"
#include "Serializer.hpp"
#include <unordered_set>

namespace Darwin {
    struct InvertedIndexValueType {
        DocIdType docId;
        size_t offset;
        size_t lineno;
        InvertedIndexValueType(DocIdType docId, size_t offset, size_t lineno) :
            docId(docId), offset(offset), lineno(lineno) {}
        InvertedIndexValueType(const InvertedIndexValueType& obj) : 
            docId(obj.docId), offset(obj.offset), lineno(obj.lineno) {}
        InvertedIndexValueType() :
            docId(0), offset(0), lineno(0) {}
        bool operator == (const InvertedIndexValueType& rhs) const {
            if (rhs.docId != docId) return false;
            if (rhs.offset != offset) return false;
            if (rhs.lineno != lineno) return false;
            return true;
        }
    };

    template <>
    struct SerializeFunc<InvertedIndexValueType> {
        void operator () (ofstream& fout, const InvertedIndexValueType& v) const {
            SerializeFunc<DocIdType>()(fout, v.docId);
            SerializeFunc<size_t>()(fout, v.offset);
            SerializeFunc<size_t>()(fout, v.lineno);
        }
    };

    template <>
    struct DeserializeFunc<InvertedIndexValueType> {
        void operator () (ifstream& fin, InvertedIndexValueType& v) const {
            DeserializeFunc<DocIdType>()(fin, v.docId);
            DeserializeFunc<size_t>()(fin, v.offset);
            DeserializeFunc<size_t>()(fin, v.lineno);
        }
    };

    struct HashFunc {
        size_t operator() (const InvertedIndexValueType &val) const {
            std::hash<size_t> hasher;
            size_t ret = val.docId;
            ret ^= hasher(val.offset) + 0x9e3779b9 + (ret << 6) + (ret >> 2);
            ret ^= hasher(val.lineno) + 0x9e3779b9 + (ret << 6) + (ret >> 2);
            return ret;
        }
    };

    using InvertedIndexType = unordered_map<WordIdType, unordered_set<InvertedIndexValueType, HashFunc>>;

    template <> 
    struct SerializeFunc<InvertedIndexType> {
        void operator () (ofstream& fout, const InvertedIndexType& index) const {
            using size_type = typename InvertedIndexType::size_type;
            using value_type = typename InvertedIndexType::value_type;

            SerializeFunc<size_type>()(fout, index.size());
            for (const auto & i : index) {
                SerializeFunc<value_type>()(fout, i);
            }
        }
    };

    template <> 
    struct DeserializeFunc<InvertedIndexType> {
        void operator () (ifstream& fin, InvertedIndexType& index) const {
            using size_type = typename InvertedIndexType::size_type;
            using key_type = typename InvertedIndexType::key_type;
            using mapped_type = typename InvertedIndexType::mapped_type;

            size_type size;
            DeserializeFunc<size_type>()(fin, size);

            index.clear();
            index.reserve(size);

            pair<key_type, mapped_type> val;
            for (size_type i = 0; i < size; i++) {
                DeserializeFunc<pair<key_type, mapped_type>>()(fin, val);
                index.insert(val);
            }
        }
    };

    using DocumentListType = vector<string>;

    template <> 
    struct SerializeFunc<DocumentListType> {
        void operator () (ofstream& fout, const DocumentListType& docList) const {
            using size_type = typename DocumentListType::size_type;
            using value_type= typename DocumentListType::value_type;

            SerializeFunc<size_type>()(fout, docList.size());
            for (const auto & d : docList) {
                SerializeFunc<value_type>()(fout, d);
            }
        }
    };

    template <>
    struct DeserializeFunc<DocumentListType> {
        void operator () (ifstream& fin, DocumentListType& docList) const {
            using size_type = typename DocumentListType::size_type;
            using value_type= typename DocumentListType::value_type;

            size_type size;
            DeserializeFunc<size_type>()(fin, size);

            docList.clear();
            docList.reserve(size);

            for (size_type i = 0; i < size; i++) {
                value_type val;
                DeserializeFunc<value_type>()(fin, val);
                docList.push_back(val);
            }
        }
    };

    template <typename Validator>
    class IndexBuilderT;

    template <typename Validator>
    inline bool operator == (const IndexBuilderT<Validator>& i1, const IndexBuilderT<Validator>& i2);

    template <typename Validator>
    class IndexBuilderT {
        friend Validator;
        friend bool operator == <> (const IndexBuilderT& i1, const IndexBuilderT& i2);
        friend SerializeFunc<IndexBuilderT>;
        friend DeserializeFunc<IndexBuilderT>;


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

    template <typename Validator>
    struct SerializeFunc<IndexBuilderT<Validator>> {
        void operator () (ofstream& fout, const IndexBuilderT<Validator>& indexBuilder) const {
            SerializeFunc<Tokenizer>()(fout, indexBuilder._tokenizer);
            SerializeFunc<string>()(fout, indexBuilder._dataDirectory);
            SerializeFunc<DocumentListType>()(fout, indexBuilder._documents);
            SerializeFunc<InvertedIndexType>()(fout, indexBuilder._index);
        }
    };

    template <typename Validator>
    struct SerializeFunc<const IndexBuilderT<Validator>> {
        void operator () (ofstream& fout, const IndexBuilderT<Validator>& indexBuilder) const {
            SerializeFunc<Tokenizer>()(fout, indexBuilder._tokenizer);
            SerializeFunc<string>()(fout, indexBuilder._dataDirectory);
            SerializeFunc<DocumentListType>()(fout, indexBuilder._documents);
            SerializeFunc<InvertedIndexType>()(fout, indexBuilder._index);
        }
    };

    template <typename Validator>
    struct DeserializeFunc<IndexBuilderT<Validator>> {
        void operator () (ifstream& fin, IndexBuilderT<Validator>& indexBuilder) const {
            DeserializeFunc<Tokenizer>()(fin, indexBuilder._tokenizer);
            DeserializeFunc<string>()(fin, indexBuilder._dataDirectory);
            DeserializeFunc<DocumentListType>()(fin, indexBuilder._documents);
            DeserializeFunc<InvertedIndexType>()(fin, indexBuilder._index);
        }
    };

    template <typename Validator>
    inline bool operator == (const IndexBuilderT<Validator>& lhs, const IndexBuilderT<Validator>& rhs) {
        if (lhs._tokenizer != rhs._tokenizer) return false;
        if (lhs._documents != rhs._documents) return false;
        if (lhs._index != rhs._index) return false;
        if (lhs._dataDirectory != rhs._dataDirectory) return false;
        return true;
    }

    using IndexBuilder = IndexBuilderT<int>;
}

#endif
