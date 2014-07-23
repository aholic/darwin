#ifndef __SERIALIZER_HPP__
#define __SERIALIZER_HPP__

#include <vector>
#include <unordered_map>
#include <string>
#include <fstream>
#include <algorithm>
#include "darwin.hpp"

namespace Darwin {

    template <typename Validator>
    class SerializerT {
        friend Validator;

        private:
            vector<size_t> _basicTypes;
            vector<size_t> _basicTypePtrs;

            template<typename T>
            void _confirmBasicType(const T& data) const {
                if (find(_basicTypes.begin(), _basicTypes.end(), typeid(data).hash_code()) == _basicTypes.end()) {
                    throw "serialization of non-basic type";
                }
            }

            template<typename T>
            void _confirmBasicTypePtr(const T& data) const {
                if (find(_basicTypePtrs.begin(), _basicTypePtrs.end(), typeid(data).hash_code()) == _basicTypePtrs.end()) {
                    throw "serialization of non-basic type ptr";
                }
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

            const SerializerT& deserialize(ifstream& fin, string& data) const {
                size_t len;
                deserialize(fin, len);

                data.clear();
                data.reserve(len);
                char* str = new char [len+1];
                fin.read(str, len);
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

            template<typename T>
            const SerializerT& deserialize(ifstream& fin, T& data) const {
                _confirmBasicType(data);
                fin.read((reinterpret_cast<char*>(&data)), sizeof(data));
                return *this;
            }

            template<typename T>
            void serialize(const string& dumpFileName, const T& data) const {
                ofstream fout(dumpFileName, ios_base::out | ios_base::binary);
                serialize(fout, data);
                fout.close();
            }

            template<typename T>
            const SerializerT& serialize(ofstream& fout, const T& data) const {
                _confirmBasicType(data);
                fout.write((reinterpret_cast<const char*>(&data)), sizeof(data));
                return *this;
            }

            template<typename T>
            const SerializerT& serialize(ofstream& fout, const vector<T>& data) const {
                serialize(fout, data.size());
                for (const auto& elem : data) {
                    serialize(fout, elem);
                }
                return *this;
            }

            template<typename KeyType, typename MappedType> 
            const SerializerT& serialize(ofstream& fout, const unordered_map<KeyType, MappedType>& data) const {
                serialize(fout, data.size());
                for (const auto& elem : data) {
                    serialize(fout, elem.first);
                    serialize(fout, elem.second);
                }
                return *this;
            }

            const SerializerT& serialize(ofstream& fout, const string& data) const {
                return serialize(fout, data.c_str(), data.length());
            }

            template<typename T>
            const SerializerT& serialize(ofstream& fout, const T* data, size_t len) const {
                _confirmBasicTypePtr(data);
                serialize(fout, len);
                fout.write((reinterpret_cast<const char*>(data)), len);
                return *this;
            }

    };

    using Serializer = SerializerT<int>;
}

#endif
