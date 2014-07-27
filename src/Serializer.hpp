#ifndef __SERIALIZER_HPP__
#define __SERIALIZER_HPP__

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <string>
#include <fstream>
#include <algorithm>
#include <exception>
#include "darwin.hpp"

using namespace std;
namespace Darwin {

    class SerializationOfUnknownType : public exception {
        private:
            string _name;
        public:
            SerializationOfUnknownType(const string& name) : _name(name) {}
            virtual const char* what() const noexcept override {
                return _name.c_str();
            }
    };

    // basic template
    template <typename T>
    struct SerializeFunc {
        void operator () (ofstream& fout, const T& data) const {
            fout.write((reinterpret_cast<const char*>(&data)), sizeof(data));
        }
    };

    // basic const T template
    template <typename T>
    struct SerializeFunc<const T> {
        void operator () (ofstream& fout, const T& data) const {
            fout.write((reinterpret_cast<const char*>(&data)), sizeof(data));
        }
    };


    // basic template for DeserializeFunc
    template <typename T>
    struct DeserializeFunc {
        void operator () (ifstream& fin, T& data) const {
            fin.read((reinterpret_cast<char*>(&data)), sizeof(data));
        }
    };


    // const T* SerializeFunc
    template <typename T>
    struct SerializeFunc<const T*> {
        void operator () (ofstream& fout, const T* data, size_t length) const {
            fout.write((reinterpret_cast<const char*>(data)), sizeof(const char) * length);
        }
    };

    // T* SerializeFunc
    template <typename T>
    struct SerializeFunc<T*> {
        void operator () (ofstream& fout, const T* data, size_t length) const {
            fout.write((reinterpret_cast<const char*>(data)), sizeof(const char) * length);
        }
    };

    // const string SerializeFunc
    template <>
    struct SerializeFunc<const string> {
        void operator () (ofstream& fout, const string& data) const {
            SerializeFunc<typename string::size_type>()(fout, data.length());
            SerializeFunc<typename string::pointer>()(fout, data.c_str(), data.length());
        }
    };

    // string SerializeFunc
    template <>
    struct SerializeFunc<string> {
        void operator () (ofstream& fout, const string& data) const {
            SerializeFunc<typename string::size_type>()(fout, data.length());
            SerializeFunc<typename string::pointer>()(fout, data.c_str(), data.length());
        }
    };

    // pair SerializeFunc
    template <typename FirstType, typename SecondType>
    struct SerializeFunc<pair<FirstType, SecondType>> {
        void operator () (ofstream& fout, const pair<FirstType, SecondType>& data) const {
            SerializeFunc<FirstType>()(fout, data.first);
            SerializeFunc<SecondType>()(fout, data.second);
        }
    };

    template <typename T, typename Alloc>
    struct SerializeFunc<vector<T, Alloc>>;

    template <typename T, typename Alloc>
    struct SerializeFunc<const vector<T, Alloc>>;

    template <typename Key, typename T, typename Hash, typename Pred, typename Alloc>
    struct SerializeFunc<unordered_map<Key, T, Hash, Pred, Alloc>>;

    template <typename Key, typename T, typename Hash, typename Pred, typename Alloc>
    struct SerializeFunc<const unordered_map<Key, T, Hash, Pred, Alloc>>;

    // unordered_set SerializeFunc
    template <typename Key, typename Hash, typename Pred, typename Alloc>
    struct SerializeFunc<unordered_set<Key, Hash, Pred, Alloc>> {
        void operator () (ofstream& fout, const unordered_set<Key, Hash, Pred, Alloc>& data) const {
            using size_type = typename unordered_set<Key, Hash, Pred, Alloc>::size_type;
            using value_type = typename unordered_set<Key, Hash, Pred, Alloc>::value_type;

            SerializeFunc<size_type>()(fout, data.size());
            for (const auto & d: data) {
                SerializeFunc<value_type>()(fout, d);
            }
        }
    };

    // const unordered_set SerializeFunc
    template <typename Key, typename Hash, typename Pred, typename Alloc>
    struct SerializeFunc<const unordered_set<Key, Hash, Pred, Alloc>> {
        void operator () (ofstream& fout, const unordered_set<Key, Hash, Pred, Alloc>& data) const {
            using size_type = typename unordered_set<Key, Hash, Pred, Alloc>::size_type;
            using value_type = typename unordered_set<Key, Hash, Pred, Alloc>::value_type;

            SerializeFunc<size_type>()(fout, data.size());
            for (const auto & d: data) {
                SerializeFunc<value_type>()(fout, d);
            }
        }
    };

    // unordered_map SerializeFunc
    template <typename Key, typename T, typename Hash, typename Pred, typename Alloc>
    struct SerializeFunc<unordered_map<Key, T, Hash, Pred, Alloc>> {
        void operator () (ofstream& fout, const unordered_map<Key, T, Hash, Pred, Alloc>& data) const {
            using size_type = typename unordered_map<Key, T, Hash, Pred, Alloc>::size_type;
            using value_type = typename unordered_map<Key, T, Hash, Pred, Alloc>::value_type;

            SerializeFunc<size_type>()(fout, data.size());
            for (const auto & d : data) {
                SerializeFunc<value_type>()(fout, d);
            }
        }
    };

    // const unordered_map SerializeFunc
    template <typename Key, typename T, typename Hash, typename Pred, typename Alloc>
    struct SerializeFunc<const unordered_map<Key, T, Hash, Pred, Alloc>> {
        void operator () (ofstream& fout, const unordered_map<Key, T, Hash, Pred, Alloc>& data) const {
            using size_type = typename unordered_map<Key, T, Hash, Pred, Alloc>::size_type;
            using value_type = typename unordered_map<Key, T, Hash, Pred, Alloc>::value_type;

            SerializeFunc<size_type>()(fout, data.size());
            for (const auto & d : data) {
                SerializeFunc<value_type>()(fout, d);
            }
        }
    };

