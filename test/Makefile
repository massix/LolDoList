# Makefile for tests

CXX = clang++
CXXFLAGS = -Wall -I/usr/local -I../src -I../src/messages
LDFLAGS = -L/usr/local/lib -lprotobuf -lz -L../src -lloldolist

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
TARGET = archiver_test

.PHONY: clean test

all: $(TARGET)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $?
	
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) -o $(TARGET)
		
run-test: $(TARGET)
	@ln -s ../src/libloldolist.dylib .
	@LD_LIBRARY_PATH=. ./$(TARGET)
	@rm -f libloldolist.dylib
	
clean:
	rm -f $(TARGET) $(OBJS)
	rm -f libloldolist.dylib