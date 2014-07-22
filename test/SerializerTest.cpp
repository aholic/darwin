#include "gtest/gtest.h"
#include "darwin.hpp"
#include "Serializer.hpp"
#include <string>
#include <vector>

using namespace Darwin;
using namespace std;

class SerializerValidator;
using Serializer4Test = SerializerT<SerializerValidator>;
class SerializerValidator {
    public:
};

TEST(SerializerTest, SplitBySingleChar) {
    Serializer4Test serializer;
}

