#include <iostream>
#include <unordered_map>
#include <fstream>
#include <cstdlib>
#include <cstdio>
#include "darwin.hpp"
#include "Tokenizer.hpp"
#include "IndexBuilder.hpp"

using namespace Darwin;

int main() {
    IndexBuilder ib((Tokenizer()));
    ib.build("../data/documents");
    auto results = ib.search("apple");
    cout << results.size() << endl;
    for (const auto& result : results) {
        cout << result << endl;
    }
    return 0;
}
