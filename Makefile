.PHONY: clean

CXX := gcc
CXXFLAGS := -Wall -Wextra -Wpedantic -g
LDLIBS := -lraylib

all:
	$(CXX) $(CXXFLAGS) -o game-of-life game-of-life.c $(LDLIBS)	

clean:
	rm -rf $(TARGET) $(OBJS)

