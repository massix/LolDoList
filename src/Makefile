# Makefile for src

# Versions
LIB_VERS_MAJ = 1
LIB_VERS_REL = 0
LIB_VERS_MIN = 0

BIN_VERS_MAJ = 1
BIN_VERS_REL = 0
BIN_VERS_MIN = 0

DEFINES = \
	-DLIB_VERS_MAJ=${LIB_VERS_MAJ} \
	-DLIB_VERS_REL=${LIB_VERS_REL} \
	-DLIB_VERS_MIN=${LIB_VERS_MIN} \
	-DBIN_VERS_MAJ=${BIN_VERS_MAJ} \
	-DBIN_VERS_REL=${BIN_VERS_REL} \
	-DBIN_VERS_MIN=${BIN_VERS_MIN}

CXX += clang++
CXXFLAGS = -Wall -I/usr/local -I. -Imessages ${DEFINES}
LDFLAGS = -L/usr/local/lib -lprotobuf -lz

PROTOC_BIN = $(shell which protoc)
PROTOC_SRCS = $(wildcard messages/*.proto)
PROTOC_GENS_SRC = $(PROTOC_SRCS:.proto=.pb.cc) 
PROTOC_GENS_HDR = $(PROTOC_SRCS:.proto=.pb.h)
PROTOC_OBJS = $(PROTOC_GENS_SRC:.pb.cc=.pb.o)

SRCS = archiver.cpp
OBJS = $(SRCS:.cpp=.o)
TARGET = libloldolist.dylib

BIN_SRCS = loldolist.cpp
BIN_OBJS = $(BIN_SRCS:.cpp=.o)
BIN_TARGET = loldolist


.PHONY: clean

all: $(BIN_TARGET)

$(PROTOC_GENS_SRC): messages/messages.proto
	$(PROTOC_BIN) --cpp_out=. messages/messages.proto

%.pb.o: %.pb.cc
	$(CXX) $(CXXFLAGS) -c -o $@ $?

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c -o $@ $?
	
$(TARGET): messages $(PROTOC_OBJS) $(OBJS)
	$(CXX) -shared $(PROTOC_OBJS) $(OBJS) $(LDFLAGS) -o $(TARGET)

$(BIN_TARGET): $(TARGET) $(BIN_OBJS)
	$(CXX) -o $(BIN_TARGET) $(BIN_OBJS) $(LDFLAGS) -L. -lloldolist -lboost_program_options-mt -lboost_system-mt

clean:
	rm -f $(TARGET) $(PROTOC_GENS_SRC) $(PROTOC_GENS_HDR) $(OBJS) $(PROTOC_OBJS)
	rm -f $(BIN_TARGET) $(BIN_OBJS)
