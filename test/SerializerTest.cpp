#include "gtest/gtest.h"
#include "darwin.hpp"
#include "Serializer.hpp"
#include <string>
#include <vector>
#include <unordered_map>
#include "CompareOperator.hpp"

using namespace Darwin;
using namespace std;

class SerializerValidator;
using Serializer4Test = SerializerT<SerializerValidator>;
class SerializerValidator {};

TEST(SerializerTest, SerializationOfBasicTypes) {
    Serializer4Test serializer;
    ofstream fout("dump/dump_basic_types", ios_base::out | ios_base::binary);
    int i = 10;
    double d = 10.0;
    char c = 'd';
    wchar_t wc = 'd';
    unsigned short int usi= 10;
    serializer.serialize(fout, i);
    serializer.serialize(fout, d);
    serializer.serialize(fout, c);
    serializer.serialize(fout, usi);
    serializer.serialize(fout, wc);
    serializer.serialize(fout, 10);
    fout.close();

    ifstream fin("dump/dump_basic_types", ios_base::in | ios_base::binary);
    int bi;
    double bd;
    char bc;
    wchar_t bwc;
    unsigned short int busi;
    int ten;
    serializer.deserialize(fin, bi);
    serializer.deserialize(fin, bd);
    serializer.deserialize(fin, bc);
    serializer.deserialize(fin, busi);
    serializer.deserialize(fin, bwc);
    serializer.deserialize(fin, ten);
    fin.close();

    ASSERT_EQ(i, bi);
    ASSERT_EQ(d, bd);
    ASSERT_EQ(c, bc);
    ASSERT_EQ(usi, busi);
    ASSERT_EQ(wc, bwc);
    ASSERT_EQ(ten, 10);
}


TEST(SerializerTest, SerializationOfStl) {
    Serializer4Test serializer;
    ofstream fout("dump/dump_stl", ios_base::out | ios_base::binary);

    string s = "hello world!";
    vector<int> vi= {10, 20, 30};
    vector<string> vs = {"hello", "my", "love"};
    unordered_map<int, string> mis = {{0, "xu"}, {1, "ruochen"}, {2, "cool"}};
    unordered_map<string, vector<string>> msvs = {
        {"Beijing", {"010", "North China", "beijing@cstdlib.com"}},
        {"Guangzhou", {"020", "South China", "guangzhou@cstdlib.com"}},
        {"Shanghai", {"021", "Southeast China", "shanghai@cstdlib.com"}}
    };

    serializer.serialize(fout, s);
    serializer.serialize(fout, vi);
    serializer.serialize(fout, vs);
    serializer.serialize(fout, mis);
    serializer.serialize(fout, msvs);
    fout.close();

    ifstream fin("dump/dump_stl", ios_base::in | ios_base::binary);
    string bs;
    vector<int> bvi;
    vector<string> bvs;
    unordered_map<int, string> bmis;
    unordered_map<string, vector<string>> bmsvs;

    serializer.deserialize(fin, bs);
    serializer.deserialize(fin, bvi);
    serializer.deserialize(fin, bvs);
    serializer.deserialize(fin, bmis);
    serializer.deserialize(fin, bmsvs);
    fin.close();

    ASSERT_EQ(s, bs);
    ASSERT_EQ(vi, bvi);
    ASSERT_EQ(vs, bvs);
    ASSERT_EQ(mis, bmis);
    ASSERT_EQ(msvs, bmsvs);
}
