CXX = clang++
CXX_FLAGS += -g -Wall -std=c++11
TEST_NAME = test.run
GTEST_DIR = ./gtest-1.7.0
GTEST_OBJ = $(GTEST_DIR)/lib/*.o
TEST_CASES = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
SRC_DIR = ../src
INCLUDE_GTEST = -I$(GTEST_DIR)/include
INCLUDE_SRC = -I$(SRC_DIR)

all : $(TEST_NAME)

clean :
	rm *.o $(TEST_NAME) -f

clean-all :
	rm *.o $(TEST_NAME) $(GTEST_DIR)/lib/* -f
	rm $(GTEST_DIR)/lib -rf

gtest : gtest_lib_dir $(GTEST_DIR)/lib/gtest-all.o $(GTEST_DIR)/lib/gtest_main.o

gtest_lib_dir :
	@ if [ ! -d $(GTEST_DIR)/lib ]; then mkdir $(GTEST_DIR)/lib ; fi

$(GTEST_DIR)/lib/gtest-all.o :
	$(CXX) -I${GTEST_DIR} -I$(GTEST_DIR)/include \
		   -pthread $(GTEST_DIR)/src/gtest-all.cc \
		   -c -o $@

$(GTEST_DIR)/lib/gtest_main.o :
	$(CXX) -I${GTEST_DIR} -I$(GTEST_DIR)/include \
		   -pthread $(GTEST_DIR)/src/gtest_main.cc \
		   -c -o $@

.cpp.o :
	$(CXX) $< $(INCLUDE_GTEST) $(INCLUDE_SRC) -c $(CXX_FLAGS)

$(TEST_NAME) : $(TEST_CASES) gtest
	$(CXX) $(INCLUDE_GTEST) $(TEST_CASES) $(GTEST_OBJ) -lpthread $(CXX_FLAGS) -o $@
