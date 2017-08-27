// Copyright 2017, Pavel Korozevtsev.

#include "fix_includes.hpp"
#include "source_file.hpp"
#include <iostream>

void FixIncludesInFile(std::string_view filename) {
    SourceFile sf;
    sf.ReadFromFile(filename);
    FixIncludes(sf);
    sf.WriteToFile(filename);
}

int main(int argc, const char** argv) {
    for (int i = 1; i < argc; ++i)
        FixIncludesInFile(argv[i]);
}
