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
            virtual const char* what() const noexcept {
                return _name.c_str();
            }
    };

    template <typename T>
    struct SerializeFunc {
        void operator () (ofstream& fout, const T& data) const {
            SerializeFunc<size_t>(fout, data.size());
        }
    };

    template <>
    struct SerializeFunc<size_t> {
        void operator () (ofstream& fout, size_t data) const {
            fout.write((reinterpret_cast<const char*>(&data)), sizeof(data));
        }
    };

    template <typename T>
    struct SerializeFunc<T*> {
        void operator () (ofstream& fout, const T* data) const {
            throw SerializationOfUnknownType(string("Unknown type to serialize: " + typeid(data).name()));
        }
    };

    template <>
    struct SerializeFunc<char*> {
        void operator () (ofstream& fout, const char* data) const {
            fout.write((reinterpret_cast<const char*>(&data)), sizeof(data));
        }
    };

    template <>
    struct SerializeFunc<string> {
    }

    template <typename Validator>
    class SerializerT {
        friend Validator;

        private:
            vector<size_t> _basicTypes;
            vector<size_t> _basicTypePtrs;

            template<typename T>
            void _confirmBasicType(const T& data) const {
                if (!_typeBelong(data, _basicTypes)) {
                    throw SerializationOfUnknownType(string("serialization of non-basic type: ") + typeid(data).name());
                }
            }

            template<typename T>
            void _confirmBasicTypePtr(const T& data) const {
                if (!_typeBelong(data, _basicTypePtrs))
                    throw SerializationOfUnknownType(string("serialization of non-basic type ptr: ") + typeid(data).name());
                }
            }

            template<typename T>
            bool _typeBelong(const T& data, vector<size_t>& types) {
                if (find(types.begin(), types.end(), typeid(data).hash_code()) == types.end()) return false;
                return true;
            }

        public:
            SerializerT() : 
                _basicTypes ({
                    typeid(char).hash_code(), typeid(char16_t).hash_code(),
                    typeid(char32_t).hash_code(), typeid(wchar_t).hash_code(),
                    typeid(signed char).hash_code(), typeid(signed short int).hash_code(),
                    typeid(signed int).hash_code(), typeid(signed long int).hash_code(),
                    typeid(signed long long int).hash_code(), typeid(unsigned char).hash_code(),
                    typeid(unsigned short int).hash_code(), typeid(unsigned int).hash_code(),
                    typeid(unsigned long int).hash_code(), typeid(unsigned long long int).hash_code(),
                    typeid(float).hash_code(), typeid(double).hash_code(),
                    typeid(long double) .hash_code(), typeid(const char).hash_code(), 
                    typeid(const char32_t).hash_code(), typeid(const wchar_t).hash_code(),
                    typeid(const signed char).hash_code(), typeid(const signed short int).hash_code(),
                    typeid(const signed int).hash_code(), typeid(const signed long int).hash_code(),
                    typeid(const signed long long int).hash_code(), typeid(const unsigned char).hash_code(),
                    typeid(const unsigned short int).hash_code(), typeid(const unsigned int).hash_code(),
                    typeid(const unsigned long int).hash_code(), typeid(const unsigned long long int).hash_code(),
                    typeid(const float).hash_code(), typeid(const double).hash_code(),
                    typeid(const long double) .hash_code(), typeid(const char16_t).hash_code()
                }), _basicTypePtrs ({
                    typeid(char *).hash_code(), typeid(char16_t *).hash_code(),
                    typeid(char32_t *).hash_code(), typeid(wchar_t *).hash_code(),
                    typeid(signed char *).hash_code(), typeid(signed short int *).hash_code(),
                    typeid(signed int *).hash_code(), typeid(signed long int *).hash_code(),
                    typeid(signed long long int *).hash_code(), typeid(unsigned char *).hash_code(),
                    typeid(unsigned short int *).hash_code(), typeid(unsigned int *).hash_code(),
                    typeid(unsigned long int *).hash_code(), typeid(unsigned long long int *).hash_code(),
                    typeid(float *).hash_code(), typeid(double *).hash_code(),
                    typeid(long double) .hash_code(), typeid(const char *).hash_code(), 
                    typeid(const char32_t *).hash_code(), typeid(const wchar_t *).hash_code(),
                    typeid(const signed char *).hash_code(), typeid(const signed short int *).hash_code(),
                    typeid(const signed int *).hash_code(), typeid(const signed long int *).hash_code(),
                    typeid(const signed long long int *).hash_code(), typeid(const unsigned char *).hash_code(),
                    typeid(const unsigned short int *).hash_code(), typeid(const unsigned int *).hash_code(),
                    typeid(const unsigned long int *).hash_code(), typeid(const unsigned long long int *).hash_code(),
                    typeid(const float *).hash_code(), typeid(const double *).hash_code(),
                    typeid(const long double) .hash_code(), typeid(const char16_t *).hash_code()
                }){}
            
            template<typename T>
            void deserialize(const string& backupFileName, T& data) const {
                ifstream fin(backupFileName, ios_base::in | ios_base::binary);
                deserialize(fin, data);
                fin.close();
            }

            template<typename T>
            const SerializerT& deserialize(ifstream& fin, T& data) const {
                _confirmBasicType(data);
                fin.read((reinterpret_cast<char*>(&data)), sizeof(data));
                return *this;
            }

            template<typename T>
            const SerializerT& deserialize(ifstream& fin, T* data, size_t len) const {
                _confirmBasicTypePtr(data);
                fin.read((reinterpret_cast<char*>(data)), len * sizeof(T));
                return *this;
            }

            const SerializerT& deserialize(ifstream& fin, string& data) const {
                size_t len;
                deserialize(fin, len);

                data.clear();
                data.reserve(len);

                char *str = new char [len+1];
                deserialize(fin, str, len);
                str[len] = '\0';

                data.append(str);
                delete [] str;
                return *this;
            }

            template<typename T>
            const SerializerT& deserialize(ifstream& fin, vector<T>& data) const {
                size_t size;
                deserialize(fin, size);

                data.clear();
                data.reserve(size);
                T elem;
                for (int i = 0; i < size; i++) {
                    deserialize(fin, elem);
                    data.push_back(elem);
                }

                return *this;
            }

            template<typename T>
            const SerializerT& deserialize(ifstream& fin, unordered_set<T>& data) const {
                size_t size;
                deserialize(fin, size);

                data.clear();
                data.reserve(size);
                T elem;
                for (int i = 0; i < size; i++) {
                    deserialize(fin, elem);
                    data.insert(elem);
                }

                return *this;
            }

            template<typename KeyType, typename MappedType>
            const SerializerT& deserialize(ifstream& fin, unordered_map<KeyType, MappedType>& data) const {
                size_t size;
                deserialize(fin, size);

                data.clear();
                data.reserve(size);
                KeyType key;
                MappedType mapped;
                for (int i = 0; i < size; i++) {
                    deserialize(fin, key);
                    deserialize(fin, mapped);
                    data.insert(make_pair(key, mapped));
                }
                return *this;
            }


/////////////////////////////////////////////////////////////////////////////////////////////////
        public:
            template<typename T>
            void serialize(ofstream&& fout, const T&& data) const {
                if (_typeBelong(data, _basicTypes)) {
                    serializeBasicType(forward<ofstream>(fout), forward<T>(data));
                } else {
                    serialize(fout, data.size());
                    serialize(fout, data.begin(), data.end());
                }
            }

            template <typename T>
            void serializeBasicType(ofstream& fout, const T& data) {
                fout.write((reinterpret_cast<const char*>(&data)), sizeof(data));
            }

            void serialize(ofstream& fout, const string& data) const {
                serialize(fout, data.length());
                serialize(fout, data.c_str(), data.length());
            }

            template<typename T>
            void serialize(ofstream& fout, const T* data, size_t len) const {
                _confirmBasicTypePtr(data);
                fout.write((reinterpret_cast<const char*>(data)), len * sizeof(T));
            }

            template<typename InputIterator, typename T>
            void serialize(ofstream& fout, InputIterator first, InputIterator last) const {
                while (first != last) {
                    serialize(fout, *first);
                    ++first;
                }
            }

    };

    using Serializer = SerializerT<int>;
}

#endif
