#ifndef __INDEXBUILDER_HPP__
#define __INDEXBUILDER_HPP__

#include <string>
#include <fstream>
#include "Tokenizer.hpp"
#include "darwin.hpp"

namespace Darwin {
    class IndexBuilder {
        public:
            IndexBuilder(const Tokenizer& tokenizer) : _tokenizer(tokenizer) {}

            void build(const string& documents) {
                _documents = _fillDocList(documents);
            }

        private:
            vector<string> _documents; 
            const Tokenizer& _tokenizer;
        private:
            vector<string> _fillDocList(const string& documents) const {
                vector<string> docList; 
                ifstream content(documents);
                string line;
                while (getline(content, line)) {
                    auto buf = _tokenizer.split(line);
                    if (2 != buf.size()) continue;
                    docList.push_back(buf[1]);
                }
                return docList;
            }
    };
}

#endif
