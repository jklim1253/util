TARGET=tiny

CXX=g++
CXXFLAGS=-g -Wall -std=c++11 -IC:\\MinGW\\boost32\\include\\boost-1_60
LDFLAGS=-LC:\\MinGW\\boost32\\lib -lboost_filesystem-mgw48-mt-1_60 -lboost_system-mgw48-mt-1_60

.PHONY: default all clean

default: $(TARGET)
all: default

OBJECTS = $(patsubst %.cpp, %.o, $(wildcard *.cpp))
HEADERS = $(wildcard *.h)

%.o: %.cpp $(HEADERS)
	$(CXX) $(CXXFLAGS) -c $< -o $@

.PRECIOUS: $(TARGET) $(OBJECTS)

$(TARGET) : $(OBJECTS)
	$(CXX) $(OBJECTS) -o $@ -Wall $(LDFLAGS)

clean :
	-rm -f *.o
	-rm -f $(TARGET)

