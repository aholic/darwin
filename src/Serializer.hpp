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

        public:
            SerializerT() {
                vector<size_t> _basicTypes = {
                    typeid(char).hash_code(), typeid(char16_t).hash_code(),
                    typeid(char32_t).hash_code(), typeid(wchar_t).hash_code(),
                    typeid(signed char).hash_code(), typeid(signed short int).hash_code(),
                    typeid(signed int).hash_code(), typeid(signed long int).hash_code(),
                    typeid(signed long long int).hash_code(), typeid(unsigned char).hash_code(),
                    typeid(unsigned short int).hash_code(), typeid(unsigned int).hash_code(),
                    typeid(unsigned long int).hash_code(), typeid(unsigned long long int).hash_code(),
                    typeid(float).hash_code(), typeid(double).hash_code(),
                    typeid(long double).hash_code()
                };
            }
            template<typename T>
            bool serialize(const string& dumpFileName, T data) {
                if (find(_basicTypes.begin(), _basicTypes.end(), typeid(data).hash_code()) == _basicTypes.end()) return false;

                ofstream fout(dumpFileName);
                serialize(fout, data);
                fout.close();
                return true;
            }

            template<typename T>
            bool serialize(ofstream& fout, T data) {
                if (find(_basicTypes.begin(), _basicTypes.end(), typeid(data).hash_code()) == _basicTypes.end()) return false;
                fout.write((reinterpret_cast<const char*>(&data)), size(data));
                return true;
            }
    };

    using Serializer = SerializerT<int>;
}
#endif
