```makefile
CXX = g++
CXXFLAGS = -std=c++20 -O2 -Wall -Wextra
TARGET = benchmark

all: $(TARGET)

$(TARGET): benchmark.cpp
	$(CXX) $(CXXFLAGS) -o $(TARGET) benchmark.cpp

clean:
	rm -f $(TARGET)
```