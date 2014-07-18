#ifndef __INDEXBUILDER_HPP__
#define __INDEXBUILDER_HPP__

#include <string>
#include <fstream>
#include "Tokenizer.hpp"
#include "darwin.hpp"

namespace Darwin {
    class IndexBuilder {
        using InvertedIndexType = unordered_map<WordIdType, vector<DocIdType>>;
        public:
            IndexBuilder(Tokenizer& tokenizer) : _tokenizer(tokenizer) {}
            IndexBuilder(Tokenizer&& tokenizer) : _tokenizer(tokenizer) {}

            void build(const string& documents) {
                _documents = _fillDocList(documents);
                _index = _buildInvertedIndex(_documents); 
            }

        private:
            vector<string> _documents; 
            Tokenizer& _tokenizer;
            InvertedIndexType _index;
        private:
            InvertedIndexType _buildInvertedIndex(const vector<string>& documents) const {
                InvertedIndexType index;
                int documentNum = documents.size();
                for (size_t i = 0; i< documentNum; i++) {
                    _buildInvertedIndexAux(i, documents[i], index);
                }
                return index;
            }

            void _buildInvertedIndexAux(DocIdType docId, const string& docName, InvertedIndexType& index) const {
                ifstream doc(docName);
                string line;
                while (getline(doc, line)) {
                    auto buf = _tokenizer.tokenize(line);
                    for (auto wordId : buf) {
                        index[wordId].push_back(docId);
                    }
                }
                doc.close();
            }

            vector<string> _fillDocList(const string& documents) const {
                vector<string> docList; 
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
