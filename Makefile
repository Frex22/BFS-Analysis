CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O3 -fopenmp
TARGET = bfs_program

SRC = main.cpp graph.cpp bfs.cpp
OBJ = $(SRC:.cpp=.o)

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o build/$@ $^

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o build/$(TARGET)

.PHONY: all clean