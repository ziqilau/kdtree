CXX = g++

INCPATH = -I./src -I./include -I./example -I/usr/local/include

CFLAGS = -std=c++11 -fopenmp -fPIC -O3 -ggdb -Wall -finline-functions $(INCPATH)

LIBPATH = -L/usr/local/lib
LIBPATH += -lprotobuf

OBJS = $(addprefix build/,  \
criterion.o criterion/cycle_criterion.o \
recordio/kdtree.pb.o )

LIB = build/libkdtree.a

TEST = build/test_kdtree

.PHONY: clean all proto test

all: build/build_kdtree build/query_kdtree

test: $(TEST)

proto:
	protoc -I=./src/recordio/ --cpp_out=./src/recordio/ ./src/recordio/kdtree.proto

build/test_kdtree: build/test_kdtree.o $(LIB)
	$(CXX) $(CFLAGS) -o $@ $^ $(LIBPATH) -lgtest -lpthread

build/%_kdtree: build/%_kdtree.o $(LIB)
	$(CXX) $(CFLAGS) -o $@ $^ $(LIBPATH)

build/libkdtree.a: $(OBJS)
	ar crv $@ $(filter %.o, $?)

build/%.o: test/%.cc
	@mkdir -p $(@D)
	$(CXX) $(INCPATH) -std=c++0x -MM -MT build/$*.o $< >build/$*.d
	$(CXX) $(CFLAGS) -c $< -o $@

build/%.o: example/%.cc
	@mkdir -p $(@D)
	$(CXX) $(INCPATH) -std=c++0x -MM -MT build/$*.o $< >build/$*.d
	$(CXX) $(CFLAGS) -c $< -o $@

build/%.o: src/%.cc
	@mkdir -p $(@D)
	$(CXX) $(INCPATH) -std=c++0x -MM -MT build/$*.o $< >build/$*.d
	$(CXX) $(CFLAGS) -c $< -o $@

clean:
	rm -rf build

-include build/*.d
-include build/*/*.d
