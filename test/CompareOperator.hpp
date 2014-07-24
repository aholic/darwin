#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <typeinfo>

using namespace std;

template<typename T1, typename T2>
inline bool operator == (const vector<T1>& v1, const vector<T2>& v2) {
    if (typeid(v1) != typeid(v2)) return false;
    if (v1 != v2) return false;
    return true;
}

template<typename KeyType1, typename KeyType2, typename MappedType1, typename MappedType2>
inline bool operator == (const unordered_map<KeyType1, MappedType1>& m1, const unordered_map<KeyType2, MappedType2>& m2) {
    if (typeid(m1) != typeid(m2)) return false;
    if (m1 != m2) return false;
    return true;
}

template<typename T1, typename T2>
inline bool operator == (const unordered_set<T1>& s1, const unordered_set<T2>& s2) {
    if (typeid(s1) != typeid(s2)) return false;
    if (s1 != s2) return false;
    return true;
}
