CXX 	 = g++ -std=c++20
CXXFLAGS = -O3 -pthread
DBGFLAGS = -O0 -pthread -DDEBUG -g -Wall

.PHONY: all debug clean

all:
	$(CXX) $(CXXFLAGS) -o jacobi main.cpp

debug:
	$(CXX) $(DBGFLAGS) -o jacobi main.cpp

clean:
	rm -f jacobi