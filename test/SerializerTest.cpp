#include "gtest/gtest.h"
#include "darwin.hpp"
#include "Serializer.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "CompareOperator.hpp"

using namespace Darwin;
using namespace std;

class SerializerValidator;
using Serializer4Test = SerializerT<SerializerValidator>;
class SerializerValidator {};

TEST(SerializerTest, SerializationOfBasicTypes) {
    Serializer4Test serializer;
    const string fname= "dump/basic_type_test";

    ofstream fout(fname, ios_base::out | ios_base::binary);
    vector<char> charVec = {'a', 'b', 'c', 'e', 'f'};
    for (const auto & c : charVec) {
        serializer.serialize(fout, c);
    }
    fout.close();

    ifstream fin(fname, ios_base::in | ios_base::binary);
    char bc;
    for (const auto & c : charVec) {
        serializer.deserialize(fin, bc);
        ASSERT_EQ(c, bc);
    }
    fin.close();

    int i = 10;
    serializer.serialize(fname, i);
    int bi;
    serializer.deserialize(fname, bi);
    ASSERT_EQ(i, bi);
}

TEST(SerializerTest, SerializationOfSpecifiedType) {
    Serializer4Test serializer;
    const string fname= "dump/specified_type_test";

    vector<int> vi = {10, 20, 30};
    serializer.serialize(fname, vi);
    vector<int> bvi;
    serializer.deserialize(fname, bvi);
    ASSERT_EQ(vi, bvi);

    vector<string> vs = {"i", "love", "lwj"};
    serializer.serialize(fname, vs);
    vector<string> bvs;
    serializer.deserialize(fname, bvs);
    ASSERT_EQ(vs, bvs);

    unordered_map<int, int> mii = {{1, 2}, {2, 3}};
    serializer.serialize(fname, mii);
    unordered_map<int, int> bmii;
    serializer.deserialize(fname, bmii);
    ASSERT_EQ(mii, bmii);

    unordered_map<string, int> msi = {{"xrc", 22}, {"lwj", 23}};
    serializer.serialize(fname, msi);
    unordered_map<string, int> bmsi;
    serializer.deserialize(fname, bmsi);
    ASSERT_EQ(msi, bmsi);

    unordered_map<string, string> mss = {{"xrc", "boy"}, {"lwj", "girl"}};
    serializer.serialize(fname, mss);
    unordered_map<string, string> bmss;
    serializer.deserialize(fname, bmss);
    ASSERT_EQ(mss, bmss);

    unordered_set<int> si = {10, 20, 30};
    serializer.serialize(fname, si);
    unordered_set<int> bsi;
    serializer.deserialize(fname, bsi);
    ASSERT_EQ(si, bsi);

    InvertedIndexValueType iivt(10, 20, 30);
    serializer.serialize(fname, iivt);
    InvertedIndexValueType biivt;
    serializer.deserialize(fname, biivt);
    ASSERT_EQ(iivt, biivt);

    unordered_set<InvertedIndexValueType, HashFunc> siivt = {{10, 20, 30}, {20, 30, 40}};
    serializer.serialize(fname, siivt);
    unordered_set<InvertedIndexValueType, HashFunc> bsiivt;
    serializer.deserialize(fname, bsiivt);
    ASSERT_EQ(siivt, bsiivt);
}
