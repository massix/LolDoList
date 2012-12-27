# Makefile for src

CXX = clang++
CXXFLAGS = -Wall -I/usr/local -I. -Imessages
LDFLAGS = -L/usr/local/lib -lprotobuf -lz

PROTOC_BIN = $(shell which protoc)
PROTOC_SRCS = $(wildcard messages/*.proto)
PROTOC_GENS_SRC = $(PROTOC_SRCS:.proto=.pb.cc) 
PROTOC_GENS_HDR = $(PROTOC_SRCS:.proto=.pb.h)
PROTOC_OBJS = $(PROTOC_GENS_SRC:.pb.cc=.pb.o)

SRCS = $(wildcard *.cpp)
OBJS = $(SRCS:.cpp=.o)
TARGET = libloldolist.dylib


.PHONY: clean

all: $(TARGET) $(ARCHIVER_TEST_TARGET)

$(PROTOC_GENS_SRC): messages/messages.proto
	$(PROTOC_BIN) --cpp_out=. messages/messages.proto

%.pb.o: %.pb.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $?

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $?
	
$(TARGET): messages $(PROTOC_OBJS) $(OBJS)
	$(CXX) -shared $(PROTOC_OBJS) $(OBJS) $(LDFLAGS) -o $(TARGET)

clean:
	rm -f $(TARGET) $(PROTOC_GENS_SRC) $(PROTOC_GENS_HDR) $(OBJS) $(PROTOC_OBJS)