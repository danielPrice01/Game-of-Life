.PHONY: clean

CXX := gcc
CXXFLAGS := -Wall -Wextra -Wpedantic -g
LDLIBS := -lraylib

TARGET := game-of-life

all:
	$(CXX) $(CXXFLAGS) -o $(TARGET) game-of-life.c $(LDLIBS)	

clean:
	rm -rf $(TARGET) 

