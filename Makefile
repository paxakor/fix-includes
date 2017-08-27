all: exe

exe:
	g++ -std=c++1z -O2 category.cpp fix_includes.cpp main.cpp source_file.cpp util.cpp -o fix-includes
