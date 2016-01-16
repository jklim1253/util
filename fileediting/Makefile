TARGET=test

CXX=g++
CXXFLAGS=-g -Wall -std=c++11
LDFLAGS=

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

