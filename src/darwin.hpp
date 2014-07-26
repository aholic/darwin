#ifndef __DARWIN_HPP__
#define __DARWIN_HPP__

#include <vector>
#include <iostream>
namespace Darwin {
    using namespace std;

    using WordIdType = size_t;
    using DocIdType = size_t;

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
