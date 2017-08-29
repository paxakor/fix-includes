// Copyright 2017, Pavel Korozevtsev.

#include "fix_includes.hpp"
#include "source_file.hpp"

#include <iostream>
#include <string_view>

static void FixIncludesInFile(std::string_view src, std::string_view dst) {
    SourceFile sf;
    sf.ReadFromFile(src);
    FixIncludes(sf);
    sf.WriteToFile(dst);
}

static void FixIncludesInFile(std::string_view filename) {
    return FixIncludesInFile(filename, filename);
}

static void PrintUsage(std::string_view bin) {
    std::cout << "Usage:\n"
              << '\t' << bin << " -i [FILE]...    Inplace edit [FILE]s\n"
              << '\t' << bin << " [SRC] [DST]     Write fixed [SRC] to [DST]\n";
}

int main(int argc, const char** argv) {
    if (argc > 1)
        if (std::string_view(argv[1]) == "-i")
            for (int i = 2; i < argc; ++i)
                FixIncludesInFile(argv[i]);
        else if (argc == 3)
            FixIncludesInFile(argv[1], argv[2]);
        else
            PrintUsage(argv[0]);
    else
        PrintUsage(argv[0]);
    return 0;
}
