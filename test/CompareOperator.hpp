#include <string>
#include <vector>
#include <unordered_map>
#include <typeinfo>

using namespace std;

template<typename T1, typename T2>
inline bool operator == (const vector<T1>& v1, const vector<T2>& v2) {
    if (typeid(v1) != typeid(v2)) return false;
    if (v1.size() != v2.size()) return false;
    for (int i = 0; i < v1.size(); i++) {
        if (v1[i] != v2[i]) return false;
    }
    return true;
}

template<typename KeyType1, typename KeyType2, typename MappedType1, typename MappedType2>
inline bool operator == (const unordered_map<KeyType1, MappedType1>& m1, const unordered_map<KeyType2, MappedType2>& m2) {
    if (typeid(m1) != typeid(m2)) return false;
    if (m1.size() != m2.size()) return false;
    for (const auto& elem : m1) {
        auto tmp = m2.find(m1.first);
        if (tmp == m2.end()) return false;
        if (tmp->second != elem.second) return false;
    }
    return true;
}
