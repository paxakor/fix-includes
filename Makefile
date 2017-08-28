SOURCES=category.cpp fix_includes.cpp main.cpp source_file.cpp util.cpp
FLAGS=-std=c++1z
CXX=g++

all: release

release:
	$(CXX) $(FLAGS) -O2 $(SOURCES) -o fix-includes

debug:
	$(CXX) $(FLAGS) -g -Og $(SOURCES) -o fix-includes-dbg
