CXX 	 = g++ -std=c++20
CXXFLAGS = -O3 -pthread
DBGFLAGS = -O0 -pthread -DDEBUG -g -Wall
ALLDEFS  = -DJACOBI_SEQ -DJACOBI_STD -DJACOBI_FF

.PHONY: all debug seq std ff par clean

all:
	$(CXX) $(CXXFLAGS) $(ALLDEFS) -o jacobi main.cpp

debug:
	$(CXX) $(DBGFLAGS) $(ALLDEFS) -o jacobi main.cpp

seq:
	$(CXX) $(CXXFLAGS) -DJACOBI_SEQ -o jacobi main.cpp

std:
	$(CXX) $(CXXFLAGS) -DJACOBI_STD -o jacobi main.cpp

ff:
	$(CXX) $(CXXFLAGS) -DJACOBI_FF -o jacobi main.cpp

par:
	$(CXX) $(CXXFLAGS) -DJACOBI_STD -DJACOBI_FF -o jacobi main.cpp

clean:
	rm -f jacobi