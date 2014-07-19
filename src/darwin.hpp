#ifndef __DARWIN_HPP__
#define __DARWIN_HPP__

#include <vector>
#include <iostream>
namespace Darwin {
    using namespace std;

    using WordIdType = size_t;
    using DocIdType = size_t;

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
    };

    ostream& operator << (ostream& stream, Result result) {
        stream << result.docId << "(" << result.docName << "): " << result.lineno << ": " << result.lineContent;
        return stream;
    }
    using SearchResultType = vector<Result>;
}

#endif
