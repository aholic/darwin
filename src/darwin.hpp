#ifndef __DARWIN_HPP__
#define __DARWIN_HPP__

#include <iostream>
#include <vector>
#include <unordered_map>
#include "Serializer.hpp"
namespace Darwin {
    using namespace std;

    using WordIdType = size_t;
    using DocIdType = size_t;

    using WordMapType = unordered_map<string, WordIdType>;

    template <>
    struct SerializeFunc<WordMapType> {
        void operator () (ofstream& fout, const WordMapType& wordMap) {
            SerializeFunc<WordMapType::size_type>()(fout, wordMap.size());
            for (const auto & d : wordMap) {
                SerializeFunc<WordMapType::value_type>()(fout, d);
            }
        }
    };

    template <>
    struct SerializeFunc<const WordMapType> {
        void operator () (ofstream& fout, const WordMapType& wordMap) {
            SerializeFunc<WordMapType::size_type>()(fout, wordMap.size());
            for (const auto & d : wordMap) {
                SerializeFunc<WordMapType::value_type>()(fout, d);
            }
        }
    };

    template <>
    struct DeserializeFunc<WordMapType> {
        void operator () (ifstream& fin, WordMapType& data) const {
            typename WordMapType::size_type size;
            DeserializeFunc<typename WordMapType::size_type>()(fin, size);

            pair<WordMapType::key_type, WordMapType::mapped_type> val;
            for (typename WordMapType::size_type i = 0; i < size; i++) {
                DeserializeFunc<pair<WordMapType::key_type, WordMapType::mapped_type>>()(fin, val);
                data.insert(val);
            }
        }
    };

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
    struct SerializeFunc<const InvertedIndexValueType> {
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

    struct Result {
        DocIdType docId;
        string docName;
        size_t lineno;
        string lineContent;

        Result (DocIdType docId, const string& docName, size_t lineno, const string& lineContent) :
            docId(docId), docName(docName), lineno(lineno), lineContent(lineContent) {}
        Result (const Result& result) :
            docId(result.docId), docName(result.docName),
            lineno(result.lineno), lineContent(result.lineContent) {}
        Result (Result&& result) :
            docId(result.docId), docName(move(result.docName)),
            lineno(result.lineno), lineContent(move(result.lineContent)) {}
        bool operator == (const Result& rhs) const {
            if (rhs.docId != docId) return false;
            if (rhs.docName != docName) return false;
            if (rhs.lineno != lineno) return false;
            if (rhs.lineContent != lineContent) return false;
            return true;
        }
    };

    inline ostream& operator << (ostream& out, const Result& result) {
        out << result.docName << "(";
        out << "line ";
        out << result.lineno << "): " << result.lineContent;
        return out;
    }

    using SearchResultType = vector<Result>;
}

#endif