    // vector SerializeFunc
    template <typename T, typename Alloc>
    struct SerializeFunc<vector<T, Alloc>> {
        void operator () (ofstream& fout, const vector<T, Alloc>& data) const {
            using size_type = typename vector<T, Alloc>::size_type;
            using value_type = typename vector<T, Alloc>::value_type;

            SerializeFunc<size_type>()(fout, data.size());
            for (const auto & d : data) {
                SerializeFunc<value_type>()(fout, d);
            }
        }
    };

    // const vector SerializeFunc
    template <typename T, typename Alloc>
    struct SerializeFunc<const vector<T, Alloc>> {
        void operator () (ofstream& fout, const vector<T, Alloc>& data) const {
            using size_type = typename vector<T, Alloc>::size_type;
            using value_type = typename vector<T, Alloc>::value_type;

            SerializeFunc<size_type>()(fout, data.size());
            for (const auto & d : data) {
                SerializeFunc<value_type>()(fout, d);
            }
        }
    };

    // T* DeserializeFunc
    template <typename T>
    struct DeserializeFunc<T*> {
        void operator () (ifstream& fin, T* data, size_t length) const {
            fin.read((reinterpret_cast<char*>(data)), length * sizeof(T));
        }
    };

    // string DeserializeFunc
    template <>
    struct DeserializeFunc<string> {
        void operator () (ifstream& fin, string& data) const {
            using size_type = typename string::size_type;
            using value_type = typename string::value_type;
            using pointer = typename string::pointer;

            size_type size;
            DeserializeFunc<size_type>()(fin, size);

            pointer tmp = new value_type [size+1];
            DeserializeFunc<pointer>()(fin, tmp, size);
            tmp[size] = '\0';

            data.clear();
            data.reserve(size);
            data.append(tmp);

            delete [] tmp;
        }
    };

    // pair DeserializeFunc
    template <typename FirstType, typename SecondType>
    struct DeserializeFunc<pair<FirstType, SecondType>> {
        void operator () (ifstream& fin, pair<FirstType, SecondType>& data) const {
            DeserializeFunc<FirstType>()(fin, data.first);
            DeserializeFunc<SecondType>()(fin, data.second);
        }
    };

    // unordered_set DeserializeFunc 
    template <typename Key, typename Hash, typename Pred, typename Alloc>
    struct DeserializeFunc<unordered_set<Key, Hash, Pred, Alloc>> {
        void operator () (ifstream& fin, unordered_set<Key, Hash, Pred, Alloc>& data) const {
            using size_type = typename unordered_set<Key, Hash, Pred, Alloc>::size_type;

            size_type size;
            DeserializeFunc<size_type>()(fin, size);

            data.clear();
            data.reserve(size);

            Key val;
            for (size_type i = 0; i < size; i++) {
                DeserializeFunc<Key>()(fin, val);
                data.insert(val);
            }
        }
    };

    // unordered_map DeserializeFunc
    template <typename Key, typename T, typename Hash, typename Pred, typename Alloc>
    struct DeserializeFunc<unordered_map<Key, T, Hash, Pred, Alloc>> {
        void operator () (ifstream& fin, unordered_map<Key, T, Hash, Pred, Alloc>& data) const {
            using size_type = typename unordered_map<Key, T, Hash, Pred, Alloc>::size_type;

            size_type size;
            DeserializeFunc<size_type>()(fin, size);

            data.clear();
            data.reserve(size);

            pair<Key, T> val;
            for (size_type i = 0; i < size; i++) {
                DeserializeFunc<pair<Key, T>>()(fin, val);
                data.insert(val);
            }
        }
    };

    // vector DeserializeFunc
    template <typename T, typename Alloc>
    struct DeserializeFunc<vector<T, Alloc>> {
        void operator () (ifstream& fin, vector<T, Alloc>& data) const {
            using size_type = typename vector<T, Alloc>::size_type;
            using value_type = typename vector<T, Alloc>::value_type;

            size_type size;
            DeserializeFunc<size_type>()(fin, size);

            data.clear();
            data.reserve(size);

            for (size_type i = 0; i < size; i++) {
                value_type val;
                DeserializeFunc<value_type>()(fin, val);
                data.push_back(val);
            }
        }
    };

    template <typename Validator>
    class SerializerT {
        friend Validator;
        public:
            template <typename T, typename Func = SerializeFunc<T>>
            void serialize(const string& dumpFileName, const T& data, const Func& func = Func()) {
                ofstream fout(dumpFileName, ios_base::out | ios_base::binary);
                serialize(fout, data, func);
                fout.close();
            }

            template <typename T, typename Func = DeserializeFunc<T>>
            void deserialize(const string& backupFileName, T& data, const Func& func = Func()) {
                ifstream fin(backupFileName, ios_base::in | ios_base::binary);
                deserialize(fin, data, func);
                fin.close();
            }

            template <typename T, typename Func = SerializeFunc<T>>
            void serialize(ofstream& fout, const T& data, const Func& func = Func()) {
                func(fout, data);
            }

            template <typename T, typename Func = DeserializeFunc<T>>
            void deserialize(ifstream& fin, T& data, const Func& func = Func()) {
                func(fin, data);
            }
    };

    using Serializer = SerializerT<int>;
}

#endif
