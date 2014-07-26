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

    template <typename T>
    struct SerializeFunc;

    template <typename T>
    struct DeserializeFunc;

    template <>
    struct SerializeFunc<size_t> {
        void operator () (ofstream& fout, size_t data) const {
            fout.write((reinterpret_cast<const char*>(&data)), sizeof(data));
        }
    };

    template <>
    struct SerializeFunc<const size_t> {
        void operator () (ofstream& fout, size_t data) const {
            fout.write((reinterpret_cast<const char*>(&data)), sizeof(data));
        }
    };

    template <typename T>
    struct SerializeFunc<T*> {
        void operator () (ofstream& fout, const T* data, size_t length) const {
            throw SerializationOfUnknownType(string("Unknown type to serialize: ") + typeid(data).name());
        }
    };

    template <>
    struct SerializeFunc<char*> {
        void operator () (ofstream& fout, const char* data, size_t length) const {
            fout.write((reinterpret_cast<const char*>(&data)), sizeof(const char) * length);
        }
    };

    template <>
    struct SerializeFunc<const string> {
        void operator () (ofstream& fout, const string& data) const {
            SerializeFunc<typename string::size_type>()(fout, data.length());
            SerializeFunc<typename string::pointer>()(fout, data.c_str(), data.length());
        }
    };

    template <>
    struct DeserializeFunc<string> {
        void operator () (ifstream& fin, string& data) const {
            typename string::size_type size;
            DeserializeFunc<typename string::size_type>()(fin, size);

            string::pointer tmp = new string::value_type [size+1];
            SerializeFunc<typename string::pointer>()(fout, tmp, size);
            tmp[size] = '\0';

            data.clear();
            data += tmp;

            delete [] tmp;
        }
    };

    template <>
    struct SerializeFunc<string> {
        void operator () (ofstream& fout, const string& data) const {
            SerializeFunc<typename string::size_type>()(fout, data.length());
            SerializeFunc<typename string::pointer>()(fout, data.c_str(), data.length());
        }
    };

    template <typename FirstType, typename SecondType>
    struct DeserializeFunc<pair<FirstType, SecondType>> {
        void operator () (ifstream& fin, pair<FirstType, SecondType>& data) const {
            DeserializeFunc<FirstType>()(fin, data.first);
            DeserializeFunc<SecondType>()(fin, data.second);
        }
    };

    template <typename FirstType, typename SecondType>
    struct SerializeFunc<pair<FirstType, SecondType>> {
        void operator () (ofstream& fout, const pair<FirstType, SecondType>& data) const {
            SerializeFunc<FirstType>()(fout, data.first);
            SerializeFunc<SecondType>()(fout, data.second);
        }
    };

    template <typename T>
    struct SerializeFunc {
        void operator () (ofstream& fout, const T& data) const {
            SerializeFunc<typename T::size_type>()(fout, data.size());
            for (const auto & d : data) {
                SerializeFunc<typename T::value_type>()(fout, d);
            }
        }
    };

    template <T>
    struct DeserializeFunc<vector<T>> {
        void operator () (ifstream& fin, T& data) const {
            typename T::size_type size;
            DeserializeFunc<typename T::size_type>()(fin, size);
            for (typename T::size_type i = 0; i < size; i++) {
                typename T::value_type val;
                DeserializeFunc<typename T::value_type>()(fin, val);
                data.push_back(val);
            }
        }
    };

    template <typename T>
    struct DeserializeFunc {
        void operator () (ifstream& fin, T& data) const {
            typename T::size_type size;
            DeserializeFunc<typename T::size_type>()(fin, size);
            for (typename T::size_type i = 0; i < size; i++) {
                typename T::value_type val;
                DeserializeFunc<typename T::value_type>()(fin, val);
                data.insert(val);
            }
        }
    };

    template <typename Validator>
    class SerializerT {
        friend Validator;
        public:
            template <typename T, typename Func = SerializeFunc<T>>
            void serialize(ofstream& fout, const T& data, const Func& func = Func()) {
                func(fout, data);
            }
    };

    using Serializer = SerializerT<int>;
}

#endif
